#include "SkinnedBox.h"
#include "Cube.h"
#include "BindableCommons.h"
#include <ImGUI/imgui.h>


SkinnedBox::SkinnedBox(Graphics& gfx)
{
	namespace dx = DirectX;
	auto tag = "SkinnedBox";

	auto vertex = DV::VertexLayout{}
		+ DV::Type::Position3D
		+ DV::Type::Normal
		+ DV::Type::Texture2D;

	auto model = Cube::MakeIndependentSkinned(vertex);
	model.CalcNormalsIndependentFlat();
	model.Deform(dx::XMMatrixScaling(5.0f, 5.0f, 5.0f));

	AddBind(VertexBuffer::Resolve(gfx, tag, model.vertices));

	AddBind(Texture::Resolve(gfx, "Materials\\crate_diffuse.png"));
	AddBind(Texture::Resolve(gfx, "Materials\\crate-normal.jpg",1));
	AddBind(Sampler::Resolve(gfx));

	auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "PhongNormalPS.cso"));
	AddBind(IndexBuffer::Resolve(gfx, tag, model.indices));
	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(std::make_shared<TransformUnified>(gfx, *this, 0u, 2u));
}

void SkinnedBox::Update(float dt) noexcept
{

}
void SkinnedBox::SpawnControlWindow(Graphics& gfx) noexcept
{
	if (ImGui::Begin("Box of cox"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &pitch, 0.995f * -90.0f, 0.995f * 90.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::Text("Shading");
		bool changed0 = ImGui::SliderFloat("Spec. Int.:", &pmc.specularIntensity, 0.0f, 1.0f);
		bool changed1 = ImGui::SliderFloat("Spec. Power:", &pmc.specularPower, 0.0f, 100.0f);
		bool checkNormals = pmc.normalEnabled == TRUE;
		bool changed2 = ImGui::Checkbox("Enable Normal Map:", &checkNormals);
		pmc.normalEnabled = checkNormals ? TRUE : FALSE;
		if (changed0 || changed1 || changed2)
		{
			QueryBindable<PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
		}
	}
	ImGui::End();
}
DirectX::XMMATRIX SkinnedBox::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return 	DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) *
		DirectX::XMMatrixTranslation(x, y, z);
}