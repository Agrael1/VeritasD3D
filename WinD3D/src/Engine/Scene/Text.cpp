#include <Engine/Scene/Text.h>
#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Scene/IndexedTriangleList.h>
#include <ranges> 
#include <span> 
#include "../../../../ttf2mesh/ttfcpp.h"
#include <unordered_map>

class ttfcached
{
public:
	struct text3d
	{
		std::vector<DirectX::XMFLOAT3> vertices;
		std::vector<float> indices;
		std::vector<int> normals;
	};
public:
	ttfcached(std::filesystem::path font) :bridge(font) {}
public:
	const ttf::mesh3d& get_glyph3d(char16_t c, uint8_t quality = 15, float depth = 0.1f)const
	{
		if (auto x = cache.find(c); x != cache.end())
			return x->second;
		ttf::glyph_view vi = bridge.find_glyph(c);
		return cache.emplace(c, vi.to_mesh3d(quality, depth)).first->second;
	}
	// no newlines
	text3d get_text3d(std::u16string_view string, uint8_t quality = 15, float depth = 0.1f)const
	{
		text3d text;
		float x_offset = 0;
		float y_offset = 0;
		int i_offset = 0;

		auto& v = text.vertices;
		auto& i = text.indices;
		auto& n = text.normals;
		for (auto c : string)
		{

			if (c == u'\n')
			{
				y_offset -= bridge.get_line_space();
				x_offset = 0;
				continue;
			}

			auto& glyph = get_glyph3d(c, quality, depth);
			auto& info = glyph.get_glyph();

			if (glyph)
			{
				auto vx = glyph.get_vertices_xm();
				auto ix = glyph.get_indices();
				auto nx = glyph.get_normals();

				v.reserve(v.size() + vx.size());
				i.reserve(i.size() + ix.size());
				n.reserve(n.size() + nx.size());

				std::ranges::transform(vx, std::back_inserter(v), 
					[x_offset, y_offset](DirectX::XMFLOAT3 a) ->DirectX::XMFLOAT3 {return { a.x + x_offset,a.y + y_offset, a.z }; });
				std::ranges::transform(ix, std::back_inserter(i), [i_offset](int a) {return a + i_offset; });
				n.insert(n.end(), nx.begin(), nx.end());
				i_offset += glyph.get().nvert;
			}
			x_offset += info.advance;
		}
		return text;
	}
public:
	ttf::ttf_bridge bridge;
	mutable std::unordered_map<char16_t, ttf::mesh3d> cache;
};


using namespace ver;

Text::Text(Graphics& gfx, float radius)
	:colorBuffer(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst, 1u))
{
	ttfcached br{ "C:\\Windows\\fonts\\arial.ttf" };

	auto xmodel = br.get_text3d(u"Hello, world!\nBUILTWAY\nčesko řř ů¨¨\nMaelström", 15, 0.1f);
	auto lay = ver::dv::VertexLayout{ ver::dv::ElementType::Position3D };
	lay.append(ver::dv::ElementType::Normal);

	namespace dx = DirectX;
	const auto geometryTag = "$stext." + std::to_string(radius);
	std::array<void*, 2> data{xmodel.vertices.data(), xmodel.normals.data()};

	std::vector<uint16_t> a{};

	for (auto i : xmodel.indices)
		a.push_back(uint16_t(i));

	//model.Deform(dx::XMMatrixScalingFromVector(dx::XMVectorReplicate(radius)));
	pVertices = std::make_shared<VertexMultibuffer>(gfx, lay, data, xmodel.vertices.size());
	pIndices = IndexBuffer::Resolve(gfx, geometryTag, a);
	pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		Technique solid;
		Step only("lambertian");

		auto pvs = VertexShader::Resolve(gfx, "solid.vs.cso");
		auto pvsbc = pvs->GetBytecode();
		only.AddBindable(std::move(pvs));

		only.AddBindable(PixelShader::Resolve(gfx, "solid.ps.cso"));


		only.AddBindable(colorBuffer);
		only.AddBindable(InputLayout::Resolve(gfx, lay, pvsbc));
		only.AddBindable(std::make_shared<TransformCbuf>(gfx));

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
