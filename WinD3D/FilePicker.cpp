#include <shobjidl.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>

#include "FilePicker.h"

ver::FileOpenDialog::FileOpenDialog(const Window& wnd)
{
	opener.as<IInitializeWithWindow>()->Initialize(wnd.hWnd.get());
	opener.ViewMode(winrt::Windows::Storage::Pickers::PickerViewMode::Thumbnail);
}

void ver::FileOpenDialog::SetFileTypes(std::span<const std::wstring_view> types)
{
	std::vector<winrt::hstring> x{ types.begin(), types.end() };
	opener.FileTypeFilter().ReplaceAll(x);
}

concurrency::task<std::wstring> ver::FileOpenDialog::GetFilePathAsync()
{
	co_return ((co_await opener.PickSingleFileAsync()).Path().c_str());
}
