#include <Engine/Loading/Material.h>
#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Bindable/ConstantBuffersEx.h>
#include <Engine/Dynamic/DynamicConstant.h>
#include <assimp/types.h>

Material::Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noxnd
	:modelPath(path.string())
{
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
		std::string shaderCode = "phong.";
		aiString texFileName;

		// common (pre)
		vtxLayout
			+ DV::Type::Position3D
			+ DV::Type::Normal;
		DC::RawLayout pscLayout;
		bool hasTexture = false;
		bool hasGlossAlpha = false;

		// diffuse
		{
			bool hasAlpha = false;
			if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += "dif.";
				vtxLayout
					+ DV::Type::Texture2D;
				auto tex = ver::Texture::Resolve(gfx, rootPath + texFileName.C_Str());
				//if (tex->UsesAlpha())
				//{
				//	hasAlpha = true;
				//	shaderCode += "msk";
				//}
				step.AddBindable(std::move(tex));
			}
			else
			{
				pscLayout.Add({ {DC::Type::Float3, "materialColor"} });
			}
			step.AddBindable(RasterizerState::Resolve(gfx, hasAlpha));
		}
		// specular
		{
			if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += "spc.";
				vtxLayout
					+ DV::Type::Texture2D;
				auto tex = ver::Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 1);
				hasGlossAlpha = tex->UsesAlpha();
				step.AddBindable(std::move(tex));
				pscLayout.Add({
					{DC::Type::Bool, "useGlossAlpha"},
					{DC::Type::Bool,"useSpecularMap"} });
			}
			pscLayout.Add({
				{DC::Type::Float3,"specularColor"},
				{DC::Type::Float,"specularWeight"},
				{DC::Type::Float,"specularGloss"} });
		}
		// normal
		{
			if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += "nrm.";
				vtxLayout
					+ DV::Type::Texture2D
					+ DV::Type::Tangent
					+ DV::Type::Bitangent;
				step.AddBindable(ver::Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 2));
				pscLayout.Add({
					{DC::Type::Bool,"useNormalMap"},
					{DC::Type::Float,"normalMapWeight"} });
			}
		}
		// common (post)
		{
			step.AddBindable(std::make_shared<TransformCbuf>(gfx, 0u));
			auto pvs = VertexShader::Resolve(gfx, shaderCode + "vs.cso");
			auto pvsbc = pvs->GetBytecode();
			step.AddBindable(std::move(pvs));
			step.AddBindable(PixelShader::Resolve(gfx, shaderCode + "ps.cso"));
			step.AddBindable(InputLayout::Resolve(gfx, vtxLayout, pvsbc));
			if (hasTexture)
			{
				step.AddBindable(Sampler::Resolve(gfx));
			}
			// PS material params (cbuf)
			DC::Buffer buf{ std::move(pscLayout) };
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
	// outline technique
	//{
	//	Technique outline("Outline", false);
	//	{
	//		Step mask("outlineMask");

	//		// TODO: better sub-layout generation tech for future consideration maybe
	//		mask.AddBindable(InputLayout::Resolve(gfx, vtxLayout, VertexShader::Resolve(gfx, "Solid_VS.cso")->GetBytecode()));

	//		mask.AddBindable(std::make_shared<TransformCbuf>(gfx));

	//		// TODO: might need to specify rasterizer when doubled-sided models start being used

	//		outline.AddStep(std::move(mask));
	//	}
	//	{
	//		Step draw("outlineDraw");

	//		{
	//			DC::RawLayout lay;
	//			lay.Add({ {DC::Type::Float3,"materialColor"} });
	//			auto buf = DC::Buffer(std::move(lay));
	//			buf["materialColor"] = DirectX::XMFLOAT3{ 1.0f,0.4f,0.4f };
	//			draw.AddBindable(std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 1u));
	//		}

	//		// TODO: better sub-layout generation tech for future consideration maybe
	//		draw.AddBindable(InputLayout::Resolve(gfx, vtxLayout, VertexShader::Resolve(gfx, "Solid_VS.cso")->GetBytecode()));

	//		draw.AddBindable(std::make_shared<TransformCbuf>(gfx));

	//		// TODO: might need to specify rasterizer when doubled-sided models start being used

	//		outline.AddStep(std::move(draw));
	//	}
	//	techniques.push_back(std::move(outline));
	//}
}

winrt::Windows::Foundation::IAsyncAction
Material::MakeMaterialAsync(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noxnd
{
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
		std::string shaderCode = "phong.";
		aiString texFileName;

		// common (pre)
		vtxLayout
			+ DV::Type::Normal
			+ DV::Type::Position3D;
		DC::RawLayout pscLayout;
		bool hasTexture = false;
		bool hasGlossAlpha = false;
		bool bHasDiffuse = false;
		bool bHasSpecular = false;
		bool bHasNormal = false;

		// diffuse
		{
			if (bHasDiffuse = (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS))
			{
				hasTexture = true;
				shaderCode += "dif.";
				vtxLayout
					+ DV::Type::Texture2D;
				texes.emplace_back(ver::Texture::ResolveAsync(gfx, rootPath + texFileName.C_Str()));
			}
			else
			{
				pscLayout.Add({ {DC::Type::Float3, "materialColor"} });
			}
		}
		// specular
		{
			if (bHasSpecular = (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS))
			{
				hasTexture = true;
				shaderCode += "spc.";
				vtxLayout
					+ DV::Type::Texture2D;
				texes.emplace_back(ver::Texture::ResolveAsync(gfx, rootPath + texFileName.C_Str(), 1));
				pscLayout.Add({
					{DC::Type::Bool, "useGlossAlpha"},
					{DC::Type::Bool,"useSpecularMap"} });
			}
			pscLayout.Add({
				{DC::Type::Float3,"specularColor"},
				{DC::Type::Float,"specularWeight"},
				{DC::Type::Float,"specularGloss"} });
		}
		// normal
		{
			if (bHasNormal = (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS))
			{
				hasTexture = true;
				shaderCode += "nrm.";
				vtxLayout
					+ DV::Type::Texture2D
					+ DV::Type::Tangent
					+ DV::Type::Bitangent;
				texes.emplace_back(ver::Texture::ResolveAsync(gfx, rootPath + texFileName.C_Str(), 2));
				pscLayout.Add({
					{DC::Type::Bool,"useNormalMap"},
					{DC::Type::Float,"normalMapWeight"} });
			}
		}

		for (auto& x : texes)
		{
			co_await x;
		}

		
		/*if (bHasDiffuse)
		{
			bool hasAlpha = false;
			if (texes[0].get()->UsesAlpha())
			{
				hasAlpha = true;
				shaderCode.insert( 8, "Msk");
			}
			step.AddBindable(RasterizerState::Resolve(gfx, hasAlpha));
		}*/
		if (bHasSpecular)
		{
			hasGlossAlpha = texes[1].get()->UsesAlpha();
		}

		for (auto& x : texes)
		{
			step.AddBindable(std::move(x.get()));
		}

		// common (post)
		{
			step.AddBindable(std::make_shared<TransformCbuf>(gfx, 0u));
			auto pvs = VertexShader::Resolve(gfx, shaderCode + "vs.cso");
			auto pvsbc = pvs->GetBytecode();
			step.AddBindable(std::move(pvs));
			step.AddBindable(PixelShader::Resolve(gfx, shaderCode + "ps.cso"));
			step.AddBindable(InputLayout::Resolve(gfx, vtxLayout, pvsbc));
			if (hasTexture)
			{
				step.AddBindable(Sampler::Resolve(gfx));
			}
			// PS material params (cbuf)
			DC::Buffer buf{ std::move(pscLayout) };
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
	// outline technique
	//{
	//	Technique outline("Outline", false);
	//	{
	//		Step mask("outlineMask");

	//		// TODO: better sub-layout generation tech for future consideration maybe
	//		mask.AddBindable(InputLayout::Resolve(gfx, vtxLayout, VertexShader::Resolve(gfx, "Solid_VS.cso")->GetBytecode()));

	//		mask.AddBindable(std::make_shared<TransformCbuf>(gfx));

	//		// TODO: might need to specify rasterizer when doubled-sided models start being used

	//		outline.AddStep(std::move(mask));
	//	}
	//	{
	//		Step draw("outlineDraw");

	//		{
	//			DC::RawLayout lay;
	//			lay.Add({ {DC::Type::Float3,"materialColor"} });
	//			auto buf = DC::Buffer(std::move(lay));
	//			buf["materialColor"] = DirectX::XMFLOAT3{ 1.0f,0.4f,0.4f };
	//			draw.AddBindable(std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 1u));
	//		}

	//		// TODO: better sub-layout generation tech for future consideration maybe
	//		draw.AddBindable(InputLayout::Resolve(gfx, vtxLayout, VertexShader::Resolve(gfx, "Solid_VS.cso")->GetBytecode()));

	//		draw.AddBindable(std::make_shared<TransformCbuf>(gfx));

	//		// TODO: might need to specify rasterizer when doubled-sided models start being used

	//		outline.AddStep(std::move(draw));
	//	}
	//	techniques.push_back(std::move(outline));
	//}
}
DV::VertexBuffer Material::ExtractVertices(const aiMesh& mesh) const noexcept
{
	return { vtxLayout,mesh };
}
std::vector<unsigned short> Material::ExtractIndices(const aiMesh& mesh) const noexcept
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
std::shared_ptr<VertexBuffer> Material::MakeVertexBindable(Graphics& gfx, const aiMesh& mesh, float scale) const noxnd
{
	auto vtc = ExtractVertices(mesh);
	if (scale != 1.0f)
	{
		for (auto i = 0u; i < vtc.Count(); i++)
		{
			DirectX::XMFLOAT3& pos = vtc[i].Attr<DV::VertexLayout::ElementType::Position3D>();
			pos.x *= scale;
			pos.y *= scale;
			pos.z *= scale;
		}
	}
	return VertexBuffer::Resolve(gfx, MakeMeshTag(mesh), std::move(vtc));
}
std::shared_ptr<IndexBuffer> Material::MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const noxnd
{
	return IndexBuffer::Resolve(gfx, MakeMeshTag(mesh), ExtractIndices(mesh));
}
std::string Material::MakeMeshTag(const aiMesh& mesh) const noexcept
{
	return modelPath + "%" + mesh.mName.C_Str();
}
std::vector<Technique> Material::GetTechniques() const noexcept
{
	return techniques;
}