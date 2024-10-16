#include <Game/PointLight.h>
#include <Engine/Bindable/Light.h>
#include <imgui.h>


ver::LightSphere::LightSphere(LightBuffer& allocator, Graphics& gfx, float radius)
	:cbData(&allocator.Allocate()), mesh(gfx, radius)
{
	Reset();
}

ver::IAsyncAction ver::LightSphere::InitializeAsync(LightBuffer& allocator, Graphics& gfx, float radius)
{
	cbData = &allocator.Allocate();
	co_await mesh.InitializeAsync(gfx, radius);
	Reset();
}

void ver::LightSphere::SpawnControlWindow() noexcept
{
	changed = false;
	if (ImGui::Begin((std::to_string((size_t)(this)) + "Light").c_str()))
	{
		ImGui::Text("Position");
		changed |= ImGui::SliderFloat("X", &cbData->pos.x, -200.f, 200.f, "%.1f");
		changed |= ImGui::SliderFloat("Y", &cbData->pos.y, -200.f, 200.f, "%.1f");
		changed |= ImGui::SliderFloat("Z", &cbData->pos.z, -200.f, 200.f, "%.1f");

		ImGui::Text("Intensity/Color");
		ImGui::SliderFloat("Intensity", &cbData->diffuseIntensity, 0.01f, 2.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		changed |= ImGui::ColorEdit3("Diffuse Color", &cbData->diffuse.x);

		ImGui::Text("Falloff");
		ImGui::SliderFloat("Constant", &cbData->attConst, 0.05f, 10.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Linear", &cbData->attLin, 0.0001f, 4.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Quadratic", &cbData->attQuad, 0.0000001f, 10.0f, "%.7f", ImGuiSliderFlags_Logarithmic);

		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void ver::LightSphere::Reset() noexcept
{
	*cbData = {
		{ 10.0f,9.0f,2.5f,1.0f },
		{ 1.0f,1.0f,1.0f },
		1.0f,
		1.0f,
		0.045f,
		0.0075f,
		true
	};
	changed = true;
}

void ver::LightSphere::SetPosition(DirectX::XMFLOAT4A pos)
{
	mesh.SetPos({ pos.x, pos.y, pos.z });
	cbData->pos = pos;
}
DirectX::XMVECTOR ver::LightSphere::GetPosition()
{
	return DirectX::XMLoadFloat4A(&cbData->pos);
}
void ver::LightSphere::SetColor(DirectX::XMFLOAT3 color)
{
	mesh.SetColor(color);
	cbData->diffuse = color;
}

void ver::LightSphere::Submit() const noxnd
{
	if (changed)
	{
		mesh.SetPos((DirectX::XMFLOAT3A&)cbData->pos);
		mesh.SetColor(cbData->diffuse);
	}
	mesh.Submit();
}

void ver::LightSphere::Bind(Graphics& gfx) const noexcept
{
	if (changed)
	{
		mesh.UpdateColor(gfx);
	}
}

void ver::LightSphere::TurnOn()
{
	cbData->active = true;
}
void ver::LightSphere::TurnOff()
{
	cbData->active = false;
}
void ver::LightSphere::Toggle()
{
	cbData->active ^= true;
}
