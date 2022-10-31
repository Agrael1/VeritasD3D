#pragma once
#include <Engine/Util/Exception.h>
#include <vector>
#include <string>

namespace GFX
{
	class GException :public Exception
	{
		using Exception::Exception;
	};
	class HrException :public GException
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> messages = {})noexcept;
	public:
		const char* what()const noexcept override;
		const char* GetType()const noexcept override;
		HRESULT GetErrorCode()const noexcept;
		std::string GetErrorString()const noexcept;
		std::string GetErrorDescription()const noexcept;
		std::string GetErrorInfo()const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType()const noexcept override;
	private:
		std::string reason;
	};
	class ContextException : public GException
	{
	public:
		ContextException(int line, const char* file, std::vector<std::string> messages = {})noexcept;
	public:
		const char* what()const noexcept override;
		const char* GetType()const noexcept override;
		std::string GetErrorInfo()const noexcept;
	private:
		std::string info;
	};
}