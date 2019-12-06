#include "Model.h"
#include "BindableBase.h"

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
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(filename,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices
	);

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], filename));
	}
	pRoot = ParseNode(*pScene->mRootNode);
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const std::filesystem::path& path)
{
	namespace dx = DirectX;
	DV::VertexBuffer vertices(std::move(
		DV::VertexLayout{}
		+DV::Type::Position3D
		+ DV::Type::Normal
	));
	vertices.Reserve(mesh.mNumVertices);

	const auto meshTag = path.string() + "%" + mesh.mName.C_Str();

	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		vertices[i].Set< DV::Type::Position3D >(std::move(*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i])));
		vertices[i].Set< DV::Type::Normal >(std::move(*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i])));
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

	bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vertices));
	bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

	auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	bindablePtrs.push_back(std::move(pvs));

	bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPS.cso"));
	bindablePtrs.push_back(InputLayout::Resolve(gfx, vertices.GetLayout(), pvsbc));

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color{ 0.6f, 0.6f, 0.8f };
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	} pmc;
	bindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));

	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

std::unique_ptr<Node> Model::ParseNode(const aiNode& node)
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

	auto pNode = std::make_unique<Node>(std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(*node.mChildren[i]));
	}
	return pNode;
}
void Model::Draw(Graphics& gfx) const
{
	pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
}


