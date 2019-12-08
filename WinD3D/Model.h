#pragma once
#include <Engine/Architecture/Drawable.h>
#include <filesystem>
#include <optional>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Mesh : public Drawable
{
public:
	Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bindable>> binds);
public:
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform)const noxnd;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class Model;
	friend class ModelWindow;
public:
	Node(std::string_view name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& Transform) noexcept
		:meshPtrs(std::move(meshPtrs)), name(name)
	{
		DirectX::XMStoreFloat4x4(&transform, Transform);
		DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
	}
public:
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform)const noxnd;
	void ShowTree(int& nodeIndexTracked, std::optional<int>& selectedIndex, Node*& pSelectedNode)const noexcept;
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
private:
	void AddChild(std::unique_ptr<Node>pChild)noxnd
	{
		assert(pChild);
		childPtrs.push_back(std::move(pChild));
	}
private:
	const std::string name;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMFLOAT4X4 appliedTransform;
};

class Model
{
public:
	Model(Graphics& gfx, const std::string filename);
	~Model()noexcept;
public:
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const std::filesystem::path& path);
public:
	std::unique_ptr<Node> ParseNode(const aiNode& node)noexcept;
	void Draw(Graphics& gfx)const noxnd;
	void ShowWindow(std::string_view windowName = "")noexcept;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	std::unique_ptr<class ModelWindow> pWindow;
};