#include "pch.h"
#include "Surface.h"
#include <Framework/Utility.h>


Surface::Surface(std::string_view filepath)
{
	winrt::check_hresult(DirectX::LoadFromWICFile(ToWide(filepath).c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image));

	if (image.GetImage(0, 0, 0)->format != format)
	{
		DirectX::ScratchImage converted;
		winrt::check_hresult(DirectX::Convert(
			*image.GetImage(0, 0, 0),
			format,
			DirectX::TEX_FILTER_DEFAULT,
			DirectX::TEX_THRESHOLD_DEFAULT,
			converted
		));
		image = std::move(converted);
	}
}

UINT Surface::GetWidth()const noexcept
{
	return (UINT)image.GetMetadata().width;
}
UINT Surface::GetHeight()const noexcept
{
	return (UINT)image.GetMetadata().height;
}
UINT Surface::GetStride()const noexcept
{
	return (UINT)image.GetPixelsSize() / GetHeight();
}
bool Surface::UsesAlpha()const noexcept
{
	return !image.IsAlphaAllOpaque();
}
void* Surface::GetBufferPtr()const noexcept
{
	return image.GetPixels();
}