#include "PixelShader.h"
#include "GraphicsThrows.m"
#include "Codex.h"
#include <Framework/Utility.h>

PixelShader::PixelShader(Graphics& gfx, std::filesystem::path xpath)
	:path(std::move(xpath))
{
	Initialize(gfx);
}

winrt::IAsyncAction PixelShader::InitializeAsync(Graphics& gfx, std::filesystem::path path)
{
	co_await winrt::resume_background();
	this->path = std::move(path);
	Initialize(gfx);
}

void PixelShader::Initialize(Graphics& gfx)
{
	INFOMAN(gfx);
	winrt::com_ptr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob((L"VShader\\" + path.native()).c_str(), pBlob.put()));
	GFX_THROW_INFO(GetDevice(gfx)->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, pPixelShader.put()));
}

void PixelShader::Bind(Graphics& gfx) noxnd
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShader(pPixelShader.get(), nullptr, 0u));
}
std::shared_ptr<PixelShader> PixelShader::Resolve(Graphics& gfx, std::filesystem::path path)
{
	return Codex::Resolve<PixelShader>(gfx, std::move(path));
}
std::string PixelShader::GenerateUID(const std::filesystem::path& path)
{
	using namespace std::string_literals;
	return typeid(PixelShader).name() + "#"s + path.string();
}
std::string PixelShader::GetUID() const noexcept
{
	return GenerateUID(path);
}