#pragma once
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <filesystem>
#include <span>



namespace ver::audio
{


	class vorbis_file
	{
	public:
		vorbis_file() = default;
		vorbis_file(std::filesystem::path path)
		{
			load_sound(std::move(path));
		}
		vorbis_file(const vorbis_file&) = delete;
		vorbis_file(vorbis_file&&) = delete;
		~vorbis_file()
		{
			clear();
		}
	public:
		void clear()
		{
			if (loaded)ov_clear(&file);
		}
		void load_sound(std::filesystem::path path)
		{
			if (!std::filesystem::exists(path))
				return;

			FILE* f;
			errno_t err;

			if ((err = _wfopen_s(&f, path.c_str(), L"rb")) != 0)
			{
				char szBuffer[MAX_PATH];
				_strerror_s(szBuffer, MAX_PATH, nullptr);
				throw std::runtime_error(szBuffer);
			}
			if (!open(f))fclose(f);
		}

		const auto& info()const
		{
			return *ov_info(&file, -1);
		}
		long read(std::span<std::byte> buffer)const
		{
			if (!loaded)return 0;
			int current_section = 0;

			long ret = 1;
			long offset = 0;
			while (ret && offset != buffer.size())
			{
				ret = ov_read(&file, (char*)buffer.data() + offset, (int)buffer.size() - offset, 0, 2, 1, &current_section);
				offset += ret;
			}
			return offset;
		}
		long size()const
		{
			return long(file.end - file.offset);
		}
	private:
		bool open(void* datasource, const char* initial = nullptr, long ibytes = 0)
		{
			clear();
			loaded = ov_open_callbacks(datasource, &file, initial, ibytes, OV_CALLBACKS_DEFAULT) == 0;
			return loaded;
		}
	private:
		mutable OggVorbis_File file{};
		bool loaded = false;
	};
}