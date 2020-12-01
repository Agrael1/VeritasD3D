#include "pch.h"
#include "Sink.h"
#include <GraphicsThrows.h>
#include <algorithm>

namespace RG
{
	Sink::Sink(std::string registeredNameIn)
		:
		registeredName(std::move(registeredNameIn))
	{
		if (registeredName.empty())
		{
			throw RGC_EXCEPTION("Empty output name");
		}
		const bool nameCharsValid = std::all_of(registeredName.begin(), registeredName.end(), [](char c) {
			return std::isalnum(c) || c == '_';
			});
		if (!nameCharsValid || std::isdigit(registeredName.front()))
		{
			throw RGC_EXCEPTION("Invalid output name: " + registeredName);
		}
	}


	const std::string& Sink::GetRegisteredName() const noexcept
	{
		return registeredName;
	}
	const std::string& Sink::GetPassName() const noexcept
	{
		return passName;
	}
	const std::string& Sink::GetOutputName() const noexcept
	{
		return outputName;
	}

	void Sink::SetTarget(std::string xpassName, std::string xoutputName)
	{
		{
			if (xpassName.empty())
			{
				throw RGC_EXCEPTION("Empty output name");
			}
			const bool nameCharsValid = std::all_of(xpassName.begin(), xpassName.end(), [](char c) {
				return std::isalnum(c) || c == '_';
				});
			if (xpassName != "$" && (!nameCharsValid || std::isdigit(xpassName.front())))
			{
				throw RGC_EXCEPTION("Invalid pass name: " + registeredName);
			}
			this->passName = xpassName;
		}
		{
			if (xoutputName.empty())
			{
				throw RGC_EXCEPTION("Empty output name");
			}
			const bool nameCharsValid = std::all_of(xoutputName.begin(), xoutputName.end(), [](char c) {
				return std::isalnum(c) || c == '_';
				});
			if (!nameCharsValid || std::isdigit(xoutputName.front()))
			{
				throw RGC_EXCEPTION("Invalid output name: " + registeredName);
			}
			this->outputName = xoutputName;
		}
	}

}