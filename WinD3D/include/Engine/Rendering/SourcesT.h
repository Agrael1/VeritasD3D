#pragma once
#include "Source.h"
#include <Engine/Util/RenderGraphException.h>

namespace RG
{
	template<class T>
	class DirectBufferSource : public Source
	{
	public:
		static std::unique_ptr<DirectBufferSource> Make(std::string name, std::shared_ptr<T>& buffer)
		{
			return std::make_unique<DirectBufferSource>(std::move(name), buffer);
		}
		DirectBufferSource(std::string name, std::shared_ptr<T>& buffer)
			:
			Source(std::move(name)),
			buffer(buffer)
		{}
		void PostLinkValidate() const
		{}
		std::shared_ptr<BufferResource> YieldBuffer() override
		{
			using namespace std::string_literals;
			if (linked)
			{
				throw RGC_EXCEPTION("Mutable output bound twice: "s + GetName().data());
			}
			linked = true;
			return buffer;
		}
	private:
		std::shared_ptr<T>& buffer;
		bool linked = false;
	};

	template<class T>
	class DirectBindableSource : public Source
	{
	public:
		static std::unique_ptr<DirectBindableSource> Make(std::string name, std::shared_ptr<T>& buffer)
		{
			return std::make_unique<DirectBindableSource>(std::move(name), buffer);
		}
		DirectBindableSource(std::string name, std::shared_ptr<T>& bind)
			:
			Source(std::move(name)),
			bind(bind)
		{}
		void PostLinkValidate() const
		{}
		std::shared_ptr<Bindable> YieldBindable() override
		{
			return bind;
		}
	private:
		std::shared_ptr<T>& bind;
	};
}

namespace ver::rg
{
	using RG::DirectBufferSource;
}