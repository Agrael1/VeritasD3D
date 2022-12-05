#include <Audio/buffer_stream.h>
#include <Audio/Audio.h>

ver::audio::ogg_stream::ogg_stream(Audio& au, std::filesystem::path path)
{
	file.load_sound(std::move(path));
	init_voice(au);
	buffer.reserve(file.size());
	state = state::initialized;
}

winrt::IAsyncAction ver::audio::ogg_stream::InitializeAsync(Audio& au, std::filesystem::path path)
{
	co_await winrt::resume_background();
	file.load_sound(std::move(path));
	init_voice(au);
	buffer.reserve(file.size());
	state = state::initialized;
}

winrt::IAsyncAction ver::audio::ogg_stream::play(bool looped)
{
	co_await winrt::resume_background();
	this->looped = looped;
	switch (state)
	{
	case ver::audio::ogg_stream::state::initialized:
		if (loaded)
			co_return send_whole();
		read_chunk();
		[[fallthrough]];
	case ver::audio::ogg_stream::state::paused:
		pSourceVoice->Start();
		state = state::played;
		break;
	default:
		break;
	}
}
void ver::audio::ogg_stream::stop()
{
	switch (state)
	{
	case ver::audio::ogg_stream::state::played:
		pSourceVoice->Stop();
		pSourceVoice->FlushSourceBuffers();
		state = state::created;
		break;
	default:
		break;
	}
}
void ver::audio::ogg_stream::pause()
{
	if (state == ver::audio::ogg_stream::state::played)
	{
		pSourceVoice->Stop();
		state = state::paused;
	}
}

void __stdcall ver::audio::ogg_stream::OnBufferEnd(void*) noexcept
{
	if (!loaded && state == state::played)
	{
		read_chunk();
	}
}
void __stdcall ver::audio::ogg_stream::OnStreamEnd() noexcept
{
	if(looped)send_whole();
}

void ver::audio::ogg_stream::read_chunk()
{
	buffer.resize(buffer.size() + page_size);
	auto sp = std::span{ buffer.begin() + offset, buffer.end() };
	size_t l = file.read(sp);
	loaded = l != page_size;
	send_chunk(sp);
	offset += l;
}

void ver::audio::ogg_stream::send_chunk(std::span<std::byte> chunk, bool loop)
{
	XAUDIO2_BUFFER xbuffer{};
	xbuffer.pAudioData = (BYTE*)chunk.data();
	xbuffer.AudioBytes = (uint32_t)chunk.size();
	xbuffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
	xbuffer.Flags = loaded ? XAUDIO2_END_OF_STREAM : 0;

	winrt::check_hresult(pSourceVoice->SubmitSourceBuffer(&xbuffer));
}
void ver::audio::ogg_stream::init_voice(Audio& au)
{
	//The vorbis_info struct keeps the most of the interesting format info
	const vorbis_info& vi = file.info();

	//Set the wave format
	WAVEFORMATEX wfm{};
	wfm.cbSize = sizeof(wfm);
	wfm.nChannels = vi.channels;
	wfm.wBitsPerSample = 16;                    //Ogg vorbis is always 16 bit
	wfm.nSamplesPerSec = vi.rate;
	wfm.nAvgBytesPerSec = wfm.nSamplesPerSec * wfm.nChannels * 2;
	wfm.nBlockAlign = 2 * wfm.nChannels;
	wfm.wFormatTag = 1;

	//Create the source voice
	winrt::check_hresult(au.GetAudio().CreateSourceVoice(pSourceVoice.put(), &wfm, 0, XAUDIO2_DEFAULT_FREQ_RATIO, this));
}
void ver::audio::ogg_stream::send_whole()
{
	send_chunk(buffer, looped);
}