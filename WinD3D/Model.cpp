#include "Model.h"
#include "BindableBase.h"
#include "Texture.h"
#include "Sampler.h"
#include <ImGUI/imgui.h>
#include <fmt/printf.h>

ModelException::ModelException(int line, const char* file, std::string note) noexcept
	:
	Exception(line, file),
	note(std::move(note))
{}

const char* ModelException::what() const noexcept
{
	auto s = fmt::sprintf("%s:\n [Note]: %s ", Exception::what(), note);
	whatBuffer = s;
	return whatBuffer.c_str();
}
const char* ModelException::GetType() const noexcept
{
	return "Veritas Model Exception";
}
const std::string& ModelException::GetNote() const noexcept
{
	return note;
}


class ModelWindow // pImpl idiom, only defined in this .cpp
{
public:
	void Show(std::string_view windowName, const Node& root) noexcept
	{
		// window name defaults to "Model"
		windowName = windowName.empty() ? "Model" : windowName;
		// need an ints to track node indices and selected node
		int nodeIndexTracker = 0;
		if (ImGui::Begin(windowName.data()))
		{
			ImGui::Columns(2, nullptr, true);
			root.ShowTree(pSelectedNode);

			ImGui::NextColumn();
			if (pSelectedNode != nullptr)
			{
				auto& transform = transforms[pSelectedNode->GetID()];
				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);
			}
		}
		ImGui::End();
	}
	DirectX::XMMATRIX GetTransform() const noexcept
	{
		const auto& transform = transforms.at(pSelectedNode->GetID());
		return
			DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
			DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}
	Node* GetSelectedNode() const noexcept
	{
		return pSelectedNode;
	}
private:
	Node* pSelectedNode;
	struct TransformParameters
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	std::unordered_map<int, TransformParameters> transforms;
};


Mesh::Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bindable>> binds)
{
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	for (auto& pb : binds)
	{
		AddBind(std::move(pb));
	}
	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}
void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform)const noxnd
{
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Draw(gfx);
}


Model::Model(Graphics& gfx, const std::string filename)
	:pWindow(std::make_unique< ModelWindow >())
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(filename,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals
	);

	if (!pScene)
		throw ModelException(__LINE__, __FILE__, imp.GetErrorString());

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials, filename));
	}
	int NextID = 0;
	pRoot = ParseNode(NextID, *pScene->mRootNode);
}
Model::~Model() noexcept //pImpl idiom 
{
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path)
{
	namespace dx = DirectX;
	bool hasSpecMap = false;

	const auto rootPath = path.parent_path().string() + "\\";

	DV::VertexBuffer vertices(std::move(
		DV::VertexLayout{}
		+DV::Type::Position3D
		+ DV::Type::Normal
		+ DV::Type::Texture2D
	));
	vertices.Reserve(mesh.mNumVertices);

	const auto meshTag = path.string() + "%" + mesh.mName.C_Str();

	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		vertices[i].Set< DV::Type::Position3D >(std::move(*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i])));
		vertices[i].Set< DV::Type::Normal >(std::move(*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i])));
		vertices[i].Set<DV::Type::Texture2D>(std::move(*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])));
	}

	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	std::vector<std::shared_ptr<Bindable>> bindablePtrs;

	if (mesh.mMaterialIndex >= 0)
	{
		auto& material = *pMaterials[mesh.mMaterialIndex];
		aiString texFileName;
		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == 0)
		{
			bindablePtrs.push_back(Texture::Resolve(gfx, rootPath + texFileName.C_Str()));
		}
		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == 0)
		{
			bindablePtrs.push_back(Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 1));
			hasSpecMap = true;
		}
		bindablePtrs.push_back(Sampler::Resolve(gfx));
	}

	bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vertices));
	bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

	auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	bindablePtrs.push_back(std::move(pvs));

	if(hasSpecMap)
		bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSSpecular.cso"));
	else
	{
		bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPS.cso"));
		struct PSMaterialConstant
		{
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[2];
		} pmc;
		bindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));
	}

	bindablePtrs.push_back(InputLayout::Resolve(gfx, vertices.GetLayout(), pvsbc));

	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

std::unique_ptr<Node> Model::ParseNode(int& NextId, const aiNode& node)noexcept
{
	namespace dx = DirectX;
	const auto transform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	));

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
	}

	auto pNode = std::make_unique<Node>(NextId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(NextId, *node.mChildren[i]));
	}
	return pNode;
}
void Model::Draw(Graphics& gfx) const noxnd
{
	if (auto node = pWindow->GetSelectedNode())
	{
		node->SetAppliedTransform(pWindow->GetTransform());
	}
	pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
}
void Model::ShowWindow(std::string_view windowName) noexcept
{
	pWindow->Show(windowName, *pRoot);
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd
{
	const auto tMat = DirectX::XMLoadFloat4x4(&appliedTransform) * DirectX::XMLoadFloat4x4(&transform)*  accumulatedTransform;
	for (const auto pm : meshPtrs)
	{
		pm->Draw(gfx, tMat);
	}
	for (const auto& pc : childPtrs)
	{
		pc->Draw(gfx, tMat);
	}
}
void Node::ShowTree(Node*& pSelectedNode) const noexcept
{
	const int selectedID = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetID();
	// build up flags for current node
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ((GetID() == selectedID) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((childPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);
	// render this node
	const auto expanded = ImGui::TreeNodeEx(
		(void*)(intptr_t)GetID(), node_flags, name.c_str()
	);
	// processing for selecting node
	if (ImGui::IsItemClicked())
	{
		pSelectedNode = const_cast<Node*>(this);
	}
	// recursive rendering of open node's children
	if (expanded)
	{
		for (const auto& pChild : childPtrs)
		{
			pChild->ShowTree(pSelectedNode);
		}
		ImGui::TreePop();
	}
}
void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	DirectX::XMStoreFloat4x4(&appliedTransform, transform);
}
