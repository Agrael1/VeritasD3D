#pragma once
#include <DirectXMath.h>

namespace ver
{
	class CursorTransform
	{
	public:
		CursorTransform() = default;
	public:
		void SetTransform(DirectX::XMVECTOR displacement)noexcept
		{
			transform = DirectX::XMMatrixTranslationFromVector(displacement);
		}
		auto GetTransform()const noexcept
		{
			return transform;
		}
	private:
		static inline DirectX::XMMATRIX transform;
	};
}