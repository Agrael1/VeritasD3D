#include "IndexBuffer.h"
#include "Drawable.h"
#include "GraphicsThrows.m"
#include <cassert>

void Drawable::Draw(Graphics & gfx) const noxnd
{
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}
	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::shared_ptr<IndexBuffer> index)noxnd
{
	assert("Multiple Index buffers attached!" && pIndexBuffer == nullptr);
	pIndexBuffer = &(*index);		
	binds.push_back(std::move(index));
}
void Drawable::AddBind(std::shared_ptr<Bindable> bind) noxnd
{
	// special case for index buffer
	if (typeid(*bind) == typeid(IndexBuffer))
	{
		assert("Binding multiple index buffers not allowed" && pIndexBuffer == nullptr);
		pIndexBuffer = &static_cast<IndexBuffer&>(*bind);
	}
	binds.push_back(std::move(bind));
}
