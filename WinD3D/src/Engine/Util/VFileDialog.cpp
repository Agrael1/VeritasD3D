#include <Engine/Util/VFileDialog.h>
#include <Shared/Checks.h>

using namespace ver;

void VFileDialog::SetFileTypes(std::span<const COMDLG_FILTERSPEC> filters)
{
	if (dwFlags & FOS_STRICTFILETYPES)return;
	ver::check_hresult(pDialog->SetOptions(dwFlags | FOS_STRICTFILETYPES));
	ver::check_hresult(pDialog->SetFileTypes((UINT)filters.size(), filters.data()));
}
std::filesystem::path VFileDialog::GetFilePath()noexcept
{
	winrt::com_ptr<IShellItem> psiResult;
	wil::unique_cotaskmem_string pszFilePath;
	if (!ver::check_hresult_nothrow(pDialog->Show(nullptr)))return{};
	if (!ver::check_hresult_nothrow(pDialog->GetResult(psiResult.put())))return{};
	if (!ver::check_hresult_nothrow(psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath)))return{};
	return { pszFilePath.get() };
}
void VFileDialog::SetDefaultFolder(std::wstring_view FolderPath)
{
	winrt::com_ptr<IShellItem> FolderDesc;
	wil::unique_cotaskmem DefItem{ILCreateFromPathW(FolderPath.data())};
	if (!DefItem)throw std::exception("Can't find default directory");

	ver::check_hresult(SHCreateItemFromIDList((LPITEMIDLIST)(DefItem.get()), __uuidof(IShellItem), FolderDesc.put_void()));
	ver::check_hresult(pDialog->SetFolder(FolderDesc.get()));
}

VFileOpenDialog::VFileOpenDialog()
{
	ver::check_hresult(CoCreateInstance(__uuidof(FileOpenDialog), nullptr, CLSCTX_ALL, __uuidof(IFileOpenDialog), pDialog.put_void()));
	ver::check_hresult(pDialog->GetOptions(&dwFlags));
	ver::check_hresult(pDialog->SetOptions(dwFlags | FOS_FORCEFILESYSTEM));
}

VFileSaveDialog::VFileSaveDialog()
{
	ver::check_hresult(CoCreateInstance(__uuidof(FileSaveDialog), nullptr, CLSCTX_ALL, __uuidof(IFileSaveDialog), pDialog.put_void()));
	ver::check_hresult(pDialog->GetOptions(&dwFlags));
	ver::check_hresult(pDialog->SetOptions(dwFlags | FOS_FORCEFILESYSTEM));
}
void VFileSaveDialog::SetDefaultItem(const std::wstring& name)
{
	pDialog->SetFileName(name.c_str());
}