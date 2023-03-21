#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Bindable/IndexBuffer.h>
#include <Engine/Rendering/Drawable.h>
#include <Engine/Loading/Material.h>

void Drawable::Submit() const noexcept
{
	for (const auto& tech : techniques)
	{
		tech.Submit(*this);
	}
}

Drawable::Drawable(Graphics& gfx, const Material& mat, const aiMesh& mesh) noexcept
{
	pVertices = mat.MakeVertexBindable(gfx, mesh);
	pIndices = mat.MakeIndexBindable(gfx, mesh);

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
	ver::Topology::Bind(gfx, topology);
	pIndices->Bind(gfx);
	if(pVertices)pVertices->Bind(gfx);
}

void Drawable::Draw(Graphics& gfx) const noxnd
{
	gfx.DrawIndexed(pIndices->GetCount());
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