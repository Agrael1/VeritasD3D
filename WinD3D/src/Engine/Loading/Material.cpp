#include <Engine/Loading/Material.h>
#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Bindable/HullShader.h>
#include <Engine/Bindable/DomainShader.h>
#include <Engine/Bindable/ConstantBuffer.h>
#include <Engine/Bindable/ConstantBuffersEx.h>
#include <Engine/Dynamic/DynamicConstant.h>
#include <assimp/types.h>
#include <assimp/material.h>
#include <assimp/mesh.h>

union Desc
{
	struct
	{
		bool diff;
		bool spec;
		bool nrm;
		bool height;
	};
	uint32_t value;
	bool Any()const { return value; }
};

Desc DescribeMaterial(const aiMaterial& material)
{
	return{
		material.GetTextureCount(aiTextureType::aiTextureType_DIFFUSE) > 0,
		material.GetTextureCount(aiTextureType::aiTextureType_SPECULAR) > 0,
		material.GetTextureCount(aiTextureType::aiTextureType_NORMALS) > 0,
		material.GetTextureCount(aiTextureType::aiTextureType_DISPLACEMENT) > 0,
	};
}

ver::dv::VertexLayout CreateLayout(Desc desc)
{
	using enum ver::dv::ElementType;
	ver::dv::VertexLayout _out;
	_out.append(Position3D);
	_out.append(Normal);
	if (desc.Any())
		_out.append(Texture3D);
	if (desc.nrm)
	{
		_out.append(Tangent);
		_out.append(Bitangent);
	}
	return _out;
}
DC::Buffer CreateConstantBuffer(Desc desc)
{
	DC::RawLayout pscLayout;
	if (!desc.diff)pscLayout.Add({ {DC::Type::Float3, "materialColor"} });
	if (desc.spec)
	{
		pscLayout.Add({
			{DC::Type::Bool, "useGlossAlpha"},
			{DC::Type::Bool,"useSpecularMap"} });
	}
	pscLayout.Add({
		{DC::Type::Float3,"specularColor"},
		{DC::Type::Float,"specularWeight"},
		{DC::Type::Float,"specularGloss"} });
	if (desc.nrm)
	{
		pscLayout.Add({
			{DC::Type::Bool,"useNormalMap"},
			{DC::Type::Float,"normalMapWeight"} });
	}
	pscLayout.Add({ { DC::Type::Float,"meshId" } });
	return { std::move(pscLayout) };
}
std::string GetShaderCode(Desc desc)
{
	std::string code{ "phong." };
	if (desc.diff)
		code += "dif.";
	if (desc.spec)
		code += "spc.";
	if (desc.nrm)
		code += "nrm.";
	if (desc.height)
		code += "ht.";
	return code;
}


Material::Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noxnd
	:modelPath(path.string())
{
	auto desc = DescribeMaterial(material);
	vtxLayout = CreateLayout(desc);

	const auto rootPath = path.parent_path().string() + "\\";
	{
		aiString tempName;
		material.Get(AI_MATKEY_NAME, tempName);
		name = tempName.C_Str();
	}
	// phong technique
	{
		Technique phong{ "Phong" };
		Step step("lambertian");
		aiString texFileName;

		// common (pre)
		bool hasGlossAlpha = false;

		// diffuse
		{
			bool hasAlpha = false;
			if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
			{
				auto tex = ver::Texture::Resolve(gfx, rootPath + texFileName.C_Str());
				//if (tex->UsesAlpha())
				//{
				//	hasAlpha = true;
				//	//step.SetTargetPass("transparent");
				//	//shaderCode += "msk.";
				//}
				step.AddBindable(std::move(tex));
			}
			//step.AddBindable(RasterizerState::Resolve(gfx, hasAlpha));
		}
		// specular
		{
			if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
			{
				auto tex = ver::Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 1);
				hasGlossAlpha = tex->UsesAlpha();
				step.AddBindable(std::move(tex));
			}
		}
		// normal
		{
			if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
			{
				step.AddBindable(ver::Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 2));
			}
		}
		// common (post)
		{
			auto shaderCode = GetShaderCode(desc);
			step.AddBindable(std::make_shared<TransformCbuf>(gfx, 0u));
			auto pvs = ver::VertexShader::Resolve(gfx, shaderCode + "vs.cso");
			auto pvsbc = pvs->GetBytecode();
			step.AddBindable(std::move(pvs));
			step.AddBindable(ver::PixelShader::Resolve(gfx, shaderCode + "ps.cso"));
			step.AddBindable(InputLayout::Resolve(gfx, vtxLayout, pvsbc, true));
			if (desc.Any())
				step.AddBindable(ver::Sampler::Resolve(gfx));

			// PS material params (cbuf)
			DC::Buffer buf{ CreateConstantBuffer(desc) };
			if (auto r = buf["materialColor"]; r.Exists())
			{
				aiColor3D color = { 0.45f,0.45f,0.85f };
				material.Get(AI_MATKEY_COLOR_DIFFUSE, color);
				r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}
			buf["useGlossAlpha"].SetIfExists(hasGlossAlpha);
			buf["useSpecularMap"].SetIfExists(true);
			if (auto r = buf["specularColor"]; r.Exists())
			{
				aiColor3D color = { 0.18f,0.18f,0.18f };
				material.Get(AI_MATKEY_COLOR_SPECULAR, color);
				r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}
			buf["specularWeight"].SetIfExists(1.0f);
			if (auto r = buf["specularGloss"]; r.Exists())
			{
				float gloss = 8.0f;
				material.Get(AI_MATKEY_SHININESS, gloss);
				r = gloss;
			}
			buf["useNormalMap"].SetIfExists(true);
			buf["normalMapWeight"].SetIfExists(1.0f);
			step.AddBindable(std::make_unique<CachingPixelConstantBufferEx>(gfx, std::move(buf), 1u));
		}
		phong.AddStep(std::move(step));
		techniques.push_back(std::move(phong));
	}
}

winrt::Windows::Foundation::IAsyncAction
Material::InitializeAsync(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noxnd
{
	static std::atomic<float> mesh_id = 1.0f;
	float xmid = mesh_id;
	mesh_id += 1.0f;

	auto desc = DescribeMaterial(material);
	vtxLayout = CreateLayout(desc);

	modelPath = path.string();
	const auto rootPath = path.parent_path().string() + "\\";
	{
		aiString tempName;
		material.Get(AI_MATKEY_NAME, tempName);
		name = tempName.C_Str();
	}

	std::vector<concurrency::task<std::shared_ptr<ver::Texture>>> texes;
	texes.reserve(3);
	// phong technique
	{
		Technique phong{ "Phong" };
		Step step("lambertian");
		aiString texFileName;

		// common (pre)
		bool hasGlossAlpha = false;

		// diffuse
		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
			texes.emplace_back(ver::Texture::ResolveAsync(gfx, rootPath + texFileName.C_Str(), 0));
		// specular
		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
			texes.emplace_back(ver::Texture::ResolveAsync(gfx, rootPath + texFileName.C_Str(), 1));
		// normal
		if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
			texes.emplace_back(ver::Texture::ResolveAsync(gfx, rootPath + texFileName.C_Str(), 2));
		
		if (material.GetTexture(aiTextureType_DISPLACEMENT, 0, &texFileName) == aiReturn_SUCCESS)
			texes.emplace_back(ver::Texture::ResolveAsync(gfx, rootPath + texFileName.C_Str(), 3));

		if (desc.spec)
			hasGlossAlpha = texes[1].get()->UsesAlpha();

		for (auto& x : texes)
			step.AddBindable(std::move(x.get()));

		// common (post)
		{
			auto shaderCode = GetShaderCode(desc);
			auto vstask = ver::VertexShader::ResolveAsync(gfx, shaderCode + "vs.cso");
			auto pstask = ver::PixelShader::ResolveAsync(gfx, shaderCode + "ps.cso");

			step.AddBindable(std::make_shared<TransformCbuf>(gfx, 0u));

			if (desc.Any())
				step.AddBindable(ver::Sampler::Resolve(gfx));
			// PS material params (cbuf)
			DC::Buffer buf{ CreateConstantBuffer(desc) };
			if (auto r = buf["materialColor"]; r.Exists())
			{
				aiColor3D color = { 0.45f,0.45f,0.85f };
				material.Get(AI_MATKEY_COLOR_DIFFUSE, color);
				r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}
			buf["useGlossAlpha"].SetIfExists(hasGlossAlpha);
			buf["useSpecularMap"].SetIfExists(true);
			if (auto r = buf["specularColor"]; r.Exists())
			{
				aiColor3D color = { 0.18f,0.18f,0.18f };
				material.Get(AI_MATKEY_COLOR_SPECULAR, color);
				r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}
			buf["specularWeight"].SetIfExists(1.0f);
			if (auto r = buf["specularGloss"]; r.Exists())
			{
				float gloss = 8.0f;
				material.Get(AI_MATKEY_SHININESS, gloss);
				r = gloss;
			}
			buf["useNormalMap"].SetIfExists(true);
			buf["normalMapWeight"].SetIfExists(1.0f);
			buf["meshId"].SetIfExists(xmid);
			step.AddBindable(std::make_unique<CachingPixelConstantBufferEx>(gfx, std::move(buf), 1u));

			auto pvs = co_await vstask;
			auto pvsbc = pvs->GetBytecode();
			step.AddBindable(std::move(pvs));
			step.AddBindable(co_await pstask);
			step.AddBindable(InputLayout::Resolve(gfx, vtxLayout, pvsbc, true));
		}
		phong.AddStep(std::move(step));
		techniques.push_back(std::move(phong));
	}
	{
		Technique map{ "ShadowMap",true };
		{
			Step draw("shadow");

			// TODO: better sub-layout generation tech for future consideration maybe
			draw.AddBindable(InputLayout::Resolve(gfx, vtxLayout, ver::VertexShader::Resolve(gfx, "solid.vs.cso")->GetBytecode()));

			draw.AddBindable(std::make_shared<TransformCbuf>(gfx));

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			map.AddStep(std::move(draw));
		}
		techniques.push_back(std::move(map));
	}
	{
		Technique map{ "Cursor",true };
		{
			Step draw("cursor");

			draw.AddBindable(InputLayout::Resolve(gfx, vtxLayout, ver::VertexShader::Resolve(gfx, "solid.vs.cso")->GetBytecode()));

			draw.AddBindable(std::make_shared<TransformCbuf>(gfx));
			draw.AddBindable(std::make_shared<ver::PixelConstantBuffer<float>>(gfx, xmid, 3));

			map.AddStep(std::move(draw));
		}
		techniques.push_back(std::move(map));
	}
}
std::vector<unsigned short> Material::ExtractIndices(const aiMesh& mesh) noexcept
{
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
	return indices;
}

#include <memory_resource>
#include <Engine/Bindable/IndexBuffer.h>

auto prescale(std::span<aiVector3D> mesh, float scale)
{
	std::vector<aiVector3D> copy;
	copy.reserve(mesh.size());
	for (auto& i : mesh)
		copy.push_back(i * scale);
	return copy;
}

std::shared_ptr<Bindable> Material::MakeVertexBindable(Graphics& gfx, const aiMesh& mesh, float scale) const noxnd
{
	void* a[size_t(ver::dv::ElementType::Count)]{};
	std::pmr::monotonic_buffer_resource b{ a, sizeof(a) };
	std::pmr::vector<void*> vb{ decltype(vb)::allocator_type{&b} };
	vb.reserve(vtxLayout.count());
	auto x = prescale({ mesh.mVertices, mesh.mNumVertices }, scale);
	vb.push_back(x.data());
	vb.push_back(mesh.mNormals);
	if (vtxLayout.contains(ver::dv::ElementType::Texture3D))
		vb.push_back(mesh.mTextureCoords[0]);
	if (vtxLayout.contains(ver::dv::ElementType::Tangent))
	{
		vb.push_back(mesh.mTangents);
		vb.push_back(mesh.mBitangents);
	}
	return std::make_shared<VertexMultibuffer>(gfx, vtxLayout, vb, mesh.mNumVertices);
}
std::shared_ptr<ver::IndexBuffer> Material::MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const noxnd
{
	return ver::IndexBuffer::Resolve(gfx, MakeMeshTag(mesh), ExtractIndices(mesh));
}
std::string Material::MakeMeshTag(const aiMesh& mesh) const noexcept
{
	return modelPath + "%" + mesh.mName.C_Str();
}
std::vector<Technique> Material::GetTechniques() const noexcept
{
	return techniques;
}