#pragma once
#include <Engine/Rendering/Technique.h>
#include <DirectXMath.h>
#include <memory>

class TechniqueProbe;
class Material;
struct aiMesh;

namespace RG
{
	class RenderGraph;
}
class VertexBuffer;
class Topology;
class InputLayout;

namespace ver
{
	class IndexBuffer;
}

class Drawable
{
public:
	Drawable() = default;
	Drawable(Graphics& gfx, const Material& mat, const aiMesh& mesh) noexcept;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable();
public:
	void AddTechnique(Technique tech_in) noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Submit() const noexcept;
	void Bind(Graphics& gfx) const noxnd;
	virtual void Draw(Graphics& gfx) const noxnd;
	void Accept(TechniqueProbe& probe);
	UINT GetIndexCount() const noxnd;
	void LinkTechniques(RG::RenderGraph&);
	void UnlinkTechniques();
protected:
	std::shared_ptr<ver::IndexBuffer> pIndices;
	std::shared_ptr<Bindable> pVertices;
	D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	std::vector<Technique> techniques;
};