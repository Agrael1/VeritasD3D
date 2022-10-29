#pragma once
#include "RenderGraph.h"

class Graphics;
class Bindable;
class RenderTarget;


namespace RG
{
	class GalaxyRenderGraph : public RenderGraph
	{
	public:
		GalaxyRenderGraph(Graphics& gfx);
	};
}