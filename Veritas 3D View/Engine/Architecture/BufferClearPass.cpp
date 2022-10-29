#include "pch.h"
#include "BufferClearPass.h"
#include "SinksT.h"
#include "SourcesT.h"

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