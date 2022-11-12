#include <Engine/Bindable/Stencil.h>
#include <Engine/Bindable/Codex.h>
#include <Engine/Deprecated/GraphicsThrows.h>
#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/GraphicsExceptions.h>

Stencil::Stencil(Graphics& gfx, Mode mode)
    :mode(mode)
{
    D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };

    if (mode == Mode::Write)
    {
        dsDesc.DepthEnable = FALSE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.StencilEnable = TRUE;
        dsDesc.StencilWriteMask = 0xFF;
        dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    }
    else if (mode == Mode::Mask)
    {
        dsDesc.DepthEnable = FALSE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.StencilEnable = TRUE;
        dsDesc.StencilReadMask = 0xFF;
        dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
        dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    }

    GetDevice(gfx)->CreateDepthStencilState(&dsDesc, &pStencil);
}

void Stencil::Bind(Graphics& gfx) noxnd
{
    INFOMAN_NOHR(gfx);
    GFX_THROW_INFO_ONLY(GetContext(gfx)->OMSetDepthStencilState(pStencil.Get(), 0xFF));
}
std::shared_ptr<Stencil> Stencil::Resolve(Graphics& gfx, Mode mode)
{
    return ver::Codex::Resolve<Stencil>(gfx, mode);
}
std::string Stencil::GenerateUID(Mode mode)
{
    using namespace std::string_literals;
    const auto modeName = [mode]() {
        switch (mode) {
        case Mode::Off:
            return "off"s;
        case Mode::Write:
            return "write"s;
        case Mode::Mask:
            return "mask"s;
        }
        return "ERROR"s;
    };
    return typeid(Stencil).name() + "#"s + modeName();
}
std::string Stencil::GetUID() const noexcept
{
    return GenerateUID(mode);
}
