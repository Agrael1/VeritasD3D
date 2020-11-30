#pragma once
#include "Framework/Exception.h"
#include "Framework/WinSetup.h"
#include <wil/win32_helpers.h>

class WindowException :public Exception
{
	using Exception::Exception;
public:
	static std::string TranslateErrorCode(HRESULT hr)noexcept;
};
class HrException :public WindowException
{
public:
	HrException(int line, const char* file, HRESULT hr);
	const char* what() const noexcept override;
	const char* GetType()const noexcept override;
	HRESULT GetErrorCode() const noexcept;
	std::string GetErrorDescription() const noexcept;
private:
	HRESULT hResult;
};
class NoGfxException :public WindowException
{
public:
	using WindowException::WindowException;
	const char* GetType()const noexcept override;
};

#define WND_EXCEPT( hr ) HrException( __LINE__,__FILE__,(hr) )
#define WND_LAST_EXCEPT() HrException( __LINE__,__FILE__,GetLastError() )
#define WND_NOGFX_EXCEPT() NoGfxException( __LINE__,__FILE__ )
#define WND_CALL_INFO(call) if(!(call)) throw(WND_LAST_EXCEPT())