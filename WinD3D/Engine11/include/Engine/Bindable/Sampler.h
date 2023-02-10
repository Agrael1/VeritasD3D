#pragma once
#include <Engine/Bindable/Bindable.h>

struct ID3D11SamplerState;

namespace ver
{
	class Sampler : public Bindable
	{
	public:
		enum class Type
		{
			Anisotropic,
			Bilinear,
			Point,
		};
	public:
		Sampler(Graphics& gfx, Type type = Type::Anisotropic, bool reflect = false, uint32_t slot = 0u);
	public:
		void Bind(Graphics& gfx) noxnd override;
		void Bind(ID3D11DeviceContext& context) noxnd;
		static std::shared_ptr<Sampler> Resolve(Graphics& gfx, Type type = Type::Anisotropic, bool reflect = false, uint32_t slot = 0u);
		static std::string GenerateUID(Type type = Type::Anisotropic, bool reflect = false, uint32_t slot = 0u);
		std::string GetUID() const noexcept override;
	protected:
		winrt::com_ptr<ID3D11SamplerState> pSampler;
		uint32_t slot;
		Type type;
		bool reflect;
	};

	class VertexSampler : public Sampler
	{
	public:
		void Bind(Graphics& gfx) noxnd override;
		void Bind(ID3D11DeviceContext& context) noxnd;
	};
	class HullSampler : public Sampler
	{
	public:
		void Bind(Graphics& gfx) noxnd override;
		void Bind(ID3D11DeviceContext& context) noxnd;
	};
	class DomainSampler : public Sampler
	{
	public:
		void Bind(Graphics& gfx) noxnd override;
		void Bind(ID3D11DeviceContext& context) noxnd;
	};
}
