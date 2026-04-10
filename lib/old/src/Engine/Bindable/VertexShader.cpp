#include <Engine/Bindable/VertexShader.h>
#include <Engine/Bindable/Codex.h>
#include <d3dcompiler.h>
#include <Shared/Checks.h>
#include <Shared/Log.h>

using namespace ver;

VertexShader::VertexShader(Graphics& gfx, std::filesystem::path xpath)
	:path(std::move(xpath))
{
	Initialize(gfx);
}
ver::IAsyncAction ver::VertexShader::InitializeAsync(Graphics& gfx, std::filesystem::path xpath)
{
	co_await winrt::resume_background();
	path = std::move(xpath);
	Initialize(gfx);
}
void ver::VertexShader::Initialize(Graphics& gfx)
{
	auto file = std::filesystem::path(shader_folder) / path;
	if (!std::filesystem::exists(file))
	{
		ver::std_error(std::format("File {} was not found.", file.string()));
		throw ver::exception{};
	}

	ver::check_hresult(D3DReadFileToBlob(file.c_str(), pBytecodeBlob.put()));
	ver::check_hresult(GetDevice(gfx)->CreateVertexShader(
		pBytecodeBlob->GetBufferPointer(),
		pBytecodeBlob->GetBufferSize(),
		nullptr,
		pVertexShader.put()
	));
}
void VertexShader::Bind(Graphics& gfx) noxnd
{
	Bind(*GetContext(gfx));
}
void ver::VertexShader::Bind(ID3D11DeviceContext& context) noxnd
{
	context.VSSetShader(pVertexShader.get(), nullptr, 0u);
	ver::check_context();
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