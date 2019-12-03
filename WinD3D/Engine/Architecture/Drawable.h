#pragma once
#include <Framework/noexcept_if.h>
#include <Engine/Graphics.h>
#include <DirectXMath.h>
#include <memory>

class Bindable;
class IndexBuffer;

class Drawable
{
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
public:
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	virtual void Update(float dt)noexcept {};
	virtual ~Drawable() = default;
public:
	void Draw(Graphics& gfx)const noxnd;
protected:
	void AddBind(std::shared_ptr<IndexBuffer> index)noxnd;
	void AddBind(std::shared_ptr<Bindable> bind)noxnd;
private:
	const IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::shared_ptr<Bindable>> binds;
};