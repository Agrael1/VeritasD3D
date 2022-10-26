#pragma once
#include "Architecture/RenderGraph.h"

class Graphics;
class Bindable;
class RenderTarget;


namespace RG
{
	class DeferredRenderGraph : public RenderGraph
	{
	public:
		DeferredRenderGraph(Graphics& gfx);
	};
}