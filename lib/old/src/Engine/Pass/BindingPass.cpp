#include <Engine/Bindable/RenderTarget.h>
#include <Engine/Bindable/DepthStencil.h>
#include <Engine/Pass/BindingPass.h>
#include <Engine/Util/RenderGraphException.h>


namespace RG
{
	BindingPass::BindingPass(std::string name, std::vector<std::shared_ptr<Bindable>> binds)
		:
		Pass(std::move(name)),
		binds(std::move(binds))
	{}

	void BindingPass::AddBind(std::shared_ptr<Bindable> bind) noexcept
	{
		binds.push_back(std::move(bind));
	}

	void BindingPass::BindAll(Graphics& gfx) const noxnd
	{
		BindBufferResources(gfx);
		for (auto& bind : binds)
		{
			bind->Bind(gfx);
		}
	}

	void BindingPass::Finalize()
	{
		using namespace std::string_literals;
		Pass::Finalize();
		if (!renderTarget && !depthStencil)
		{
			throw RGC_EXCEPTION("BindingPass ["s + GetName().data() + "] needs at least one of a renderTarget or depthStencil");
		}
	}

	void BindingPass::BindBufferResources(Graphics& gfx) const noxnd
	{
		if (renderTarget)
		{
			renderTarget->BindAsBuffer(gfx, depthStencil.get());
		}
		else
		{
			depthStencil->BindAsBuffer(gfx);
		}
	}
}