#pragma once
#include "Sink.h"
#include <memory>
#include <GraphicsThrows.h>
#include <fmt\printf.h>
#include "BufferResource.h"

namespace RG
{
	template<class T>
	class DirectBufferSink : public Sink
	{
		static_assert(std::is_base_of_v<BufferResource, T>, "DirectBufferSink target type must be a BufferResource type");
	public:
		static std::unique_ptr<Sink> Make(std::string registeredName, std::shared_ptr<T>& target)
		{
			return std::make_unique<DirectBufferSink>(std::move(registeredName), target);
		}
		void PostLinkValidate() const override
		{
			if (!linked)
			{
				throw RGC_EXCEPTION("Unlinked input: " + GetRegisteredName());
			}
		}
		void Bind(Source& source) override
		{
			auto p = std::dynamic_pointer_cast<T>(source.YieldBuffer());
			if (!p)
			{
				auto msg = fmt::sprintf("Binding input[%s] to output [%s.%s] { %s } not compatible with { %s }",
					GetRegisteredName(),
					GetPassName(),
					GetOutputName(),
					typeid(T).name(),
					typeid(*source.YieldBuffer().get()).name());
				throw RGC_EXCEPTION(msg);
			}
			target = std::move(p);
			linked = true;
		}
		DirectBufferSink(std::string registeredName, std::shared_ptr<T>& bind)
			:
			Sink(std::move(registeredName)),
			target(bind)
		{}
	private:
		std::shared_ptr<T>& target;
		bool linked = false;
	};

	template<class T>
	class ContainerBindableSink : public Sink
	{
		static_assert(std::is_base_of_v<Bindable, T>, "DirectBindableSink target type must be a Bindable type");
	public:
		void PostLinkValidate() const override
		{
			if (!linked)
			{
				throw RGC_EXCEPTION("Unlinked input: " + GetRegisteredName());
			}
		}
		void Bind(Source& source) override
		{
			auto p = std::dynamic_pointer_cast<T>(source.YieldBindable());
			if (!p)
			{
				auto msg = fmt::sprintf("Binding input[%s] to output [%s.%s] { %s } not compatible with { %s }",
					GetRegisteredName(),
					GetPassName(),
					GetOutputName(),
					typeid(T).name(),
					typeid(*source.YieldBuffer().get()).name());
				throw RGC_EXCEPTION(msg);
			}
			container[index] = std::move(p);
			linked = true;
		}
		ContainerBindableSink(std::string registeredName, std::vector<std::shared_ptr<Bindable>>& container, size_t index)
			:
			Sink(std::move(registeredName)),
			container(container),
			index(index)
		{}
	private:
		std::vector<std::shared_ptr<Bindable>>& container;
		size_t index;
		bool linked = false;
	};

	template<class T>
	class DirectBindableSink : public Sink
	{
		static_assert(std::is_base_of_v<Bindable, T>, "DirectBindableSink target type must be a Bindable type");
	public:
		static std::unique_ptr<Sink> Make(std::string registeredName, std::shared_ptr<T>& target)
		{
			return std::make_unique<DirectBindableSink>(std::move(registeredName), target);
		}
		void PostLinkValidate() const override
		{
			if (!linked)
			{
				throw RGC_EXCEPTION("Unlinked input: " + GetRegisteredName());
			}
		}
		void Bind(Source& source) override
		{
			auto p = std::dynamic_pointer_cast<T>(source.YieldBindable());
			if (!p)
			{
				auto msg = fmt::sprintf("Binding input[%s] to output [%s.%s] { %s } not compatible with { %s }",
					GetRegisteredName(),
					GetPassName(),
					GetOutputName(),
					typeid(T).name(),
					typeid(*source.YieldBuffer().get()).name());
				throw RGC_EXCEPTION(msg);
			}
			target = std::move(p);
			linked = true;
		}
		DirectBindableSink(std::string registeredName, std::shared_ptr<T>& target)
			:
			Sink(std::move(registeredName)),
			target(target)
		{}
	private:
		std::shared_ptr<T>& target;
		bool linked = false;
	};
}