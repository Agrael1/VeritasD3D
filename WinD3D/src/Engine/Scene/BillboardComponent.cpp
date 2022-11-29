#include <Engine/Scene/BillboardComponent.h>
#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Scene/Plane.h>
#include <Engine/Graphics.h>
#include <imgui.h>

namespace ver
{
	winrt::IAsyncAction BillboardComponent::InitializeAsync(Graphics& gfx, std::filesystem::path tex_path, DirectX::XMFLOAT2 dims, bool spherical)
	{
		Technique tech("Lambertian");
		{
			Step only{ "forward" };

			auto texture = Codex::ResolveAsync<Texture>(gfx, tex_path, 0);
			auto pshader = PixelShader::ResolveAsync(gfx, "billboard.ps.cso");
			auto vshader = VertexShader::ResolveAsync(gfx, "billboard.vs.cso");
			auto xcbuf = ver::make_shared_async<decltype(cbuf)::element_type>(gfx, buffer, 0);
			auto xvcbuf = ver::make_shared_async<VertexConstantBuffer<uint32_t>>(gfx, spherical, 1);

			auto plane = Plane::Make();
			plane.Deform(DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat2(&dims)));

			pVertices = std::make_shared<VertexBuffer>(gfx, plane.vertices);
			pIndices = std::make_shared<IndexBuffer>(gfx, plane.indices);
			pTopology = Codex::Resolve<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


			only.AddBindable(std::make_shared<BillboardCbuf>(gfx));
			only.AddBindable(Codex::Resolve<BlendState>(gfx, true, std::nullopt));
			only.AddBindable(Codex::Resolve<Sampler>(gfx));

			only.AddBindable(cbuf = co_await xcbuf);
			only.AddBindable(std::move(co_await xvcbuf));
			only.AddBindable(std::move(co_await vshader));
			only.AddBindable(std::move(co_await pshader));

			auto tex = co_await texture;
			frame_count = tex->GetCount();
			only.AddBindable(std::move(tex));
			tech.AddStep(std::move(only));
		}
		AddTechnique(std::move(tech));
	}
	void BillboardComponent::Update(Graphics& gfx)
	{
		speed = (speed + 1)%aa;
		buffer.frame = (buffer.frame + (speed == 0)) % frame_count;
		cbuf->Update(gfx, buffer);
	}
	void BillboardComponent::SpawnControlWindow() noexcept
	{
		if (ImGui::Begin((std::to_string((size_t)(this)) + "XLight").c_str()))
		{
			ImGui::Text("Position");
			ImGui::SliderFloat("X", &position.x, -200.f, 200.f, "%.1f");
			ImGui::SliderFloat("Y", &position.y, -200.f, 200.f, "%.1f");
			ImGui::SliderFloat("Z", &position.z, -200.f, 200.f, "%.1f");
		}
		ImGui::End();
	}
}