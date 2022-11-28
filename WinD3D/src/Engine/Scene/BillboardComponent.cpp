#include <Engine/Scene/BillboardComponent.h>
#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Scene/Plane.h>

namespace ver
{
	winrt::IAsyncAction BillboardComponent::InitializeAsync(Graphics& gfx, std::filesystem::path tex_path, DirectX::XMFLOAT2 dims)
	{
		ver::scoped_timer t;
		dimensions = dims;
		Technique tech("Lambertian");
		{
			Step only{ "forward" };

			auto texture = Codex::ResolveAsync<Texture>(gfx, tex_path, 0);
			auto pshader = PixelShader::ResolveAsync(gfx, "billboard.ps.cso");
			auto vshader = VertexShader::ResolveAsync(gfx, "billboard.vs.cso");
			auto xcbuf = ver::make_shared_async<decltype(cbuf)::element_type>(gfx, 1);

			auto plane = Plane::Make();
			plane.Deform(DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat2(&dims)));

			pVertices = std::make_shared<VertexBuffer>(gfx, plane.vertices);
			pIndices = std::make_shared<IndexBuffer>(gfx, plane.indices);
			pTopology = Codex::Resolve<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


			only.AddBindable(std::make_shared<BillboardCbuf>(gfx));
			only.AddBindable(Codex::Resolve<BlendState>(gfx, true, std::nullopt));
			only.AddBindable(Codex::Resolve<Sampler>(gfx));

			only.AddBindable(cbuf = co_await xcbuf);
			only.AddBindable(std::move(co_await vshader));
			only.AddBindable(std::move(co_await pshader));
			only.AddBindable(std::move(co_await texture));
			tech.AddStep(std::move(only));
		}
		AddTechnique(std::move(tech));
	}
}