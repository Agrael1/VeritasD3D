#pragma once
#include <DirectXMath.h>
#include <array>
#include "ConstantBuffer2.h"

class Graphics;

namespace ver
{
	struct PointLightConsts
	{
		DirectX::XMFLOAT4A pos;

		DirectX::XMFLOAT3 diffuse;
		float diffuseIntensity;

		float attConst;
		float attLin;
		float attQuad;
		uint32_t active;
	};

	class LightBuffer
	{
		static constexpr uint32_t max_lights = 32u;
		static constexpr uint32_t binding_slot = 7u;
	public:
		LightBuffer() = default;
		LightBuffer(Graphics& gfx);
		winrt::IAsyncAction InitializeAsync(Graphics& gfx);
	public:
		void Bind(Graphics& gfx) noxnd;
		PointLightConsts& Allocate();
		const PointLightConsts& at(size_t n)const
		{
			return data.lights.at(n);
		}
	private:
		mutable struct alignas(alignof(DirectX::XMFLOAT4X4A)) cbuffer
		{
			DirectX::XMFLOAT3 ambient{ 0.1f,0.1f,0.1f };
			uint32_t count = 0u;
			std::array<PointLightConsts, max_lights> lights{};
		}data;
		std::array<PointLightConsts, max_lights> light_buffer{};
		PixelConstantBuffer<cbuffer> cbuf;
	};
}