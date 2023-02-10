#include <Engine/Pass/BufferClearPass.h>
#include <Engine/Rendering/SinksT.h>
#include <Engine/Rendering/SourcesT.h>
#include <Engine/Bindable/BufferResource.h>

namespace RG
{
	BufferClearPass::BufferClearPass(std::string name)
		:
		Pass(std::move(name))
	{
		RegisterSink(DirectBufferSink<BufferResource>::Make("buffer", buffer));
		RegisterSource(DirectBufferSource<BufferResource>::Make("buffer", buffer));
	}

	void BufferClearPass::Execute(Graphics& gfx) const noxnd
	{
		buffer->Clear(gfx);
	}
}