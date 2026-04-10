#include <Engine/Bindable/DomainShader.h>
#include <Engine/Bindable/Codex.h>
#include <d3dcompiler.h>
#include <Shared/Checks.h>

using namespace ver;

DomainShader::DomainShader(Graphics& gfx, std::filesystem::path xpath)
	:path(std::move(xpath))
{
	Initialize(gfx);
}

ver::IAsyncAction DomainShader::InitializeAsync(Graphics& gfx, std::filesystem::path path)
{
	co_await winrt::resume_background();
	this->path = std::move(path);
	Initialize(gfx);
}

void DomainShader::Initialize(Graphics& gfx)
{
	winrt::com_ptr<ID3DBlob> pBlob;
	ver::check_hresult(D3DReadFileToBlob((L"..\\shaders\\" + path.native()).c_str(), pBlob.put()));
	ver::check_hresult(GetDevice(gfx)->CreateDomainShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, pDomainShader.put()));
}

void DomainShader::Bind(Graphics& gfx) noxnd
{
	(GetContext(gfx)->DSSetShader(pDomainShader.get(), nullptr, 0u));
	ver::check_context();
}
std::shared_ptr<DomainShader> DomainShader::Resolve(Graphics& gfx, std::filesystem::path path)
{
	return ver::Codex::Resolve<DomainShader>(gfx, std::move(path));
}
concurrency::task<std::shared_ptr<DomainShader>> DomainShader::ResolveAsync(Graphics& gfx, std::filesystem::path path)
{
	co_return co_await ver::Codex::ResolveAsync<DomainShader>(gfx, std::move(path));
}
std::string DomainShader::GenerateUID(const std::filesystem::path& path)
{
	return std::format("{}#{}", typeid(DomainShader).name(), path.string());
}
std::string DomainShader::GetUID() const noexcept
{
	return GenerateUID(path);
}