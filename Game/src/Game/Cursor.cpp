#include <Game/Cursor.h>
#include <Engine/Scene/Plane.h>
#include <imgui.h>
#include <Engine/Scene/Plane.h>
#include <Engine/Bindable/TransformCBuf.h>
#include <Engine/Bindable/VertexBuffer.h>
#include <Engine/Bindable/IndexBuffer.h>
#include <Engine/Bindable/InputLayout.h>
#include <Engine/Bindable/VertexShader.h>

using namespace ver;

ver::Cursor::Cursor(Graphics& gfx, float scale)
	:scale(scale)
{
	namespace dx = DirectX;
	const auto geometryTag = "$splane." + std::to_string(scale);
	auto model = Plane::Make();
	model.Deform(DirectX::XMMatrixScaling(scale, scale, 1.0f));
	pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
	pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);

	Technique tech("Shadow");
	{
		Step draw("shadow");
		//draw.AddBindable(InputLayout::Resolve(gfx, vtxLayout, ver::VertexShader::Resolve(gfx, "solid.vs.cso")->GetBytecode()));
		draw.AddBindable(std::make_shared<TransformCbuf>(gfx));
		tech.AddStep(std::move(draw));
	}
	AddTechnique(std::move(tech));
}

void ver::Cursor::SpawnControlWindow()
{
	if (ImGui::Begin("Cursor"))
	{
		ImGui::SliderFloat("Scale", &scale, 0.1, 100.0f);
	}
	ImGui::End();
}
DirectX::XMMATRIX ver::Cursor::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixScaling(scale, scale, 1.0f) * DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}