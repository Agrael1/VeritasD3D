#pragma once
#include <Engine/Architecture/Drawable.h>
#include <Framework/Exception.h>
#include <filesystem>
#include <optional>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class ModelException : public Exception
{
public:
	ModelException(int line, const char* file, std::string note) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	const std::string& GetNote() const noexcept;
private:
	std::string note;
};

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
public:
	Node(int id, std::string_view name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& Transform) noexcept
		:meshPtrs(std::move(meshPtrs)), name(name), id(id)
	{
		DirectX::XMStoreFloat4x4(&transform, Transform);
		DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
	}
public:
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform)const noxnd;
	void ShowTree(Node*& pSelectedNode)const noexcept;
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	int GetID()const noexcept
	{
		return id;
	}
private:
	void AddChild(std::unique_ptr<Node>pChild)noxnd
	{
		assert(pChild);
		childPtrs.push_back(std::move(pChild));
	}
private:
	const std::string name;
	int id;
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
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path);
public:
	std::unique_ptr<Node> ParseNode(int& NextId, const aiNode& node)noexcept;
	void Draw(Graphics& gfx)const noxnd;
	void ShowWindow(std::string_view windowName = "")noexcept;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	std::unique_ptr<class ModelWindow> pWindow;
};