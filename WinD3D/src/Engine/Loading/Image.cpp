#include <Engine/Loading/Image.h>

using namespace ver;

inline constexpr DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;

std::optional<DirectX::ScratchImage> ver::LoadWICTexture(std::wstring_view path, bool mips)
{
	DirectX::ScratchImage image;
	HRESULT hr = (DirectX::LoadFromWICFile(path.data(), DirectX::WIC_FLAGS_NONE, nullptr, image));
	if (hr < 0) return{};

	if (image.GetImage(0, 0, 0)->format != format)
	{
		DirectX::ScratchImage converted;
		hr = DirectX::Convert(
			*image.GetImage(0, 0, 0),
			format,
			DirectX::TEX_FILTER_DEFAULT,
			DirectX::TEX_THRESHOLD_DEFAULT,
			converted
		);

		if (hr < 0) return{};
		image = std::move(converted);
	}
	if (!mips)return image;

	DirectX::ScratchImage mipped;
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(),
		image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipped);
	if (hr < 0) return {};
	return mipped;
}
