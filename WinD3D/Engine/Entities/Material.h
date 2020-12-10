#pragma once
#include <Engine/Graphics.h>
#include <vector>
#include <filesystem>
#include <Architecture/VertexLayout.h>
#include <Architecture/Technique.h>
#include <pplawait.h>

struct aiMaterial;
struct aiMesh;
class VertexBuffer;
class IndexBuffer;

class Material
{
public:
	Material() = default;
	Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noxnd;
	concurrency::task<void> MakeMaterialAsync(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path)noxnd;
public:
	DV::VertexBuffer ExtractVertices(const aiMesh& mesh) const noexcept;
	std::vector<unsigned short> ExtractIndices(const aiMesh& mesh) const noexcept;
	std::shared_ptr<VertexBuffer> MakeVertexBindable(Graphics& gfx, const aiMesh& mesh, float scale = 1.0f) const noxnd;
	std::shared_ptr<IndexBuffer> MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const noxnd;
	std::vector<Technique> GetTechniques() const noexcept;
private:
	std::string MakeMeshTag(const aiMesh& mesh) const noexcept;
private:
	DV::VertexLayout vtxLayout;
	std::vector<Technique> techniques;
	std::string modelPath;
	std::string name;
};