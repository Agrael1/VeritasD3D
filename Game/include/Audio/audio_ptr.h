#pragma once

namespace ver::audio
{
	template<typename TOwner, void(TOwner::* func)()>
	class audio_ptr
	{
	public:
		audio_ptr() = default;
		~audio_ptr()
		{
			reset();
		}
		audio_ptr(const audio_ptr&) = delete;
		audio_ptr(audio_ptr&& other)noexcept
			:ptr(other.ptr) {
			other.ptr = nullptr;
		}
	public:
		bool operator ==(void* other)const noexcept
		{
			return ptr == other;
		}
		bool operator !=(void* other)const noexcept
		{
			return ptr != other;
		}

		template<class Self>
		auto* operator->(this Self&& s)
		{
			return s.get();
		}

		void reset()
		{
			if (!ptr)return;
			(ptr->*func)();
			ptr = nullptr;
		}
		auto** put()
		{
			reset();
			return &ptr;
		}
		template<class Self>
		auto* get(this Self&& s)
		{
			return s.ptr;
		}
		operator bool()const noexcept
		{
			return ptr != nullptr;
		}
	private:
		TOwner* ptr = nullptr;
	};

	template<class T>
	using voice_ptr = ver::audio::audio_ptr<T, &T::DestroyVoice>;
}