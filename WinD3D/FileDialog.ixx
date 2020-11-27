module;
#define FULL_WINOPT
#include "Engine\Window.h"
#include <ShlObj.h>
#include <string>
#include <wrl.h>
#include <span>
#define VFD_THROW( hrcall ) if(FAILED(hr = hrcall)) {throw HRException( __LINE__,__FILE__,(hr) );}
export module VFileDialog;

export COMDLG_FILTERSPEC;

// Helper Class
export class VFileDialog
{
public:

public:
	class HRException : public Window::HrException
	{
	public:
		using Window::HrException::HrException;
		const char* GetType()const noexcept override
		{
			return "Veritas File Exception";
		}
	};
public:
	void SetFileTypes(std::span<COMDLG_FILTERSPEC> filters)
	{
		HRESULT hr;
		if (!(dwFlags & FOS_STRICTFILETYPES))
		{
			VFD_THROW(pDialog->SetOptions(dwFlags | FOS_STRICTFILETYPES));
			VFD_THROW(pDialog->SetFileTypes((UINT)filters.size(), filters.data()));
		}
	}
	std::wstring GetFilePath()
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
	void SetDefaultFolder(std::wstring_view FolderPath)
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
protected:
	DWORD dwFlags;
	Microsoft::WRL::ComPtr<IFileDialog> pDialog;
};

export class VFileOpenDialog : public VFileDialog
{
public:
	VFileOpenDialog()
	{
		HRESULT hr;
		VFD_THROW(CoCreateInstance(__uuidof(FileOpenDialog), nullptr, CLSCTX_ALL, __uuidof(IFileOpenDialog), &pDialog));
		VFD_THROW(pDialog->GetOptions(&dwFlags));
		VFD_THROW(pDialog->SetOptions(dwFlags | FOS_FORCEFILESYSTEM));
	}
};

export class VFileSaveDialog : public VFileDialog
{
public:
	VFileSaveDialog()
	{
		HRESULT hr;
		VFD_THROW(CoCreateInstance(__uuidof(FileSaveDialog), nullptr, CLSCTX_ALL, __uuidof(IFileSaveDialog), &pDialog));
		VFD_THROW(pDialog->GetOptions(&dwFlags));
		VFD_THROW(pDialog->SetOptions(dwFlags | FOS_FORCEFILESYSTEM));
	}
	void SetDefaultItem(std::wstring_view name)
	{
		pDialog->SetFileName(name.data());
	}
};