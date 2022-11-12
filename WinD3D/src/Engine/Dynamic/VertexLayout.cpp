//#define DVTX_SOURCE_FILE
//#include <Engine/Dynamic/VertexLayout.h>
//
//
//#if _DEBUG
//
//std::array<std::wstring, size_t(DV::VertexLayout::ElementType::Count)> DV::VertexLayout::Typenames
//{
//	L"Position2D",
//	L"Position3D",
//	L"Texture2D",
//	L"Normal",
//	L"Tangent",
//	L"Bitangent",
//	L"Float3Color",
//	L"Float4Color",
//	L"BGRAColor"
//};
//
//#endif
//
//namespace DV
//{
//	ConstVertex::ConstVertex(const Vertex& v) noxnd
//		:
//	vertex(v)
//	{}
//
//	template<VertexLayout::ElementType type>
//	struct AttributeAiMeshFill
//	{
//		static constexpr void Exec(VertexBuffer* pBuf, const aiMesh& mesh) noxnd
//		{
//			for (auto end = mesh.mNumVertices, i = 0u; i < end; i++)
//			{
//				(*pBuf)[i].Attr<type>() = VertexLayout::Map<type>::Extract(mesh, i);
//			}
//		}
//	};
//	VertexBuffer::VertexBuffer(VertexLayout layout, size_t size) noxnd
//		:
//		layout(std::move(layout))
//	{
//		Reserve(size);
//	}
//	VertexBuffer::VertexBuffer(VertexLayout layout_in, const aiMesh& mesh)
//		:
//		layout(std::move(layout_in))
//	{
//		Reserve(mesh.mNumVertices);
//		for (size_t i = 0, end = layout.GetElementCount(); i < end; i++)
//		{
//			VertexLayout::Bridge<AttributeAiMeshFill>(layout.ResolveByIndex(i).GetType(), this, mesh);
//		}
//	}
//
//
//	const VertexLayout& VertexBuffer::GetLayout() const noexcept
//	{
//		return layout;
//	}
//	size_t VertexBuffer::Count() const noxnd
//	{
//		return buffer.size() / layout.Size();
//	}
//	size_t VertexBuffer::Size() const noxnd
//	{
//		return buffer.size();
//	}
//	const unsigned char* VertexBuffer::data() const noxnd
//	{
//		return buffer.data();
//	}
//	void VertexBuffer::Reserve(size_t size)
//	{
//		buffer.resize(buffer.size() + layout.Size() * size);
//	}
//	Vertex VertexBuffer::Back() noxnd
//	{
//		assert(!buffer.empty());
//		return Vertex{ buffer.data() + buffer.size() - layout.Size(), layout };
//	}
//	Vertex VertexBuffer::Front() noxnd
//	{
//		assert(buffer.empty());
//		return Vertex{ buffer.data(), layout };
//	}
//	Vertex VertexBuffer::operator[](size_t i)noxnd
//	{
//		assert(i < Count());
//		return Vertex{ buffer.data() + layout.Size() * i,layout };
//	}
//
//	ConstVertex VertexBuffer::Back() const noxnd
//	{
//		return const_cast<VertexBuffer*>(this)->Back();
//	}
//	ConstVertex VertexBuffer::Front() const noxnd
//	{
//		return const_cast<VertexBuffer*>(this)->Front();
//	}
//
//
//	VertexLayout::Element::Element(ElementType type, size_t offset)
//		: type(type),
//		offset(offset)
//	{}
//
//	size_t VertexLayout::Element::GetOffsetAfter() const noxnd
//	{
//		return offset + Size();
//	}
//	size_t VertexLayout::Element::GetOffset() const noexcept
//	{
//		return offset;
//	}
//	size_t VertexLayout::Element::Size() const noxnd
//	{
//		return SizeOf(type);
//	}
//
//	template<Type type>
//	struct SysSizeLookup
//	{
//		static constexpr auto Exec() noexcept
//		{
//			return sizeof(VertexLayout::Map<type>::SysType);
//		}
//	};
//	constexpr size_t VertexLayout::Element::SizeOf(ElementType type) noxnd
//	{
//		return Bridge<SysSizeLookup>(type);
//	}
//
//	template<VertexLayout::ElementType type>
//	struct CodeLookup
//	{
//		static constexpr auto Exec() noexcept
//		{
//			return VertexLayout::Map<type>::code;
//		}
//	};
//	const char* VertexLayout::Element::GetCode() const noexcept
//	{
//		return Bridge<CodeLookup>(type);
//	}
//
//	Type VertexLayout::Element::GetType() const noexcept
//	{
//		return type;
//	}
//
//	template<VertexLayout::ElementType type> struct DescGenerate {
//		static constexpr D3D11_INPUT_ELEMENT_DESC Exec(size_t offset) noexcept {
//			return {
//				VertexLayout::Map<type>::semantic,0,
//				VertexLayout::Map<type>::dxgiFormat,
//				0,(UINT)offset,D3D11_INPUT_PER_VERTEX_DATA,0
//			};
//		}
//	};
//	D3D11_INPUT_ELEMENT_DESC VertexLayout::Element::GetDesc() const noxnd
//	{
//		return Bridge<DescGenerate>(type, GetOffset());
//	}
//	bool VertexLayout::Has(ElementType type) const noexcept
//	{
//		for (auto& e : elements)
//		{
//			if (e.GetType() == type)
//			{
//				return true;
//			}
//		}
//		return false;
//	}
//}
