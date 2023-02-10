#include <Engine/Bindable/HullShader.h>
#include <Engine/Bindable/Codex.h>
#include <Engine/Util/GraphicsExceptions.h>
#include <d3dcompiler.h>

using namespace ver;

HullShader::HullShader(Graphics& gfx, std::filesystem::path xpath)
	:path(std::move(xpath))
{
	Initialize(gfx);
}

winrt::IAsyncAction HullShader::InitializeAsync(Graphics& gfx, std::filesystem::path path)
{
	co_await winrt::resume_background();
	this->path = std::move(path);
	Initialize(gfx);
}

void HullShader::Initialize(Graphics& gfx)
{
	winrt::com_ptr<ID3DBlob> pBlob;
	ver::check_graphics(D3DReadFileToBlob((L"..\\WinD3D\\shaders\\" + path.native()).c_str(), pBlob.put()));
	ver::check_graphics(GetDevice(gfx)->CreateHullShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, pHullShader.put()));
}

void HullShader::Bind(Graphics& gfx) noxnd
{
	(GetContext(gfx)->HSSetShader(pHullShader.get(), nullptr, 0u));
	ver::check_context();
}
std::shared_ptr<HullShader> HullShader::Resolve(Graphics& gfx, std::filesystem::path path)
{
	return ver::Codex::Resolve<HullShader>(gfx, std::move(path));
}
concurrency::task<std::shared_ptr<HullShader>> HullShader::ResolveAsync(Graphics& gfx, std::filesystem::path path)
{
	co_return co_await ver::Codex::ResolveAsync<HullShader>(gfx, std::move(path));
}
std::string HullShader::GenerateUID(const std::filesystem::path& path)
{
	return std::format("{}#{}", typeid(HullShader).name(), path.string());
}
std::string HullShader::GetUID() const noexcept
{
	return GenerateUID(path);
}