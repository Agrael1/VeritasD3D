#pragma once
#include <DirectXMath.h>

namespace ver
{
	class CursorTransform
	{
	public:
		CursorTransform() = default;
	public:
		void SetTransform(DirectX::XMMATRIX displacement)noexcept
		{
			transform = displacement;
		}
		auto GetTransform()const noexcept
		{
			return transform;
		}
	private:
		static inline DirectX::XMMATRIX transform;
	};
}