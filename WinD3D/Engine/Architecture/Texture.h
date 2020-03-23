#pragma once
#include <Engine/Architecture/Bindable.h>
#include <memory>

class Texture : public Bindable
{
public:
	Texture(Graphics& gfx, std::string_view path, UINT slot = 0);
public:
	void Bind(Graphics& gfx) noexcept override;
	static std::shared_ptr<Texture> Resolve(Graphics& gfx, std::string_view path, UINT slot = 0);
	static std::string GenerateUID(std::string_view path, UINT slot = 0);
	std::string GetUID() const noexcept override;
	bool UsesAlpha() const noexcept;
private:
	static UINT CalculateNumberOfMipLevels(UINT width, UINT height) noexcept;
private:
	unsigned int slot;
protected:
	bool hasAlpha = false;
	std::string path;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};