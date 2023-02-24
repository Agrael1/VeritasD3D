#include <Game/MediaEngine.h>
#include <wil/com.h>
#include <filesystem>
#include <Engine/Util/GraphicsExceptions.h>

class MediaEngineNotify : public winrt::implements< MediaEngineNotify, IMFMediaEngineNotify >
{
	IMFNotify* m_pCB = nullptr;
public:
	MediaEngineNotify() = default;
public:
	void SetCallback(IMFNotify* pCB)
	{
		m_pCB = pCB;
	}

	// EventNotify is called when the Media Engine sends an event.
	STDMETHODIMP EventNotify(DWORD meEvent, DWORD_PTR param1, DWORD)
	{
		if (meEvent == MF_MEDIA_ENGINE_EVENT_NOTIFYSTABLESTATE)
		{
			SetEvent(reinterpret_cast<HANDLE>(param1));
		}
		else
		{
			m_pCB->OnMediaEngineEvent(meEvent);
		}

		return S_OK;
	}
};

void MediaEnginePlayer::Initialize()
{
	// Initialize Media Foundation (see Main.cpp for code to handle Windows N Editions)
	winrt::check_hresult(MFStartup(MF_VERSION));

	winrt::com_ptr<ID3D11Device>        device;
	winrt::com_ptr<ID3D11DeviceContext> context;
	winrt::check_hresult(
		D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			D3D11_CREATE_DEVICE_VIDEO_SUPPORT | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			device.put(),
			nullptr,
			context.put()
		));

	m_device = device.as<ID3D11Device3>();
	m_context = context.as<ID3D11DeviceContext1>();

	winrt::com_ptr<ID3D11Multithread> multithreaded = m_device.as<ID3D11Multithread>();
	multithreaded->SetMultithreadProtected(TRUE);

	// Setup Media Engine
	winrt::com_ptr<IMFDXGIDeviceManager> dxgiManager;
	UINT resetToken;
	winrt::check_hresult(MFCreateDXGIDeviceManager(&resetToken, dxgiManager.put()));
	winrt::check_hresult(dxgiManager->ResetDevice(m_device.get(), resetToken));

	// Create our event callback object.
	auto spNotify = winrt::make_self<MediaEngineNotify>();

	spNotify->SetCallback(this);

	// Set configuration attribiutes.
	winrt::com_ptr<IMFAttributes> attributes;
	winrt::check_hresult(MFCreateAttributes(attributes.put(), 1));
	winrt::check_hresult(attributes->SetUnknown(MF_MEDIA_ENGINE_DXGI_MANAGER, reinterpret_cast<IUnknown*>(dxgiManager.get())));
	winrt::check_hresult(attributes->SetUnknown(MF_MEDIA_ENGINE_CALLBACK, reinterpret_cast<IUnknown*>(spNotify.get())));
	winrt::check_hresult(attributes->SetUINT32(MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT, DXGI_FORMAT_B8G8R8A8_UNORM));

	// Create MediaEngine.
	winrt::com_ptr<IMFMediaEngineClassFactory> mfFactory;
	winrt::check_hresult(
		CoCreateInstance(CLSID_MFMediaEngineClassFactory,
			nullptr,
			CLSCTX_ALL, __uuidof(IMFMediaEngineClassFactory),
			mfFactory.put_void()));

	winrt::com_ptr<IMFMediaEngine>      xmediaEngine;
	winrt::check_hresult(
		mfFactory->CreateInstance(0,
			attributes.get(),
			xmediaEngine.put()));

	m_mediaEngine = xmediaEngine.as<IMFMediaEngineEx>();
}


void MediaEnginePlayer::Initialize(winrt::com_ptr<ID3D11Device> device, winrt::com_ptr<ID3D11DeviceContext> context)
{
	// Initialize Media Foundation (see Main.cpp for code to handle Windows N Editions)
	winrt::check_hresult(MFStartup(MF_VERSION));

	m_device = device.as<ID3D11Device3>();
	m_context = context.as<ID3D11DeviceContext1>();

	winrt::com_ptr<ID3D11Multithread> multithreaded = m_device.as<ID3D11Multithread>();
	multithreaded->SetMultithreadProtected(TRUE);

	// Setup Media Engine
	winrt::com_ptr<IMFDXGIDeviceManager> dxgiManager;
	UINT resetToken;
	winrt::check_hresult(MFCreateDXGIDeviceManager(&resetToken, dxgiManager.put()));
	winrt::check_hresult(dxgiManager->ResetDevice(m_device.get(), resetToken));

	// Create our event callback object.
	auto spNotify = winrt::make_self<MediaEngineNotify>();

	spNotify->SetCallback(this);

	// Set configuration attribiutes.
	winrt::com_ptr<IMFAttributes> attributes;
	winrt::check_hresult(MFCreateAttributes(attributes.put(), 1));
	winrt::check_hresult(attributes->SetUnknown(MF_MEDIA_ENGINE_DXGI_MANAGER, reinterpret_cast<IUnknown*>(dxgiManager.get())));
	winrt::check_hresult(attributes->SetUnknown(MF_MEDIA_ENGINE_CALLBACK, reinterpret_cast<IUnknown*>(spNotify.get())));
	winrt::check_hresult(attributes->SetUINT32(MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT, DXGI_FORMAT_B8G8R8A8_UNORM));

	// Create MediaEngine.
	winrt::com_ptr<IMFMediaEngineClassFactory> mfFactory;
	winrt::check_hresult(
		CoCreateInstance(CLSID_MFMediaEngineClassFactory,
			nullptr,
			CLSCTX_ALL, __uuidof(IMFMediaEngineClassFactory),
			mfFactory.put_void()));

	winrt::com_ptr<IMFMediaEngine>      xmediaEngine;
	winrt::check_hresult(
		mfFactory->CreateInstance(0,
			attributes.get(),
			xmediaEngine.put()));

	m_mediaEngine = xmediaEngine.as<IMFMediaEngineEx>();
}

void MediaEnginePlayer::Shutdown()
{
	if (m_mediaEngine) m_mediaEngine->Shutdown();
}

void MediaEnginePlayer::Play()
{
	if (m_isPlaying)
		return;

	if (m_mediaEngine)
	{
		winrt::check_hresult(m_mediaEngine->Play());
		m_isPlaying = true;
		m_isFinished = false;
	}
}
void MediaEnginePlayer::SetStereo(bool stereo) {
	if (m_mediaEngine)
		winrt::check_hresult(m_mediaEngine->SetStereo3DRenderMode(MF3DVideoOutputType(stereo)));

	auto m = MF_MEDIA_ENGINE_S3D_PACKING_MODE::MF_MEDIA_ENGINE_S3D_PACKING_MODE_SIDE_BY_SIDE;
	m_mediaEngine->GetStereo3DFramePackingMode(&m);
}
void MediaEnginePlayer::SetMuted(bool muted)
{
	if (m_mediaEngine)
		winrt::check_hresult(m_mediaEngine->SetMuted(muted));
}

void MediaEnginePlayer::SetSource(std::wstring_view sourceUri)
{
	wil::unique_bstr bstrURL = wil::make_bstr_failfast(sourceUri.data());

	m_isPlaying = false;
	m_isInfoReady = false;
	m_isFinished = false;
	if (!m_mediaEngine)return;
	if (!std::filesystem::exists(sourceUri))return; //early return

	winrt::check_hresult(m_mediaEngine->SetSource(bstrURL.get()));
	winrt::check_hresult(m_mediaEngine->Load());

	while (!IsInfoReady())
		SwitchToThread();


	//create texture resource
	D3D11_TEXTURE2D_DESC texDesc = {};
	GetNativeVideoSize(width, height);
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

	ver::check_graphics(m_device->CreateTexture2D(
		&texDesc,
		nullptr,
		m_texture.put()));


	//TBD
	//D3D11_TEXTURE2D_DESC texDesc1 = {};
	//texDesc1.Width = width;
	//texDesc1.Height = height;
	//texDesc1.MipLevels = 0;
	//texDesc1.ArraySize = 1;
	//texDesc1.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
	//texDesc1.SampleDesc.Count = 1;
	//texDesc1.SampleDesc.Quality = 0;
	//texDesc1.Usage = D3D11_USAGE_DEFAULT;
	//texDesc1.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//texDesc1.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	//texDesc1.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
	//
	//m_device->CreateTexture2D(
	//	&texDesc1,
	//	nullptr,
	//	Show.put());
	//CD3D11_SHADER_RESOURCE_VIEW_DESC rtvDesc(D3D11_SRV_DIMENSION_TEXTURE2D,
	//	texDesc.Format);
	//ver::check_graphics(m_device->CreateShaderResourceView(Show.get(), &rtvDesc, srv.put()));
}

bool MediaEnginePlayer::TransferFrame(RECT rcTarget)
{
	MFVideoNormalizedRect rect = { 0.0f, 0.0f, 1.0f,1.0f };
	if (m_mediaEngine && m_isPlaying)
	{
		LONGLONG pts;
		if (m_mediaEngine->OnVideoStreamTick(&pts) == S_OK)
		{
			// The texture must have been created with D3D11_RESOURCE_MISC_SHARED
			HANDLE textureHandle;
			winrt::check_hresult(m_texture.as<IDXGIResource>()->GetSharedHandle(&textureHandle));

			winrt::com_ptr<ID3D11Texture2D> mediaTexture;
			if (SUCCEEDED(m_device->OpenSharedResource(textureHandle, __uuidof(ID3D11Texture2D), mediaTexture.put_void())))
			{
				if (m_mediaEngine->TransferVideoFrame(mediaTexture.get(), &rect, &rcTarget, &m_bkgColor) == S_OK)
				{
					//copy the render target texture to the readable texture.
					m_context->CopySubresourceRegion1(Show.get(), 0, 0, 0, 0, mediaTexture.get(), 0, NULL, 0);
					m_context->Flush();


					//winrt::check_hresult(m_context->Map(Show.get(), 0, D3D11_MAP_READ, 0, &mappedResource));
					//m_context->Unmap(Show.get(), 0);
				}
				return true;
			}
		}
	}
	return false;
}
void MediaEnginePlayer::TransferDirect(winrt::com_ptr<ID3D11ShaderResourceView> srv)
{
	RECT rcTarget{ 0,0, width, height };
	MFVideoNormalizedRect rect = { 0.0f, 0.0f, 1.0f,1.0f };
	LONGLONG pts;
	if (!m_mediaEngine || !m_isPlaying || m_mediaEngine->OnVideoStreamTick(&pts) != S_OK)
		return;

	// The texture must have been created with D3D11_RESOURCE_MISC_SHARED
	HANDLE textureHandle;
	winrt::check_hresult(m_texture.as<IDXGIResource>()->GetSharedHandle(&textureHandle));

	winrt::com_ptr<ID3D11Texture2D> mediaTexture;
	if (SUCCEEDED(m_device->OpenSharedResource(textureHandle, __uuidof(ID3D11Texture2D), mediaTexture.put_void())))
	{
		if (m_mediaEngine->TransferVideoFrame(mediaTexture.get(), &rect, &rcTarget, &m_bkgColor) == S_OK)
		{
			winrt::com_ptr<ID3D11Resource> xmediaTexture;srv->GetResource(xmediaTexture.put());
			//copy the render target texture to the readable texture.
			m_context->CopySubresourceRegion1(xmediaTexture.get(), 0, 0, 0, 0, mediaTexture.get(), 0, NULL,0);
			ver::check_context();
			m_context->Flush();

			//winrt::check_hresult(m_context->Map(Show.get(), 0, D3D11_MAP_READ, 0, &mappedResource));
			//m_context->Unmap(Show.get(), 0);
		}
	}

	//if (m_mediaEngine->TransferVideoFrame(m_texture.get(), &rect, &rcTarget, &m_bkgColor) == S_OK)
	//{
	//	printf("frame extracted\n");
	//
	//}


	//// The texture must have been created with D3D11_RESOURCE_MISC_SHARED
	//HANDLE textureHandle;
	//winrt::check_hresult(m_texture.as<IDXGIResource>()->GetSharedHandle(&textureHandle));
	//
	//
	//
	//winrt::com_ptr<ID3D11Texture2D> mediaTexture;
	//if (SUCCEEDED(m_device->OpenSharedResource(textureHandle, __uuidof(ID3D11Texture2D), mediaTexture.put_void())))
	//{
	//	
	//		//copy the render target texture to the readable texture.
	//		m_context->CopySubresourceRegion1(Show.get(), 0, 0, 0, 0, mediaTexture.get(), 0, NULL, 0);
	//		m_context->Flush();
	//
	//
	//		winrt::check_hresult(m_context->Map(Show.get(), 0, D3D11_MAP_READ, 0, &mappedResource));
	//		m_context->Unmap(Show.get(), 0);
	//	}
	//}
}
void MediaEnginePlayer::OnMediaEngineEvent(uint32_t meEvent)
{
	switch (meEvent)
	{
	case MF_MEDIA_ENGINE_EVENT_LOADEDMETADATA:
		m_isInfoReady = true;
		break;

	case MF_MEDIA_ENGINE_EVENT_CANPLAY:
		break;

	case MF_MEDIA_ENGINE_EVENT_PLAY:
		break;

	case MF_MEDIA_ENGINE_EVENT_PAUSE:
		m_isPlaying = false;
		break;

	case MF_MEDIA_ENGINE_EVENT_ENDED:
		m_isFinished = true;
		break;

	case MF_MEDIA_ENGINE_EVENT_TIMEUPDATE:
		break;

	case MF_MEDIA_ENGINE_EVENT_ERROR:
#ifdef _DEBUG
		if (m_mediaEngine)
		{
			winrt::com_ptr<IMFMediaError> error;
			if (SUCCEEDED(m_mediaEngine->GetError(error.put())))
			{
				USHORT errorCode = error->GetErrorCode();
				HRESULT hr = error->GetExtendedErrorCode();
				char buff[128] = {};
				sprintf_s(buff, "ERROR: Media Foundation Event Error %u (%08X)\n", errorCode, static_cast<unsigned int>(hr));
				OutputDebugStringA(buff);
			}
			else
			{
				OutputDebugStringA("ERROR: Media Foundation Event Error *FAILED GetError*\n");
			}
		}
#endif
		break;
	}
}

void MediaEnginePlayer::FrameFinished()
{

}

void MediaEnginePlayer::GetNativeVideoSize(uint32_t& cx, uint32_t& cy)
{
	cx = cy = 0;
	if (m_mediaEngine && m_isInfoReady)
	{
		DWORD x, y;
		winrt::check_hresult(m_mediaEngine->GetNativeVideoSize(&x, &y));

		cx = x;
		cy = y;
	}
}
