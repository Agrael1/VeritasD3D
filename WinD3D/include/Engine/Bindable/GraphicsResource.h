#pragma once

struct ID3D11DeviceContext;
struct ID3D11Device;
class Graphics;

namespace ver
{
	class GraphicsResource
	{
	protected:
		static ID3D11DeviceContext* GetContext(const Graphics& gfx) noexcept;
		static ID3D11Device* GetDevice(const Graphics& gfx) noexcept;
	};
}
