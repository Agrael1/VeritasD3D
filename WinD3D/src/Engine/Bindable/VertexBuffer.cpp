#include <Engine/Bindable/VertexBuffer.h>
#include <Engine/Bindable/Codex.h>
#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/GraphicsExceptions.h>
#include <Engine/Deprecated/GraphicsThrows.h>

VertexBuffer::VertexBuffer(Graphics& gfx, const DV::VertexBuffer& vbuf)
	:
	VertexBuffer(gfx, "?", vbuf)
{}
VertexBuffer::VertexBuffer(Graphics& gfx, const std::string& tag, const DV::VertexBuffer& vbuf)
	:
	stride((UINT)vbuf.GetLayout().Size()),
	tag(tag),
	layout(vbuf.GetLayout())
{
	INFOMAN(gfx);

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = UINT(vbuf.Size());
	bd.StructureByteStride = stride;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vbuf.data();
	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &pVertexBuffer));
}


const DV::VertexLayout& VertexBuffer::GetLayout() const noexcept
{
	return layout;
}
void VertexBuffer::Bind(Graphics& gfx) noxnd
{
	const UINT offset = 0u;
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset));
}
std::shared_ptr<VertexBuffer> VertexBuffer::Resolve(Graphics& gfx, const std::string& tag,
	const DV::VertexBuffer& vbuf)
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

void NullVertexBuffer::Bind(Graphics& gfx) noxnd
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->IASetVertexBuffers(0u, 0u, nullptr, nullptr, nullptr));
}
