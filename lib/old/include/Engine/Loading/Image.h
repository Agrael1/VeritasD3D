#pragma once
#include <DirectXTex.h>
#include <optional>
#include <string_view>

namespace ver
{
	std::optional<DirectX::ScratchImage> LoadWICTexture(std::wstring_view path, bool mips = true);
}