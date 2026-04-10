#pragma once

namespace RG
{
	class RenderGraph;
}

namespace ver
{
	class IDrawable
	{
	public:
		virtual ~IDrawable() = default;
		virtual void Submit() = 0;
	};
}