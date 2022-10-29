#pragma once
#include <Graphics.h>
#include <filesystem>
#include <Framework/noexcept_if.h>
#include <winrt/Windows.Foundation.h>

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
	Model() = default;
	Model(Graphics& gfx, std::string_view pathString, float scale = 1.0f);
	~Model() noexcept;

	winrt::Windows::Foundation::IAsyncAction InitializeAsync(Graphics& gfx, std::string_view pathString, float scale = 1.0f);
public:
	void Submit() const noxnd;
	void SetRootTransform(DirectX::FXMMATRIX tf) noexcept;
	void Accept(class ModelProbe& probe);
	void LinkTechniques(RG::RenderGraph&);
	void UnlinkTechniques();

	static winrt::Windows::Foundation::IAsyncAction
		MakeModelAsync(std::unique_ptr<Model>& to, Graphics& gfx, std::string_view pathString, float scale = 1.0f);
private:
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node, float scale) noexcept;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;	// sharing meshes here perhaps dangerous?
};