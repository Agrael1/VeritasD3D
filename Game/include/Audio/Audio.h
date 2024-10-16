#pragma once
#include <xaudio2.h>
#include <filesystem>
#include <Audio/audio_ptr.h>
#include <Audio/vorbis_file.h>

namespace ver::audio
{
	class Audio
	{
		
	public:
		Audio() = default;
	public:
		void Initialize();
		ver::IAsyncAction InitializeAsync();
		template<class Self>
		auto&& GetAudio(this Self&& s)
		{
			return *s.pXAudio2;
		}
	private:
		winrt::com_ptr<IXAudio2> pXAudio2;
		voice_ptr<IXAudio2MasteringVoice> pMasteringVoice;
	};
}
