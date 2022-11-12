#pragma once
#include <cstdint>
#include <string_view>

#include <Windows.h>
#include <d3d11_4.h>
#include <mfapi.h>
#include <mfmediaengine.h>
#include <winrt/base.h>

class IMFNotify
{
public:
    virtual ~IMFNotify() = default;

    IMFNotify(const IMFNotify&) = delete;
    IMFNotify& operator=(const IMFNotify&) = delete;

    IMFNotify(IMFNotify&&) = delete;
    IMFNotify& operator=(IMFNotify&&) = delete;
public:
    virtual void OnMediaEngineEvent(uint32_t meEvent) = 0;
protected:
    IMFNotify() = default;
};

class MediaEnginePlayer : public IMFNotify
{
public:
    MediaEnginePlayer() = default;
    ~MediaEnginePlayer()
    {
        Shutdown();
        MFShutdown();
    }

    MediaEnginePlayer(const MediaEnginePlayer&) = delete;
    MediaEnginePlayer& operator=(const MediaEnginePlayer&) = delete;

    MediaEnginePlayer(MediaEnginePlayer&&) = default;
    MediaEnginePlayer& operator=(MediaEnginePlayer&&) = default;
public:
    void Initialize(ID3D11Device* device, DXGI_FORMAT format);
    void Shutdown();

    void Play();
    void SetMuted(bool muted);

    void SetSource(std::wstring_view sourceUri);

    bool TransferFrame(const winrt::com_ptr<ID3D11Texture2D>& texture, MFVideoNormalizedRect rect, RECT rcTarget);

    // Callbacks
    void OnMediaEngineEvent(uint32_t meEvent) override;

    // Properties
    void GetNativeVideoSize(uint32_t& cx, uint32_t& cy);
    bool IsPlaying() const { return m_isPlaying; }
    bool IsInfoReady() const { return m_isInfoReady; }
    bool IsFinished() const { return m_isFinished; }
private:
    winrt::com_ptr<ID3D11Device>        m_device;
    winrt::com_ptr<IMFMediaEngine>      m_mediaEngine;

    MFARGB  m_bkgColor{};

    bool m_isPlaying = false;
    bool m_isInfoReady = false;
    bool m_isFinished = false;
};