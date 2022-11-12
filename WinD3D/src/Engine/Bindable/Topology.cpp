#include <Engine/Bindable/Topology.h>
#include <Engine/Bindable/Codex.h>
#include <Engine/Deprecated/GraphicsThrows.h>
#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/GraphicsExceptions.h>

Topology::Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type)
	:
	type(type)
{}

void Topology::Bind(Graphics& gfx) noxnd
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->IASetPrimitiveTopology(type));
}

std::shared_ptr<Topology> Topology::Resolve(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type)
{
	return ver::Codex::Resolve<Topology>(gfx, type);
}
std::string Topology::GenerateUID(D3D11_PRIMITIVE_TOPOLOGY type)
{
	using namespace std::string_literals;
	return typeid(Topology).name() + "#"s + std::to_string(type);
}
std::string Topology::GetUID() const noexcept
{
	return GenerateUID(type);
}
