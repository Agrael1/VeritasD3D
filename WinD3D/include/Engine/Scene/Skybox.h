#pragma once
#include <Engine/Rendering/Drawable.h>
#include <filesystem>


namespace ver
{
	class Skybox : public Drawable
	{
	public:
		Skybox() = default;
		ver::IAsyncAction InitializeAsync(Graphics& gfx, std::filesystem::path tex_path, bool rot = true);
	public:
		virtual DirectX::XMMATRIX GetTransformXM() const noexcept override { return DirectX::XMMatrixIdentity(); }
	};
}