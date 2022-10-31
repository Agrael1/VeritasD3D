#include <Engine/Bindable/NullPixelShader.h>
#include <Engine/Bindable/Codex.h>
#include <Engine/Deprecated/GraphicsThrows.h>
#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/GraphicsExceptions.h>

NullPixelShader::NullPixelShader(Graphics& gfx)
{
}
void NullPixelShader::Bind(Graphics& gfx) noxnd
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShader(nullptr, nullptr, 0u));
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
