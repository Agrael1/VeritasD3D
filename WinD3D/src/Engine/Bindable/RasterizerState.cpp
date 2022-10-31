#include <Engine/Bindable/RasterizerState.h>
#include <Engine/Deprecated/GraphicsThrows.h>
#include <Engine/Bindable/Codex.h>
#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/GraphicsExceptions.h>

RasterizerState::RasterizerState(Graphics& gfx, bool twoSided)
	:
	twoSided(twoSided)
{
	INFOMAN(gfx);

	D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
	rasterDesc.CullMode = twoSided ? D3D11_CULL_NONE : D3D11_CULL_BACK;

	GFX_THROW_INFO(GetDevice(gfx)->CreateRasterizerState(&rasterDesc, &pRasterizer));
}

void RasterizerState::Bind(Graphics& gfx) noxnd
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->RSSetState(pRasterizer.Get()));
}

std::shared_ptr<RasterizerState> RasterizerState::Resolve(Graphics& gfx, bool twoSided)
{
	return ver::Codex::Resolve<RasterizerState>(gfx, twoSided);
}
std::string RasterizerState::GenerateUID(bool twoSided)
{
	using namespace std::string_literals;
	return typeid(RasterizerState).name() + "#"s + (twoSided ? "2s" : "1s");
}
std::string RasterizerState::GetUID() const noexcept
{
	return GenerateUID(twoSided);
}