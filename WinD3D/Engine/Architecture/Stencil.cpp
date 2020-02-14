#include "Stencil.h"

Stencil::Stencil(Graphics& gfx, Mode mode)
{
    D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = FALSE;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    const D3D11_DEPTH_STENCILOP_DESC defaultStencilOp =
    { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };
    dsDesc.FrontFace = defaultStencilOp;
    dsDesc.BackFace = defaultStencilOp;

    if (mode == Mode::Write)
    {
        dsDesc.StencilEnable = TRUE;
        dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    }
    if (mode == Mode::Mask)
    {
        dsDesc.DepthEnable = FALSE;
        dsDesc.StencilEnable = TRUE;
        dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
    }

    GetDevice(gfx)->CreateDepthStencilState(&dsDesc, &pStencil);
}

void Stencil::Bind(Graphics& gfx) noexcept
{
    GetContext(gfx)->OMSetDepthStencilState(pStencil.Get(), 0xFF);
}
