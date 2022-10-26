#include "MediaEngine.h"
#include <wil/com.h>

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

void MediaEnginePlayer::Initialize(ID3D11Device* device, DXGI_FORMAT format)
{
    // Initialize Media Foundation (see Main.cpp for code to handle Windows N Editions)
    winrt::throw_hresult(MFStartup(MF_VERSION));

    // Create our own device to avoid threading issues
    winrt::com_ptr<IDXGIDevice> dxgiDevice;
    winrt::throw_hresult(device->QueryInterface(dxgiDevice.put()));

    winrt::com_ptr<IDXGIAdapter> adapter;
    winrt::throw_hresult(dxgiDevice->GetAdapter(adapter.put()));

    D3D_FEATURE_LEVEL fLevel = device->GetFeatureLevel();

    winrt::throw_hresult(
        D3D11CreateDevice(
            adapter.get(),
            D3D_DRIVER_TYPE_UNKNOWN,
            nullptr,
            D3D11_CREATE_DEVICE_VIDEO_SUPPORT | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            &fLevel,
            1,
            D3D11_SDK_VERSION,
            m_device.put(),
            nullptr,
            nullptr
        ));

    winrt::com_ptr<ID3D11Multithread> multithreaded = m_device.as<ID3D11Multithread>();
    multithreaded->SetMultithreadProtected(TRUE);

    // Setup Media Engine
    winrt::com_ptr<IMFDXGIDeviceManager> dxgiManager;
    UINT resetToken;
    winrt::throw_hresult(MFCreateDXGIDeviceManager(&resetToken, dxgiManager.put()));
    winrt::throw_hresult(dxgiManager->ResetDevice(m_device.get(), resetToken));

    // Create our event callback object.
    auto spNotify = winrt::make_self<MediaEngineNotify>();

    spNotify->SetCallback(this);

    // Set configuration attribiutes.
    winrt::com_ptr<IMFAttributes> attributes;
    winrt::throw_hresult(MFCreateAttributes(attributes.put(), 1));
    winrt::throw_hresult(attributes->SetUnknown(MF_MEDIA_ENGINE_DXGI_MANAGER, reinterpret_cast<IUnknown*>(dxgiManager.get())));
    winrt::throw_hresult(attributes->SetUnknown(MF_MEDIA_ENGINE_CALLBACK, reinterpret_cast<IUnknown*>(spNotify.get())));
    winrt::throw_hresult(attributes->SetUINT32(MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT, format));

    // Create MediaEngine.
    winrt::com_ptr<IMFMediaEngineClassFactory> mfFactory;
    winrt::throw_hresult(
        CoCreateInstance(CLSID_MFMediaEngineClassFactory,
            nullptr,
            CLSCTX_ALL, __uuidof(IMFMediaEngineClassFactory),
            mfFactory.put_void()));

    winrt::throw_hresult(
        mfFactory->CreateInstance(0,
            attributes.get(),
            m_mediaEngine.put()));
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
        winrt::throw_hresult(m_mediaEngine->Play());
        m_isPlaying = true;
        m_isFinished = false;
    }
}

void MediaEnginePlayer::SetMuted(bool muted)
{
    if (m_mediaEngine)
        winrt::throw_hresult(m_mediaEngine->SetMuted(muted));
}

void MediaEnginePlayer::SetSource(std::wstring_view sourceUri)
{
    wil::unique_bstr bstrURL = wil::make_bstr_failfast(sourceUri.data());

    m_isPlaying = false;
    m_isInfoReady = false;
    m_isFinished = false;

    if (m_mediaEngine)
        winrt::throw_hresult(m_mediaEngine->SetSource(bstrURL.get()));
}

bool MediaEnginePlayer::TransferFrame(const winrt::com_ptr<ID3D11Texture2D>& texture, MFVideoNormalizedRect rect, RECT rcTarget)
{
    if (m_mediaEngine && m_isPlaying)
    {
        LONGLONG pts;
        if (m_mediaEngine->OnVideoStreamTick(&pts) == S_OK)
        {
            // The texture must have been created with D3D11_RESOURCE_MISC_SHARED
            HANDLE textureHandle;
            winrt::throw_hresult(texture.as<IDXGIResource>()->GetSharedHandle(&textureHandle));

            winrt::com_ptr<ID3D11Texture2D> mediaTexture;
            if (SUCCEEDED(m_device->OpenSharedResource(textureHandle, __uuidof(ID3D11Texture2D), mediaTexture.put_void())))
            {
                if (m_mediaEngine->TransferVideoFrame(mediaTexture.get(), &rect, &rcTarget, &m_bkgColor) == S_OK)
                    return true;
            }
        }
    }
    return false;
}

void MediaEnginePlayer::OnMediaEngineEvent(uint32_t meEvent)
{
    switch (meEvent)
    {
    case MF_MEDIA_ENGINE_EVENT_LOADEDMETADATA:
        m_isInfoReady = true;
        break;

    case MF_MEDIA_ENGINE_EVENT_CANPLAY:

        // Here we auto-play when ready...
        Play();

        break;

    case MF_MEDIA_ENGINE_EVENT_PLAY:
        break;

    case MF_MEDIA_ENGINE_EVENT_PAUSE:
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

void MediaEnginePlayer::GetNativeVideoSize(uint32_t& cx, uint32_t& cy)
{
    cx = cy = 0;
    if (m_mediaEngine && m_isInfoReady)
    {
        DWORD x, y;
        winrt::throw_hresult(m_mediaEngine->GetNativeVideoSize(&x, &y));

        cx = x;
        cy = y;
    }
}
