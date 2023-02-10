#pragma once
#include <Engine/Bindable/Bindable.h>

struct ID3D11DepthStencilState;

namespace ver
{
	class Stencil : public Bindable
	{
	public:
		enum class Mode
		{
			Off,
			Write,
			Mask,
			DepthFirst
		};
		Stencil(Graphics& gfx, Mode mode);
	public:
		void Bind(Graphics& gfx) noxnd override;
		void Bind(ID3D11DeviceContext& context) noxnd;
		static std::shared_ptr<Stencil> Resolve(Graphics& gfx, Mode mode);
		static std::string GenerateUID(Mode mode);
		std::string GetUID() const noexcept override;
	private:
		winrt::com_ptr<ID3D11DepthStencilState> pStencil;
		Mode mode;
	};
}
