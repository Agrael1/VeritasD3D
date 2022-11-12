#pragma once
#include <Engine/Rendering/Pass.h>
#include <Engine/Rendering/SinksT.h>

class Bindable;
class IRenderTarget;
class DepthStencil;

namespace RG
{
	class BindingPass : public Pass
	{
	protected:
		BindingPass(std::string name, std::vector<std::shared_ptr<Bindable>> binds = {});
	public:
		void AddBind(std::shared_ptr<Bindable> bind) noexcept;
		void BindAll(Graphics& gfx) const noxnd;
		void Finalize() override;
	protected:
		template<class T>
		void AddBindSink(std::string name)
		{
			const auto index = binds.size();
			binds.emplace_back();
			RegisterSink(std::make_unique<ContainerBindableSink<T>>(std::move(name), binds, index));
		}
		std::shared_ptr<IRenderTarget> renderTarget;
		std::shared_ptr<DepthStencil> depthStencil;
	private:
		void BindBufferResources(Graphics& gfx) const noxnd;
	private:
		std::vector<std::shared_ptr<Bindable>> binds;
	};
}