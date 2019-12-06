#pragma once
#include <Engine/Architecture/Drawable.h>
#include <filesystem>

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
public:
	Node(std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& Transform) noexcept
		:meshPtrs(std::move(meshPtrs))
	{
		DirectX::XMStoreFloat4x4(&transform, Transform);
	}
public:
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform)const noxnd
	{
		const auto tMat = DirectX::XMLoadFloat4x4(&transform) * accumulatedTransform;
		for (const auto pm : meshPtrs)
		{
			pm->Draw(gfx, tMat);
		}
		for (const auto& pc : childPtrs)
		{
			pc->Draw(gfx, tMat);
		}
	}
private:
	void AddChild(std::unique_ptr<Node>pChild)noxnd
	{
		assert(pChild);
		childPtrs.push_back(std::move(pChild));
	}
private:
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
};

class Model
{
public:
	Model(Graphics& gfx, const std::string filename);
public:
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const std::filesystem::path& path);
public:
	std::unique_ptr<Node> ParseNode(const aiNode& node);
	void Draw(Graphics& gfx)const;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
};