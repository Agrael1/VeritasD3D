#pragma once
#include <shtypes.h>
#include <ShlObj.h>
#include <span>
#include <filesystem>

namespace ver
{
	// Helper Class
	class VFileDialog
	{
	public:
		virtual ~VFileDialog() = default;
	public:
		void SetFileTypes(std::span<const COMDLG_FILTERSPEC> filters);
		std::filesystem::path GetFilePath()noexcept;
		void SetDefaultFolder(std::wstring_view FolderPath);
	protected:
		DWORD dwFlags = 0;
		winrt::com_ptr<IFileDialog> pDialog;
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
}

