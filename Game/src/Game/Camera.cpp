#include <Game/Camera.h>
#include "imgui.h"
#include <algorithm>
#include <numbers>

namespace dx = DirectX;

template<typename T>
T wrap_angle(T theta)
{
	T x = (T)fmod(theta + (T)dx::XM_PI, (T)dx::XM_2PI);
	if (x < 0)
		x += (T)dx::XM_2PI;
	return x - (T)dx::XM_PI;
}

DirectX::XMMATRIX ShearY(float x, float z)
{
	using namespace dx;
	DirectX::XMMATRIX i;
	i.r[0] = XMVectorSet(1,x,0,0);
	i.r[1] = g_XMIdentityR1.v;
	i.r[2] = XMVectorSet(0,z,1,0);
	i.r[3] = g_XMIdentityR3.v;
	return XMMatrixTranspose(i);
}
DirectX::XMMATRIX ShearX(float y, float z)
{
	using namespace dx;
	DirectX::XMMATRIX i;
	i.r[0] = g_XMIdentityR0.v;
	i.r[1] = XMVectorSet(y, 1, 0, 0);
	i.r[2] = XMVectorSet(z, 0, 1, 0);
	i.r[3] = g_XMIdentityR3.v;
	return XMMatrixTranspose(i);
}
DirectX::XMMATRIX ShearZ(float x, float y)
{
	using namespace dx;
	DirectX::XMMATRIX i;
	i.r[0] = XMVectorSet(1, 0, x, 0);
	i.r[1] = XMVectorSet(0, 1, y, 0);
	i.r[2] = g_XMIdentityR2.v;
	i.r[3] = g_XMIdentityR3.v;
	return XMMatrixTranspose(i);
}

Camera::Camera() noexcept
{
	Reset();
}

DirectX::XMMATRIX Camera::GetViewMatrix() const noexcept
{
	using namespace dx;
	const dx::XMVECTOR forwardBaseVector = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	// apply the camera rotations to a base vector
	const auto lookVector = dx::XMVector3Transform(forwardBaseVector,
		dx::XMMatrixRotationRollPitchYawFromVector(dx::XMLoadFloat2(&rot))
	);
	// generate camera transform (applied to all objects to arrange them relative
	// to camera position/orientation in world) from cam position and direction
	// camera "top" always faces towards +Y (cannot do a barrel roll)
	return XMMatrixLookToLH(dx::XMLoadFloat3(&pos), lookVector, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}
void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &rot.x, 0.995f * -90.0f, 0.995f * 90.0f);
		ImGui::SliderAngle("Yaw", &rot.y, -180.0f, 180.0f);
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}
void Camera::Reset() noexcept
{
	pos = { 0.4f, 1.9f, -3.7f };
	rot = { 0, std::numbers::pi / 2 };
	orbit = { 0.0f,0.0f };
}
void Camera::Rotate(float dx, float dy) noexcept
{
	rot.y = wrap_angle(rot.y + dx * rotationSpeed);
	rot.x = std::clamp(rot.x + dy * rotationSpeed, 0.995f * -dx::XM_PIDIV2, 0.995f * dx::XM_PIDIV2);
}
void Camera::Orbit(float dx, float dy) noexcept
{
	orbit.y = wrap_angle(orbit.y + dx * rotationSpeed);
	orbit.x = std::clamp(orbit.x + dy * rotationSpeed, 0.995f * -dx::XM_PIDIV2, 0.995f * dx::XM_PIDIV2);
}
void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	using namespace dx;
	dx::XMStoreFloat3(&pos, dx::XMLoadFloat3(&pos) + dx::XMVector3Transform(
		dx::XMLoadFloat3(&translation),
		dx::XMMatrixRotationRollPitchYawFromVector(dx::XMLoadFloat2(&rot)) *
		dx::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)
	));
}



/////////////////////////////////////////////////////////////////////////////////

constexpr float ShearCoefficient(float focus_dist, float eye_dist, int conv)
{
	return conv * eye_dist / focus_dist;
}

StereoCamera::StereoCamera() noexcept
{
	Reset();
}

DirectX::XMMATRIX StereoCamera::GetLeftViewMatrix() const noexcept
{
	using namespace dx;
	const dx::XMVECTOR forwardBaseVector = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	// apply the camera rotations to a base vector
	const auto lookVector = dx::XMVector3Transform(forwardBaseVector,
		dx::XMMatrixRotationRollPitchYawFromVector(dx::XMLoadFloat2(&rot))
	);
	// generate camera transform (applied to all objects to arrange them relative
	// to camera position/orientation in world) from cam position and direction
	// camera "top" always faces towards +Y (cannot do a barrel roll)

	XMVECTOR Forward = XMVector3Normalize(lookVector);
	XMVECTOR Right = XMVector3Normalize(XMVector3Cross(dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), Forward));
	XMVECTOR Up = XMVector3Cross(Forward, Right);


	XMVECTOR NegEyePosition = XMVectorNegate(XMLoadFloat3(&pos) - Right * eye_dist);

	XMVECTOR D0 = XMVector3Dot(Right, NegEyePosition);
	XMVECTOR D1 = XMVector3Dot(Up, NegEyePosition);
	XMVECTOR D2 = XMVector3Dot(Forward, NegEyePosition);

	XMMATRIX M;
	M.r[0] = XMVectorSelect(D0, Right, g_XMSelect1110.v);
	M.r[1] = XMVectorSelect(D1, Up, g_XMSelect1110.v);
	M.r[2] = XMVectorSelect(D2, Forward, g_XMSelect1110.v);
	M.r[3] = g_XMIdentityR3.v;

	M = XMMatrixTranspose(M) * ShearZ(ShearCoefficient(focus_distance, -eye_dist, conv), 0);

	return M;

	//return dx::XMMatrixLookToLH(
	//	XMLoadFloat3(&pos), lookVector, dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)) * ShearZ(-eye_dist, 0);
}
DirectX::XMMATRIX StereoCamera::GetRightViewMatrix() const noexcept
{
	using namespace dx;
	const dx::XMVECTOR forwardBaseVector = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	// apply the camera rotations to a base vector
	const auto lookVector = dx::XMVector3Transform(forwardBaseVector,
		dx::XMMatrixRotationRollPitchYawFromVector(dx::XMLoadFloat2(&rot))
	);
	// generate camera transform (applied to all objects to arrange them relative
	// to camera position/orientation in world) from cam position and direction
	// camera "top" always faces towards +Y (cannot do a barrel roll)
	XMVECTOR Forward = XMVector3Normalize(lookVector);
	XMVECTOR Right = XMVector3Normalize(XMVector3Cross(dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), Forward));
	XMVECTOR Up = XMVector3Cross(Forward, Right);


	XMVECTOR NegEyePosition = XMVectorNegate(XMLoadFloat3(&pos) + Right * eye_dist);

	XMVECTOR D0 = XMVector3Dot(Right, NegEyePosition);
	XMVECTOR D1 = XMVector3Dot(Up, NegEyePosition);
	XMVECTOR D2 = XMVector3Dot(Forward, NegEyePosition);

	XMMATRIX M;
	M.r[0] = XMVectorSelect(D0, Right, g_XMSelect1110.v);
	M.r[1] = XMVectorSelect(D1, Up, g_XMSelect1110.v);
	M.r[2] = XMVectorSelect(D2, Forward, g_XMSelect1110.v);
	M.r[3] = g_XMIdentityR3.v;

	M = XMMatrixTranspose(M) * ShearZ(ShearCoefficient(focus_distance, eye_dist, conv), 0);

	return M;

	/*return dx::XMMatrixLookToLH(
		XMLoadFloat3(&pos), lookVector, dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)) * ShearZ(eye_dist, 0);*/
}
DirectX::XMMATRIX StereoCamera::GetCentralViewMatrix() const noexcept
{
	using namespace dx;
	const dx::XMVECTOR forwardBaseVector = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	// apply the camera rotations to a base vector
	const auto lookVector = dx::XMVector3Transform(forwardBaseVector,
		dx::XMMatrixRotationRollPitchYawFromVector(dx::XMLoadFloat2(&rot))
	);
	// generate camera transform (applied to all objects to arrange them relative
	// to camera position/orientation in world) from cam position and direction
	// camera "top" always faces towards +Y (cannot do a barrel roll)
	return dx::XMMatrixLookToLH(
		XMLoadFloat3(&pos), lookVector, dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

void StereoCamera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Stereo Camera"))
	{
		ImGui::Text("Stereo Parameters");
		ImGui::SliderFloat("Eye distance", &eye_dist, 0.001f, 1.f, "%.3f");
		ImGui::SliderFloat("Focus distance", &focus_distance, 0.001f, 10.f, "%.3f");
		ImGui::SliderInt("Enable", &conv, 0, 1);
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &rot.x, 0.995f * -90.0f, 0.995f * 90.0f);
		ImGui::SliderAngle("Yaw", &rot.y, -180.0f, 180.0f);
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}
void StereoCamera::Reset() noexcept
{
	pos = { 0.4f, 1.9f, -3.7f };
	rot = { 0, std::numbers::pi / 2 };
	//orbit = { 0.0f,0.0f };
}
void StereoCamera::Rotate(float dx, float dy) noexcept
{
	rot.y = wrap_angle(rot.y + dx * rotationSpeed);
	rot.x = std::clamp(rot.x + dy * rotationSpeed, 0.995f * -dx::XM_PIDIV2, 0.995f * dx::XM_PIDIV2);
}
//void StereoCamera::Orbit(float dx, float dy) noexcept
//{
//	orbit.y = wrap_angle(orbit.y + dx * rotationSpeed);
//	orbit.x = std::clamp(orbit.x + dy * rotationSpeed, 0.995f * -dx::XM_PIDIV2, 0.995f * dx::XM_PIDIV2);
//}
void StereoCamera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	using namespace dx;
	dx::XMStoreFloat3(&pos, dx::XMLoadFloat3(&pos) + dx::XMVector3Transform(
		dx::XMLoadFloat3(&translation),
		dx::XMMatrixRotationRollPitchYawFromVector(dx::XMLoadFloat2(&rot)) *
		dx::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)
	));
}