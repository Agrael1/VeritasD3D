#define FULL_WINOPT
#include <ShlObj.h>
#include "VFileDialog.h"


const char* VFileDialog::HRException::GetType()const noexcept
{
	return "Veritas File Exception";
}

VFileDialog::~VFileDialog()
{
}

void VFileDialog::SetFileTypes(std::span<COMDLG_FILTERSPEC> filters)
{
	HRESULT hr;
	if (!(dwFlags&FOS_STRICTFILETYPES))
	{
		VFD_THROW(pDialog->SetOptions(dwFlags | FOS_STRICTFILETYPES));
		VFD_THROW(pDialog->SetFileTypes((UINT)filters.size(), filters.data()));
	}	
}
std::wstring VFileDialog::GetFilePath()
{
	if (SUCCEEDED(pDialog->Show(NULL)))
	{
		Microsoft::WRL::ComPtr<IShellItem> psiResult;
		if (SUCCEEDED(pDialog->GetResult(&psiResult)))
		{
			PWSTR pszFilePath = NULL;
			if (SUCCEEDED(psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath)))
			{
				std::wstring _out = pszFilePath;
				CoTaskMemFree(pszFilePath);
				return _out;
			}
		}
	}
	return std::wstring();
}
void VFileDialog::SetDefaultFolder(std::wstring_view FolderPath)
{
	PIDLIST_ABSOLUTE DefItem = ILCreateFromPathW(FolderPath.data());
	if (DefItem)
	{
		Microsoft::WRL::ComPtr<IShellItem> FolderDesc;
		HRESULT hr;

		VFD_THROW(SHCreateItemFromIDList(DefItem, __uuidof(IShellItem), &FolderDesc));
		VFD_THROW(pDialog->SetFolder(FolderDesc.Get()));
		CoTaskMemFree(DefItem);
	}
	else throw std::exception("Can't find default directory");
}

VFileOpenDialog::VFileOpenDialog()
{
	HRESULT hr;
	VFD_THROW(CoCreateInstance(__uuidof(FileOpenDialog), nullptr, CLSCTX_ALL, __uuidof(IFileOpenDialog), &pDialog));
	VFD_THROW(pDialog->GetOptions(&dwFlags));
	VFD_THROW(pDialog->SetOptions(dwFlags | FOS_FORCEFILESYSTEM));
}

VFileSaveDialog::VFileSaveDialog()
{
	HRESULT hr;
	VFD_THROW(CoCreateInstance(__uuidof(FileSaveDialog), nullptr, CLSCTX_ALL, __uuidof(IFileSaveDialog), &pDialog));
	VFD_THROW(pDialog->GetOptions(&dwFlags));
	VFD_THROW(pDialog->SetOptions(dwFlags | FOS_FORCEFILESYSTEM));
}
void VFileSaveDialog::SetDefaultItem(const std::wstring& name)
{
	pDialog->SetFileName(name.c_str());
}