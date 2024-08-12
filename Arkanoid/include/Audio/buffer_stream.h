#pragma once
#include <xaudio2.h>
#include <Audio/vorbis_file.h>
#include <Audio/audio_ptr.h>
#include <vector>
#include <span>

namespace ver::audio
{
	class Audio;

	class ogg_stream : public IXAudio2VoiceCallback
	{
		enum class state : uint8_t
		{
			created,
			initialized,
			played,
			paused
		};
		static inline constexpr const size_t page_size = 4096u;
	public:
		ogg_stream() = default;
		ogg_stream(Audio& au, std::filesystem::path path);
		~ogg_stream() { stop(); }
	public:
		ver::IAsyncAction InitializeAsync(Audio& au, std::filesystem::path path);
		ver::IAsyncAction play(bool looped = true);
		void stop();
		void pause();
	private:
		void __stdcall OnVoiceProcessingPassStart(UINT32)noexcept override {}
		void __stdcall OnVoiceProcessingPassEnd()noexcept override {}
		void __stdcall OnStreamEnd()noexcept override;
		void __stdcall OnBufferStart(void*)noexcept override {}
		void __stdcall OnBufferEnd(void*) noexcept override;
		void __stdcall OnLoopEnd(void*)noexcept override{}
		void __stdcall OnVoiceError(void*, HRESULT)noexcept override{}
		void send_chunk(std::span<std::byte> chunk, bool loop = false);
		void init_voice(Audio& au);
		void send_whole();
		void read_chunk();
		void swap();
	private:
		voice_ptr<IXAudio2SourceVoice> pSourceVoice;
		std::vector<std::byte> buffer;
		ver::audio::vorbis_file file{};
		size_t offset = 0;
		state state = state::created;
		bool looped = false;
		bool loaded = false;
	};
}