#pragma once
#include <Engine/Bindable/Bindable.h>
#include <Engine/Dynamic/VertexBuffer.h>
#include <memory>

class VertexBuffer : public Bindable
{
public:
	VertexBuffer(Graphics& gfx, const std::string& tag, ver::dv::VertexSpan vbuf);
	VertexBuffer(Graphics& gfx, ver::dv::VertexSpan vbuf);
public:
	void Bind(Graphics& gfx) noxnd override;
	static std::shared_ptr<VertexBuffer> Resolve(Graphics& gfx, const std::string& tag,
		ver::dv::VertexSpan vbuf);
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
	uint32_t stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};


class VertexMultibuffer : public Bindable
{
public:
	VertexMultibuffer(Graphics& gfx, ver::dv::LayoutSpan vbuf, std::span<void*> data, size_t num_verts);
public:
	void Bind(Graphics& gfx) noxnd override;
protected:
	std::vector<uint32_t> strides;
	std::vector< winrt::com_ptr<ID3D11Buffer>> buffers;
};