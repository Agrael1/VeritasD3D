#pragma once
#include <Engine/Architecture/Drawable.h>

class SkinnedBox : public Drawable
{
public:
	SkinnedBox(Graphics& gfx);
public:
	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	// positional
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};