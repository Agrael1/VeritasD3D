#include "BindableCommons.h"
#include "Drawable.h"
#include "GraphicsThrows.m"
#include "Material.h"

Drawable::Drawable(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale) noexcept
{
	pVertices = mat.MakeVertexBindable(gfx, mesh, scale);
	pIndices = mat.MakeIndexBindable(gfx, mesh);
	pTopology = Topology::Resolve(gfx);

	for (auto& t : mat.GetTechniques())
	{
		AddTechnique(std::move(t));
	}
}

void Drawable::AddTechnique(Technique tech_in) noexcept
{
	tech_in.InitializeParentReferences(*this);
	techniques.push_back(std::move(tech_in));
}
void Drawable::Submit(FrameCommander& frame) const noexcept
{
	for (const auto& tech : techniques)
	{
		tech.Submit(frame, *this);
	}
}
void Drawable::Bind(Graphics& gfx) const noexcept
{
	pTopology->Bind(gfx);
	pIndices->Bind(gfx);
	pVertices->Bind(gfx);
}
void Drawable::Accept(TechniqueProbe& probe)
{
	for (auto& t : techniques)
	{
		t.Accept(probe);
	}
}
UINT Drawable::GetIndexCount() const noexcept(!IS_DEBUG)
{
	return pIndices->GetCount();
}