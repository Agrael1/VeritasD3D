#pragma once
#include <Engine/Graphics.h>
#include <vector>
#include <filesystem>
#include <Engine/Dynamic/VertexBuffer.h>
#include <Engine/Rendering/Technique.h>

struct aiMaterial;
struct aiMesh;
class VertexBuffer;
class IndexBuffer;

namespace ver
{
	class IndexBuffer;
}

class Material
{
public:
	Material() = default;
	Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noxnd;
	ver::IAsyncAction	InitializeAsync(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path)noxnd;
public:
	static std::vector<uint16_t> ExtractIndices(const aiMesh& mesh) noexcept;
	std::shared_ptr<Bindable> MakeVertexBindable(Graphics& gfx, const aiMesh& mesh, float scale = 1.0f) const noxnd;
	std::shared_ptr<ver::IndexBuffer> MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const noxnd;
	std::vector<Technique> GetTechniques() const noexcept;
	ver::dv::LayoutSpan GetLayout()const noexcept
	{
		return vtxLayout;
	}
private:
	std::string MakeMeshTag(const aiMesh& mesh) const noexcept;
private:
	ver::dv::VertexLayout vtxLayout;
	std::vector<Technique> techniques;
	std::string modelPath;
	std::string name;
};
