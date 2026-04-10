#pragma once
#include <string>
#include <vector>
#include <memory>

class Graphics;
class RenderTarget;

namespace ver { class DepthStencil; }

namespace RG
{
	class Pass;
	class RenderQueuePass;
	class Source;
	class Sink;

	class RenderGraph
	{
	public:
		RenderGraph(Graphics& gfx);
		~RenderGraph();
	public:
		void Execute(Graphics& gfx) noxnd;
		void Reset() noexcept;
		RenderQueuePass& GetRenderQueue(std::string_view passName);
	protected:
		void SetSinkTarget(const std::string& sinkName, const std::string& target);
		void AddGlobalSource(std::unique_ptr<Source>);
		void AddGlobalSink(std::unique_ptr<Sink>);
		void Finalize();
		void AppendPass(std::unique_ptr<Pass> pass);
	private:
		void LinkSinks(Pass& pass);
		void LinkGlobalSinks();
	private:
		std::vector<std::unique_ptr<Pass>> passes;
		std::vector<std::unique_ptr<Source>> globalSources;
		std::vector<std::unique_ptr<Sink>> globalSinks;
		std::shared_ptr<RenderTarget> backBufferTarget;
		std::shared_ptr<ver::DepthStencil> masterDepth;
		bool finalized = false;
	};
}