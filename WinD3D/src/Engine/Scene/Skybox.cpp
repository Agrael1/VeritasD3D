#include <Engine/Scene/Skybox.h>
#include <Engine/Bindable/CubeTexture.h>
#include <Engine/Bindable/AsyncResource.h>
#include <Engine/Bindable/BindableCommons.h>

winrt::IAsyncAction ver::Skybox::InitializeAsync(Graphics& gfx, std::filesystem::path tex_path)
{
	co_await winrt::resume_background();
	constexpr uint16_t a[]{ 3, 2, 6, 7, 4, 2, 0, 3, 1, 6, 5, 4, 1, 0 };

	Technique render("skybox");
	{
		Step only("forward");
		auto tex = ver::make_shared_async<CubeTexture>(gfx, std::move(tex_path));
		auto ps = PixelShader::ResolveAsync(gfx, "skybox.ps.cso");
		auto vs = VertexShader::ResolveAsync(gfx, "skybox.vs.cso");

		only.AddBindable(Stencil::Resolve(gfx, Stencil::Mode::DepthFirst));
		pIndices = IndexBuffer::Resolve(gfx, "$GlobCube", a);
		pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		only.AddBindable(std::make_shared<SkyboxTransformCbuf>(gfx));

		only.AddBindable(co_await vs);
		only.AddBindable(co_await ps);
		only.AddBindable(co_await tex);
		render.AddStep(std::move(only));
	}
	AddTechnique(std::move(render));
}
