#include <Engine/Bindable/VertexShader.h>
#include <Engine/Deprecated/GraphicsThrows.h>
#include <Engine/Bindable/Codex.h>
#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/GraphicsExceptions.h>
#include <Engine/Util/Utility.h>
#include <d3dcompiler.h>


VertexShader::VertexShader(Graphics& gfx, const std::string& path)
	:path(path)
{
	INFOMAN(gfx);
	const std::wstring ShaderFolder{ L"..\\WinD3D\\shaders\\" };
	GFX_THROW_INFO(D3DReadFileToBlob((ShaderFolder + ToWide(path)).c_str(), &pBytecodeBlob));
	GFX_THROW_INFO(GetDevice(gfx)->CreateVertexShader(
		pBytecodeBlob->GetBufferPointer(),
		pBytecodeBlob->GetBufferSize(),
		nullptr,
		&pVertexShader
	));
}

void VertexShader::Bind(Graphics& gfx) noxnd
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->VSSetShader(pVertexShader.Get(), nullptr, 0u));
}

ID3DBlob* VertexShader::GetBytecode() const noexcept
{
	return pBytecodeBlob.Get();
}

std::shared_ptr<VertexShader> VertexShader::Resolve(Graphics& gfx, const std::string& path)
{
	return Codex::Resolve<VertexShader>(gfx, path);
}
std::string VertexShader::GenerateUID(const std::string& path)
{
	using namespace std::string_literals;
	return typeid(VertexShader).name() + "#"s + path;
}
std::string VertexShader::GetUID() const noexcept
{
	return GenerateUID(path);
}