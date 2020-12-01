#include "BindableCommons.h"
#include "Drawable.h"
#include "GraphicsThrows.m"
#include "Material.h"

void Drawable::Submit() const noexcept
{
	for (const auto& tech : techniques)
	{
		tech.Submit(*this);
	}
}

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

void Drawable::Bind(Graphics& gfx) const noxnd
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

UINT Drawable::GetIndexCount() const noxnd
{
	return pIndices->GetCount();
}

void Drawable::LinkTechniques(RG::RenderGraph& rg)
{
	for (auto& tech : techniques)
	{
		tech.Link(rg);
	}
}
void Drawable::UnlinkTechniques()
{
	for (auto& tech : techniques)
	{
		tech.Unlink();
	}
}

Drawable::~Drawable()
{}