#include <Audio/Audio.h>
#include <fstream>

constexpr const auto flags = DEBUG_MODE ? XAUDIO2_DEBUG_ENGINE : 0;

void ver::audio::Audio::Initialize()
{
	winrt::check_hresult(XAudio2Create(pXAudio2.put(), flags));
	winrt::check_hresult(pXAudio2->CreateMasteringVoice(pMasteringVoice.put()));
}
ver::IAsyncAction ver::audio::Audio::InitializeAsync()
{
	co_await winrt::resume_background();
	Initialize();
}
