#pragma once
#include <Engine/Rendering/RenderGraph.h>

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