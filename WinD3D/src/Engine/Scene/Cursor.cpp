#include <Engine/Scene/Cursor.h>
#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Scene/Sphere.h>


ver::Cursor::Cursor(Graphics& gfx)
	:colorBuffer(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst, 1u))
{
	namespace dx = DirectX;
	const auto geometryTag = "$ssphere." + std::to_string(radius);
	auto model = Sphere::Make();
	model.Deform(dx::XMMatrixScalingFromVector(dx::XMVectorReplicate(radius)));
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
DirectX::XMMATRIX ver::Cursor::GetTransformXM() const noexcept
{
	return ctm.GetTransform();
}
