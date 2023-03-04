#pragma once
#include <Engine/Bindable/CursorTransform.h>
#include <Engine/Scene/SolidSphere2.h>

namespace ImGuizmo
{
	enum OPERATION;
	enum MODE;
}

namespace ver
{
	class Cursor : public Drawable
	{
		 inline static constexpr float radius = 0.2f;
	public:
		Cursor(Graphics& gfx);
	public:
		DirectX::XMMATRIX GetTransformXM() const noexcept override;		
	private:
		struct PSColorConstant
		{
			DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
			float padding;
		} colorConst;
		std::shared_ptr<ver::PixelConstantBuffer<PSColorConstant>> colorBuffer;
		CursorTransform ctm;
	};

	class Gizmo
	{
	public:
		Gizmo();
	public:
		void Render(Graphics& gfx);
		void SetPosition(DirectX::XMVECTOR xposition)noexcept
		{
			DirectX::XMStoreFloat3(&position, xposition);
			DirectX::XMStoreFloat4x4(&control_matrix, DirectX::XMMatrixTranslationFromVector(xposition));
		}
		void SetPosition(DirectX::XMFLOAT3 xposition)noexcept
		{
			SetPosition(DirectX::XMLoadFloat3(&xposition));
		}
		DirectX::XMVECTOR GetPosition()const noexcept
		{
			return DirectX::XMLoadFloat3(&position);
		}
	private:
		ImGuizmo::OPERATION gizmo_op;
		ImGuizmo::MODE gizmo_mode;
		DirectX::XMFLOAT4X4 control_matrix;
		DirectX::XMFLOAT3 position;
	};
}