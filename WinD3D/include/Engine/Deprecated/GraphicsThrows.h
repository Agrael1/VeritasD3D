#pragma once

// HRESULT hr should exist in the local scope for these macros to work

#define GFX_EXCEPT_NOINFO(hr) ver::make_error<ver::hr_error>(hr)
#define GFX_THROW_NOINFO(hrcall) ver::check_hresult(hrcall)

#ifndef NDEBUG
#define GFX_EXCEPT(hr) ver::make_error<ver::hr_error>({hr, infoManager.GetMessages()})
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(xhr) ver::make_error<ver::DeviceRemovedException>({xhr,infoManager.GetMessages()})
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw ver::make_error<ver::ContextException>(std::move(v));}}
#else
#define GFX_EXCEPT(hr) GFX_EXCEPT_NOINFO(hr)
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) ver::make_error<ver::DeviceRemovedException>(hr)
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

// macro for importing infomanager into local scope
// this.GetInfoManager(Graphics& gfx) must exist
#ifdef NDEBUG
#define INFOMAN_NOHR(gfx)
#else
#define INFOMAN_NOHR(gfx) DXGIInfoManager& infoManager = *GetInfoManager((gfx))
#endif

#ifdef NDEBUG
#define INFOMAN(gfx) HRESULT hr
#else
#define INFOMAN(gfx) HRESULT hr; INFOMAN_NOHR(gfx)
#endif