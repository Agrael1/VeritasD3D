#pragma once
#include <Engine/Graphics.h>
#include <Framework/noexcept_if.h>

class Model;
class Mesh;
class TechniqueProbe;
class ModelProbe;

class Node
{
	friend Model;
public:
	Node(int id, std::string_view, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noxnd;
	void Submit(DirectX::FXMMATRIX accumulatedTransform) const noxnd;
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;
	int GetId() const noexcept;
	bool HasChildren() const noexcept
	{
		return childPtrs.size() > 0;
	}
	void Accept(ModelProbe& probe);
	void Accept(TechniqueProbe& probe);
	const std::string& GetName() const
	{
		return name;
	}
private:
	void AddChild(std::unique_ptr<Node> pChild) noxnd;
private:
	std::string name;
	int id;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMFLOAT4X4 appliedTransform;
};