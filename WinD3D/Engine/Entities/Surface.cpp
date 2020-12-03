#include <Framework/WindowExceptions.h>
#include <fmt/printf.h>
#include "Surface.h"
#include <Framework/Utility.h>

// surface exception stuff
Surface::LoadException::LoadException(int line, const char* file, std::string_view filepath, std::string Note, HRESULT hr) noexcept
	:
	Exception(line, file)
{
	using namespace std::string_literals;
	note = std::move(
		"[Error]: "s + Note + "\n" 
		+ WindowException::TranslateErrorCode(hr) + "\n" 
		+ "[File]: " + filepath.data());
}
const char* Surface::LoadException::what() const noexcept
{
	whatBuffer = std::move(fmt::sprintf("%s:\n [Note]: %s ", Exception::what(), note));
	return whatBuffer.c_str();
}
const char* Surface::LoadException::GetType() const noexcept
{
	return "Graphics Loading Exception";
}
const std::string& Surface::LoadException::GetNote() const noexcept
{
	return note;
}

bool Surface::FromFile(std::string_view filepath)
{
	HRESULT hr = DirectX::LoadFromWICFile(ToWide(filepath).c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);

	if (FAILED(hr))return false;

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

		if (FAILED(hr))
		{
			throw Surface::LoadException(__LINE__, __FILE__, filepath, "Failed to convert image", hr);
		}
		image = std::move(converted);
	}
	return true;
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