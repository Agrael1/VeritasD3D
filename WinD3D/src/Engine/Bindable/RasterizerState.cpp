#include <Engine/Bindable/RasterizerState.h>
#include <Engine/Bindable/Codex.h>
#include <Shared/Checks.h>

RasterizerState::RasterizerState(Graphics& gfx, bool twoSided)
	:
	twoSided(twoSided)
{
	D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
	rasterDesc.CullMode = twoSided ? D3D11_CULL_NONE : D3D11_CULL_BACK;

	ver::check_hresult(GetDevice(gfx)->CreateRasterizerState(&rasterDesc, &pRasterizer));
}

void RasterizerState::Bind(Graphics& gfx) noxnd
{
	GetContext(gfx)->RSSetState(pRasterizer.Get());
	ver::check_context();
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