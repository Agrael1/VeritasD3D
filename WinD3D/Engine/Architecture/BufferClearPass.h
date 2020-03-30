#pragma once
#include "Pass.h"

class BufferResource;


namespace RG
{
	class BufferClearPass : public Pass
	{
	public:
		BufferClearPass(std::string name);
		void Execute(Graphics& gfx) const noxnd override;
	private:
		std::shared_ptr<BufferResource> buffer;
	};
}