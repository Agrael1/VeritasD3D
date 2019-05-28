#include "Drawable.h"
#include "IndexBuffer.h"
#include "GraphicsThrows.m"
#include <cassert>

void Drawable::Draw(Graphics & gfx) const noexcept(!IS_DEBUG)
{
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}
	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{
	assert("*Must* use AddIndexBuffer to bind index buffer"&&typeid(*bind) != typeid(IndexBuffer));
	binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<class IndexBuffer> iBuf) noexcept
{
	assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
	pIndexBuffer = iBuf.get();
	binds.push_back(std::move(iBuf));
}
