#include <Engine/Bindable/VertexBuffer.h>
#include <Engine/Bindable/Codex.h>
#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/GraphicsExceptions.h>
#include <Engine/Deprecated/GraphicsThrows.h>

VertexBuffer::VertexBuffer(Graphics& gfx, ver::dv::VertexSpan vbuf)
	:VertexBuffer(gfx, "?", vbuf)
{}
VertexBuffer::VertexBuffer(Graphics& gfx, const std::string& tag, ver::dv::VertexSpan vbuf)
	:stride((UINT)vbuf.layout().layout_size()),
	tag(tag)
{
	INFOMAN(gfx);

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = UINT(vbuf.size());
	bd.StructureByteStride = stride;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vbuf.data().data();
	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &pVertexBuffer));
}


void VertexBuffer::Bind(Graphics& gfx) noxnd
{
	const UINT offset = 0u;
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset));
}
std::shared_ptr<VertexBuffer> VertexBuffer::Resolve(Graphics& gfx, const std::string& tag, ver::dv::VertexSpan vbuf)
{
	assert(tag != "?");
	return ver::Codex::Resolve<VertexBuffer>(gfx, tag, vbuf);
}
std::string VertexBuffer::GenerateUID_(const std::string& tag)
{
	using namespace std::string_literals;
	return typeid(VertexBuffer).name() + "#"s + tag;
}
std::string VertexBuffer::GetUID() const noexcept
{
	return GenerateUID(tag);
}

VertexMultibuffer::VertexMultibuffer(Graphics& gfx, ver::dv::LayoutSpan lay, std::span<void*> data, size_t num_verts)
{
	size_t c = lay.count();
	buffers.resize(c);
	strides.resize(c);

	INFOMAN(gfx);

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;

	D3D11_SUBRESOURCE_DATA sd = {};

	auto s = lay.span();
	for (size_t i = 0; i < c; i++)
	{
		bd.ByteWidth = uint32_t(s[i].size() * num_verts);
		bd.StructureByteStride = strides[i] = uint32_t(s[i].size());
		sd.pSysMem = data[i];
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, buffers[i].put()));
	}
}
void VertexMultibuffer::Bind(Graphics& gfx) noxnd
{
	const uint32_t offsets[size_t(ver::dv::ElementType::Count)]{};
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->IASetVertexBuffers(0u, uint32_t(buffers.size()), reinterpret_cast<ID3D11Buffer* const*>(buffers.data()), strides.data(), offsets));
}
