#pragma once
#include <Engine/Graphics.h>
#include <string>
#include <memory>
#include <filesystem>
#include <Framework/noexcept_if.h>

class Node;
class Mesh;
class ModelWindow;
struct aiMesh;
struct aiMaterial;
struct aiNode;

namespace RG
{
	class RenderGraph;
}

class Model
{
public:
	Model(Graphics& gfx, std::string_view pathString, float scale = 1.0f);
	void Submit() const noxnd;
	void SetRootTransform(DirectX::FXMMATRIX tf) noexcept;
	void Accept(class ModelProbe& probe);
	void LinkTechniques(RG::RenderGraph&);
	void UnlinkTechniques();
	~Model() noexcept;
private:
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node, float scale) noexcept;
private:
	std::unique_ptr<Node> pRoot;
	// sharing meshes here perhaps dangerous?
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
};