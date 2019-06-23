#pragma once
#define FULL_WINOPT
#include "Engine\WinSetup.h"

#include "Engine\Window.h"
#include <ShObjIdl.h>
#include <wrl.h>
#include <vector>

// Helper Class
class VFileDialog
{
public:
	class HRException : public Window::HrException
	{
	public:
		using Window::HrException::HrException;
		const char* GetType()const noexcept override;
	};
public:
	void SetFileTypes(const std::vector<COMDLG_FILTERSPEC> filters);
	std::wstring GetFilePath();
	void SetDefaultFolder(std::wstring FolderPath);
protected:
	DWORD dwFlags;
	Microsoft::WRL::ComPtr<IFileDialog> pDialog;
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
	void SetDefaultItem(const std::wstring & name);
};

#define VFD_THROW( hrcall ) if(FAILED(hr = hrcall)) {throw VFileDialog::HRException( __LINE__,__FILE__,(hr) );}