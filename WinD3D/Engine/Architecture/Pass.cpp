#include "Pass.h"
#include "Sink.h"
#include <Framework\RenderGraphException.h>
#include <Framework\Utility.h>
#include <fmt\printf.h>


namespace RG
{
	Pass::Pass(std::string name) noexcept
		:name(std::move(name))
	{}
	Pass::Pass(std::string_view name) noexcept
		:name(name)
	{}
	Pass::~Pass()
	{}

	void Pass::Reset() noxnd
	{}

	std::string_view Pass::GetName() const noexcept
	{
		return name;
	}
	std::span<const std::unique_ptr<Sink>> Pass::GetSinks() const
	{
		return sinks;
	}
	Source& Pass::GetSource(std::string_view registeredName) const
	{
		for (auto& src : sources)
		{
			if (src->GetName() == registeredName)
			{
				return *src;
			}
		}
		throw RGC_EXCEPTION(fmt::sprintf("Input named [%s] not found in pass: %s", registeredName, GetName()));
	}
	Sink& Pass::GetSink(std::string_view registeredName) const
	{
		for (auto& si : sinks)
		{
			if (si->GetRegisteredName() == registeredName)
			{
				return *si;
			}
		}
		throw RGC_EXCEPTION(fmt::sprintf("Input named [%s] not found in pass: %s", registeredName, GetName()));
	}

	void Pass::Finalize()
	{
		for (auto& in : sinks)
		{
			in->PostLinkValidate();
		}
		for (auto& out : sources)
		{
			out->PostLinkValidate();
		}
	}

	void Pass::RegisterSink(std::unique_ptr<Sink> sink)
	{
		// check for overlap of input names
		for (auto& si : sinks)
		{
			if (si->GetRegisteredName() == sink->GetRegisteredName())
			{
				throw RGC_EXCEPTION("Registered input overlaps with existing: " + sink->GetRegisteredName());
			}
		}

		sinks.push_back(std::move(sink));
	}
	void Pass::RegisterSource(std::unique_ptr<Source> source)
	{
		// check for overlap of output names
		using namespace std::string_literals;
		for (auto& src : sources)
		{
			if (src->GetName() == source->GetName())
			{
				throw RGC_EXCEPTION("Registered output overlaps with existing: "s + source->GetName().data());
			}
		}

		sources.push_back(std::move(source));
	}

	void Pass::SetSinkLinkage(std::string_view registeredName, const std::string& target)
	{
		auto& sink = GetSink(registeredName);
		auto targetSplit = SplitString(target, ".");
		if (targetSplit.size() != 2u)
		{
			throw RGC_EXCEPTION("Input target has incorrect format");
		}
		sink.SetTarget(std::move(targetSplit[0]), std::move(targetSplit[1]));
	}
}
