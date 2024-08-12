#include <Engine/Scene/SolidCube.h>
#include <Engine/Scene/Cube.h>
#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Bindable/ConstantBuffer.h>
#include <Engine/Bindable/AsyncResource.h>


using namespace ver;

ver::SolidCube::SolidCube(Graphics& gfx, float width, float height)
	:colorBuffer(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst, 1u))
{
	namespace dx = DirectX;
    const auto geometryTag = "$scube." + std::to_string(width) + "." + std::to_string(height);;
	auto model = Cube::Make();
	model.Deform(dx::XMMatrixScalingFromVector(dx::XMVectorSet(width, 1, height, 0)));
	pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
	pIndices = ver::IndexBuffer::Resolve(gfx, geometryTag, model.indices);

	{
		Technique solid;
		Step only("lambertian");

		auto pvs = VertexShader::Resolve(gfx, "solid.vs.cso");
		auto pvsbc = pvs->GetBytecode();
		only.AddBindable(std::move(pvs));

		only.AddBindable(PixelShader::Resolve(gfx, "solid.ps.cso"));


		only.AddBindable(colorBuffer);
		only.AddBindable(InputLayout::Resolve(gfx, model.vertices.layout(), pvsbc));
		only.AddBindable(std::make_shared<TransformCbuf>(gfx));

		solid.AddStep(std::move(only));
		AddTechnique(std::move(solid));
	}
}

ver::IAsyncAction ver::SolidCube::InitializeAsync(Graphics& gfx, float width, float height)
{
	co_await winrt::resume_background();
	colorBuffer = std::make_shared<ver::PixelConstantBuffer<PSColorConstant>>(gfx, colorConst, 1u);
	namespace dx = DirectX;
	const auto geometryTag = "$scube." + std::to_string(width) + "." + std::to_string(height);;
	auto model = Cube::Make();
	model.Deform(dx::XMMatrixScalingFromVector(dx::XMVectorSet(width, 0, height, 0)));
	pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
	pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);

	{
		Technique solid;
		Step only("lambertian");

		auto pvs = VertexShader::Resolve(gfx, "solid.vs.cso");
		auto pvsbc = pvs->GetBytecode();
		only.AddBindable(std::move(pvs));

		only.AddBindable(PixelShader::Resolve(gfx, "solid.ps.cso"));


		only.AddBindable(colorBuffer);
		only.AddBindable(InputLayout::Resolve(gfx, model.vertices.layout(), pvsbc));
		only.AddBindable(std::make_shared<TransformCbuf>(gfx));

		solid.AddStep(std::move(only));
		AddTechnique(std::move(solid));
	}
}

void SolidCube::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}
void SolidCube::SetColor(DirectX::XMFLOAT3 color) noexcept
{
	colorConst.color = color;
}
void SolidCube::UpdateColor(Graphics& gfx) noexcept
{
	colorBuffer->Update(gfx, colorConst);
}
DirectX::XMMATRIX SolidCube::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}
