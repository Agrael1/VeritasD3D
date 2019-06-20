#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include <memory>

class Bindable;

class Drawable
{
	template<class T>
	friend class DrawableBase;
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
public:
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	virtual void Update(float dt)noexcept = 0;
	virtual ~Drawable() = default;
public:
	void Draw(Graphics& gfx)const noexcept(!IS_DEBUG);
protected:
	void AddBind(std::unique_ptr<Bindable> bind)noexcept(!IS_DEBUG);
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer>iBuf)noexcept(!IS_DEBUG);
private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds()const noexcept = 0;
private:
	const class IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;
};