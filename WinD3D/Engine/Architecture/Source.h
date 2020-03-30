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
		const std::string& GetName()const noexcept;
		virtual void PostLinkValidate()const = 0;
		virtual std::shared_ptr<Bindable> YieldBindable();
		virtual std::shared_ptr<BufferResource> YieldBuffer();
		~Source() = default;
	protected:
		Source(std::string name);
	private:
		std::string name;
	};
}
