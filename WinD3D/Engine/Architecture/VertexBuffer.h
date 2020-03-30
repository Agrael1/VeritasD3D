#pragma once
#include <Engine/Architecture/Bindable.h>
#include <Engine/Architecture/VertexLayout.h>
#include "GraphicsThrows.m"
#include <memory>

class VertexBuffer : public Bindable
{
public:
	VertexBuffer(Graphics& gfx, const std::string& tag, const DV::VertexBuffer& vbuf);
	VertexBuffer(Graphics& gfx, const DV::VertexBuffer& vbuf);
public:
	const DV::VertexLayout& GetLayout() const noexcept;
	void Bind(Graphics& gfx) noxnd override;
	static std::shared_ptr<VertexBuffer> Resolve(Graphics& gfx, const std::string& tag,
		const DV::VertexBuffer& vbuf);
	template<typename...Ignore>
	static std::string GenerateUID(const std::string& tag, Ignore&&...ignore)
	{
		return GenerateUID_(tag);
	}
	std::string GetUID() const noexcept override;
private:
	static std::string GenerateUID_(const std::string& tag);
protected:
	std::string tag;
	UINT stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	DV::VertexLayout layout;
};