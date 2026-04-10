#pragma once
#include <Engine/Bindable/Bindable.h>

namespace ver
{
	class ShadowSampler : public Bindable
	{
	public:
		ShadowSampler(Graphics& gfx);
	public:
		void Bind(Graphics& gfx) noxnd override;
		void SetBilinear(bool bilin);
		void SetHwPcf(bool hwPcf);
		bool GetBilinear() const;
		bool GetHwPcf() const;
	private:
		uint32_t GetCurrentSlot() const;
		static size_t ShadowSamplerIndex(bool bilin, bool hwPcf);
		static winrt::com_ptr<ID3D11SamplerState> MakeSampler(Graphics& gfx, bool bilin, bool hwPcf);
	protected:
		size_t curSampler;
		winrt::com_ptr<ID3D11SamplerState> samplers[4];
	};
}