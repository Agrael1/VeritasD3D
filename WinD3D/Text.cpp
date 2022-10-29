#include "Text.h"
#include "ttfcpp.h"
#include "Architecture/BindableCommons.h"
#include "Scene/IndexedTriangleList.h"
#include <ranges> 
#include <span> 


using namespace ver;

Text::Text(Graphics& gfx, float radius)
	:colorBuffer(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst, 1u))
{
	//auto x = ttf_list_system_fonts(nullptr);
	//int id = ttf_list_match_id(x, "f", "Arial");
	//auto t = x[id];
	ttf::ttf_bridge br{ "C:\\Windows\\fonts\\arial.ttf"};
	auto gl = br.find_glyph(u'ﷺ');


	auto xmodel = gl.to_mesh3d(15, 0.1);
	auto lay = DV::VertexLayout{}
		+ DV::VertexLayout::ElementType::Position3D;


	auto& mo = xmodel.get();
	auto vbuf = DV::VertexBuffer{ lay };


	for (auto&& i : std::span<DirectX::XMFLOAT3>(reinterpret_cast<DirectX::XMFLOAT3*>(mo.vert), mo.nvert))
		vbuf.EmplaceBack(i);

	namespace dx = DirectX;
	const auto geometryTag = "$stext." + std::to_string(radius);

	std::vector<uint16_t> a{}; 

	for (size_t i = 0; i < static_cast<unsigned long long>(mo.nfaces) * 3; i++)
	{
		a.push_back(reinterpret_cast<int*>(mo.faces)[i]);
	}

	IndexedTriangleList model{ vbuf,  a};


	//model.Deform(dx::XMMatrixScalingFromVector(dx::XMVectorReplicate(radius)));
	pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
	pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
	pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		Technique solid;
		Step only("lambertian");

		auto pvs = VertexShader::Resolve(gfx, "solid.vs.cso");
		auto pvsbc = pvs->GetBytecode();
		only.AddBindable(std::move(pvs));

		only.AddBindable(PixelShader::Resolve(gfx, "solid.ps.cso"));


		only.AddBindable(colorBuffer);
		only.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));
		only.AddBindable(std::make_shared<TransformUnified>(gfx));

		solid.AddStep(std::move(only));
		AddTechnique(std::move(solid));
	}
}

void Text::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}
void Text::SetColor(DirectX::XMFLOAT3 color) noexcept
{
	colorConst.color = color;
}
void Text::UpdateColor(Graphics& gfx) noexcept
{
	colorBuffer->Update(gfx, colorConst);
}
DirectX::XMMATRIX Text::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}
