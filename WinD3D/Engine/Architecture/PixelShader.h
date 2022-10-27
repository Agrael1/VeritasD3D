#pragma once
#include "Bindable.h"
#include "pch.h"
#include <filesystem>


class PixelShader : public Bindable
{
public:
	PixelShader() = default;
	PixelShader(Graphics& gfx, std::filesystem::path path);
	winrt::IAsyncAction InitializeAsync(Graphics& gfx, std::filesystem::path path);
	void Initialize(Graphics& gfx);
public:
	void Bind(Graphics& gfx) noxnd override;
	static std::shared_ptr<PixelShader> Resolve(Graphics& gfx, std::filesystem::path path);
	static std::string GenerateUID(const std::filesystem::path& path);
	std::string GetUID() const noexcept;
protected:
	std::filesystem::path path;
	winrt::com_ptr<ID3D11PixelShader> pPixelShader;
};