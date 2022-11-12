#pragma once
#include <Window.h>
#include <winrt/Windows.Storage.Pickers.h>
#include <span>
#include <pplawait.h>

namespace ver
{
	class FileOpenDialog
	{
	public:
		FileOpenDialog(const Window& wnd);
	public:
		void SetFileTypes(std::span<const std::wstring_view> types);
		concurrency::task<std::wstring> GetFilePathAsync();
		void SetDefaultFolder(std::wstring_view FolderPath) {}
	private:
		winrt::Windows::Storage::Pickers::FileOpenPicker opener;
	};
}