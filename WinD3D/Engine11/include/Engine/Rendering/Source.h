#pragma once
#include <memory>
#include <string>

class Bindable;
class BufferResource;

namespace RG
{
	class Source 
	{
	public:
		std::string_view GetName()const noexcept;
		virtual void PostLinkValidate()const = 0;
		virtual std::shared_ptr<Bindable> YieldBindable();
		virtual std::shared_ptr<BufferResource> YieldBuffer();
		virtual ~Source() = default;
	protected:
		explicit Source(std::string name);
		Source(std::string_view name);
	private:
		std::string name;
	};
}
