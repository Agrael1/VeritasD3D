#include <Engine/Scene/Cursor.h>
#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Scene/Sphere.h>

#include <imgui.h>
#include <ImGuizmo.h>
#include <Engine/Graphics.h>


ver::Cursor::Cursor(Graphics& gfx)
	:colorBuffer(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst, 1u))
{
	namespace dx = DirectX;
	const auto geometryTag = "$ssphere." + std::to_string(radius);
	auto model = Sphere::Make();
	model.Deform(dx::XMMatrixScalingFromVector(dx::XMVectorReplicate(radius)));
	pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
	pIndices = ver::IndexBuffer::Resolve(gfx, geometryTag, model.indices);

	{
		Technique solid;
		Step only("lambertian");

		auto pvs = VertexShader::Resolve(gfx, "solid.vs.cso");
		auto pvsbc = pvs->GetBytecode();
		only.AddBindable(std::move(pvs));

		only.AddBindable(PixelShader::Resolve(gfx, "solid.ps.cso"));


		only.AddBindable(colorBuffer);
		only.AddBindable(InputLayout::Resolve(gfx, model.vertices.layout(), pvsbc));
		only.AddBindable(std::make_shared<TransformCbuf>(gfx));

		solid.AddStep(std::move(only));
		AddTechnique(std::move(solid));
	}
}
DirectX::XMMATRIX ver::Cursor::GetTransformXM() const noexcept
{
	auto m = ctm.GetTransform();
	return m ;
}




ver::Gizmo::Gizmo()
	:gizmo_op(ImGuizmo::TRANSLATE),
	gizmo_mode(ImGuizmo::WORLD),
	control_matrix(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1){}

void ver::Gizmo::Render(Graphics& gfx)
{
	if (ImGui::Begin("Cursor"))
	{
		if (ImGui::RadioButton("Translate", gizmo_op == ImGuizmo::TRANSLATE))
			gizmo_op = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", gizmo_op == ImGuizmo::ROTATE))
			gizmo_op = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", gizmo_op == ImGuizmo::SCALE))
			gizmo_op = ImGuizmo::SCALE;

		if (gizmo_op != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", gizmo_mode == ImGuizmo::LOCAL))
				gizmo_mode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", gizmo_mode == ImGuizmo::WORLD))
				gizmo_mode = ImGuizmo::WORLD;
		}

		auto vmr = gfx.ccamera;
		auto vp = gfx.GetProjection();

		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		ImGuizmo::Manipulate((float*)&vmr, (float*)&vp, gizmo_op, gizmo_mode, (float*)&control_matrix);
	}
	ImGui::End();
}
