#pragma once
#include <Engine/Bindable/Bindable.h>
#include <optional>

struct ID3D11BlendState;

namespace ver
{
	class BlendState : public Bindable
	{
	public:
		BlendState() = default;
		BlendState(Graphics& gfx, bool blending, std::optional<float> factor = {});
	public:
		void Bind(Graphics& gfx) noxnd override;
		void Bind(ID3D11DeviceContext& context) noxnd;

		void SetFactor(float factor) noxnd;
		float GetFactor() const noxnd;

		static std::shared_ptr<BlendState> Resolve(Graphics& gfx, bool blending, std::optional<float> factor = {});
		static std::string GenerateUID(bool blending, std::optional<float> factor);
		std::string GetUID() const noexcept override;
	protected:
		winrt::com_ptr<ID3D11BlendState> pBlendState;
		std::optional<std::array<float, 4>> factors{};
		bool blending = false;
	};
}
