#pragma once

// HRESULT hr should exist in the local scope for these macros to work
#define GFX_EXCEPT_NOINFO(hr) winrt::hresult_error(hr)
#define GFX_THROW_NOINFO(hrcall) winrt::check_hresult(hrcall)

#ifndef NDEBUG
#define GFX_EXCEPT(hr) winrt::hresult_error((hr), infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) GFX_EXCEPT(hr)
#define GFX_THROW_INFO_ONLY(call) do{infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) throw winrt::hresult_error(E_FAIL,v);}}while(0)
#else
#define GFX_EXCEPT(hr) winrt::hresult_error((hr))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) GFX_EXCEPT(hr)
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

// macro for importing infomanager into local scope
// this.GetInfoManager(Graphics& gfx) must exist
#ifdef NDEBUG
#define INFOMAN_NOHR(gfx)
#else
#define INFOMAN_NOHR(gfx) DXGIInfoManager& infoManager = GetInfoManager((gfx))
#endif

#ifdef NDEBUG
#define INFOMAN(gfx) HRESULT hr
#else
#define INFOMAN(gfx) HRESULT hr; INFOMAN_NOHR(gfx)
#endif

// render graph exception
#define RGC_EXCEPTION( message ) winrt::hresult_error( E_FAIL, winrt::to_hstring(message))