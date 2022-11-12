#include <Engine/Bindable/VertexShader.h>
#include <Engine/Deprecated/GraphicsThrows.h>
#include <Engine/Bindable/Codex.h>
#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/GraphicsExceptions.h>
#include <d3dcompiler.h>

using namespace ver;

VertexShader::VertexShader(Graphics& gfx, std::filesystem::path xpath)
	:path(std::move(xpath))
{
	Initialize(gfx);
}
winrt::IAsyncAction ver::VertexShader::InitializeAsync(Graphics& gfx, std::filesystem::path xpath)
{
	co_await winrt::resume_background();
	path = (std::move(xpath));
	Initialize(gfx);
}
void ver::VertexShader::Initialize(Graphics& gfx)
{
	INFOMAN(gfx);
	GFX_THROW_INFO(D3DReadFileToBlob((L"..\\WinD3D\\shaders\\" + path.native()).c_str(), pBytecodeBlob.put()));
	GFX_THROW_INFO(GetDevice(gfx)->CreateVertexShader(
		pBytecodeBlob->GetBufferPointer(),
		pBytecodeBlob->GetBufferSize(),
		nullptr,
		pVertexShader.put()
	));
}
void VertexShader::Bind(Graphics& gfx) noxnd
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->VSSetShader(pVertexShader.get(), nullptr, 0u));
}

ID3DBlob* VertexShader::GetBytecode() const noexcept
{
	return pBytecodeBlob.get();
}

std::shared_ptr<VertexShader> VertexShader::Resolve(Graphics& gfx, std::filesystem::path xpath)
{
	return ver::Codex::Resolve<VertexShader>(gfx, std::move(xpath));
}
concurrency::task<std::shared_ptr<VertexShader>> ver::VertexShader::ResolveAsync(Graphics& gfx, std::filesystem::path path)
{
	return ver::Codex::ResolveAsync<VertexShader>(gfx, std::move(path));
}
std::string VertexShader::GenerateUID(const std::filesystem::path& path)
{
	return std::format("{}#{}", typeid(VertexShader).name(), path.string());
}
std::string VertexShader::GetUID() const noexcept
{
	return GenerateUID(path);
}