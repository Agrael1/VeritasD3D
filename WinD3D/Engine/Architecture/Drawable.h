#pragma once
#include <Framework/noexcept_if.h>
#include <Engine/Graphics.h>
#include <DirectXMath.h>
#include <memory>
#include "Technique.h"

class Drawable
{
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
public:
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	virtual ~Drawable() = default;
public:
	void AddTechnique(Technique tech_in) noexcept;
	void Submit(class FrameCommander& frame) const noexcept;
	void Bind(Graphics& gfx)const noexcept;
	void Accept(TechniqueProbe& probe);
	UINT GetIndexCount()const noxnd;
protected:
	std::shared_ptr<class IndexBuffer> pIndices;
	std::shared_ptr<class VertexBuffer> pVertices;
	std::shared_ptr<class Topology> pTopology;
	std::vector<Technique> techniques;
};