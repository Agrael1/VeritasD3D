#include <Engine/Bindable/PixelShader.h>
#include <Engine/Bindable/Codex.h>
#include <d3dcompiler.h>
#include <Shared/Checks.h>

using namespace ver;

PixelShader::PixelShader(Graphics& gfx, std::filesystem::path xpath)
	:path(std::move(xpath))
{
	Initialize(gfx);
}

ver::IAsyncAction PixelShader::InitializeAsync(Graphics& gfx, std::filesystem::path path)
{
	co_await winrt::resume_background();
	this->path = std::move(path);
	Initialize(gfx);
}

void PixelShader::Initialize(Graphics& gfx)
{
	winrt::com_ptr<ID3DBlob> pBlob;
	ver::check_hresult(D3DReadFileToBlob((L"..\\Shaders\\" + path.native()).c_str(), pBlob.put()));
	ver::check_hresult(GetDevice(gfx)->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, pPixelShader.put()));
}

void PixelShader::Bind(Graphics& gfx) noxnd
{
	GetContext(gfx)->PSSetShader(pPixelShader.get(), nullptr, 0u);
	ver::check_context();
}
std::shared_ptr<PixelShader> PixelShader::Resolve(Graphics& gfx, std::filesystem::path path)
{
	return ver::Codex::Resolve<PixelShader>(gfx, std::move(path));
}
concurrency::task<std::shared_ptr<PixelShader>> PixelShader::ResolveAsync(Graphics& gfx, std::filesystem::path path)
{
	co_return co_await ver::Codex::ResolveAsync<PixelShader>(gfx, std::move(path));
}
std::string PixelShader::GenerateUID(const std::filesystem::path& path)
{
	return std::format("{}#{}", typeid(PixelShader).name(), path.string());
}
std::string PixelShader::GetUID() const noexcept
{
	return GenerateUID(path);
}