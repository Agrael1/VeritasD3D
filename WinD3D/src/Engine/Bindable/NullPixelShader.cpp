#include <Engine/Bindable/NullPixelShader.h>
#include <Engine/Bindable/Codex.h>
#include <Shared/Checks.h>
#include <d3d11.h>


NullPixelShader::NullPixelShader(Graphics& gfx)
{
}
void NullPixelShader::Bind(Graphics& gfx) noxnd
{
	GetContext(gfx)->PSSetShader(nullptr, nullptr, 0u);
	ver::check_context();
}
std::shared_ptr<NullPixelShader> NullPixelShader::Resolve(Graphics& gfx)
{
	return ver::Codex::Resolve<NullPixelShader>(gfx);
}
std::string NullPixelShader::GenerateUID()
{
	return typeid(NullPixelShader).name();
}
std::string NullPixelShader::GetUID() const noexcept
{
	return GenerateUID();
}
