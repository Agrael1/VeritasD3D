#pragma once
#include <filesystem>
#include <Engine/Loading/Mesh.h>
#include <Engine/Loading/Node.h>

class Graphics;
class Node;
class Mesh;
class ModelWindow;
struct aiMesh;
struct aiMaterial;
struct aiNode;
struct aiScene;

namespace RG
{
	class RenderGraph;
}

class Model
{
public:
	Model() = default;
	[[deprecated("Use InitializeAsync with default c-tor")]] 
	Model(Graphics& gfx, std::filesystem::path pathString, float scale = 1.0f);
	~Model();

	ver::IAsyncAction InitializeAsync(Graphics& gfx, std::filesystem::path path, float scale = 1.0f);
	ver::IAsyncAction InitializeAsync(Graphics& gfx, const aiScene& scene, std::filesystem::path path, float scale = 1.0f);
	void Initialize(Graphics& gfx, const aiScene& scene, std::filesystem::path path, float scale = 1.0f);
public:
	void Submit() const noxnd;

	void SetRootTransform(DirectX::FXMMATRIX tf) noexcept
	{
		pRoot->SetAppliedTransform(tf);
	}
	void AppendRootTransform(DirectX::FXMMATRIX tf) noexcept
	{
		using namespace DirectX;
		auto m = DirectX::XMLoadFloat4x4(&pRoot->GetAppliedTransform());
		pRoot->SetAppliedTransform(m*tf);
	}
	void PrependRootTransform(DirectX::FXMMATRIX tf) noexcept
	{
		using namespace DirectX;
		auto m = DirectX::XMLoadFloat4x4(&pRoot->GetAppliedTransform());
		pRoot->SetAppliedTransform(tf*m);
	}

	void Accept(class ModelProbe& probe);
	void LinkTechniques(RG::RenderGraph&);
	void UnlinkTechniques();
private:
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node, float scale) noexcept;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
};