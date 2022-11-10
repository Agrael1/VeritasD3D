#pragma once
#include <Engine/Util/WindowExceptions.h>
#include <shtypes.h>
#include <wrl.h>
#include <span>

// Helper Class
class VFileDialog
{
public:
	~VFileDialog();
public:
	void SetFileTypes(std::span<const COMDLG_FILTERSPEC> filters);
	std::wstring GetFilePath();
	void SetDefaultFolder(std::wstring_view FolderPath);
protected:
	DWORD dwFlags;
	Microsoft::WRL::ComPtr<struct IFileDialog> pDialog;
};

class VFileOpenDialog : public VFileDialog
{
public:
	VFileOpenDialog();
};

class VFileSaveDialog : public VFileDialog
{
public:
	VFileSaveDialog();
	void SetDefaultItem(const std::wstring& name);
};

#define VFD_THROW( hrcall ) if(FAILED(hr = hrcall)) {throw ver::make_error<ver::hr_error>( {(hr)} );}