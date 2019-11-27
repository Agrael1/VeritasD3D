#pragma once
#include <vector>
#include <array>
#include <DirectXMath.h>
#include <Framework\noexcept_if.h>
#include <Fmtlib\include\fmt\printf.h>
#include <Engine\Graphics.h>

struct BGRAColor
{
	unsigned char a;
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

class VertexLayout
{
public:
	enum ElementType : size_t
	{
		Position2D,
		Position3D,
		Texture2D,
		Normal,
		Tangent,
		Bitangent,
		Float3Color,
		Float4Color,
		BGRAColor,
#if _DEBUG
		Count,
#endif
	};
#if _DEBUG
	std::array<std::string, size_t(ElementType::Count)> Typenames
	{
		"Position2D",
		"Position3D",
		"Texture2D",
		"Normal",
		"Tangent",
		"Bitangent",
		"Float3Color",
		"Float4Color",
		"BGRAColor"
	};
#endif
	template<ElementType> struct Map;
	template<> struct Map<Position2D>
	{
		using SysType = DirectX::XMFLOAT2;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
		static constexpr const char* semantic = "Position";
		static constexpr const char* code = "P2";
	};
	template<> struct Map<Position3D>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr const char* semantic = "Position";
		static constexpr const char* code = "P3";
	};
	template<> struct Map<Texture2D>
	{
		using SysType = DirectX::XMFLOAT2;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
		static constexpr const char* semantic = "Texcoord";
		static constexpr const char* code = "T2";
	};
	template<> struct Map<Normal>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr const char* semantic = "Normal";
		static constexpr const char* code = "N";
	};
	template<> struct Map<Tangent>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr const char* semantic = "Tangent";
		static constexpr const char* code = "Nt";
	};
	template<> struct Map<Bitangent>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr const char* semantic = "Bitangent";
		static constexpr const char* code = "Nb";
	};
	template<> struct Map<Float3Color>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr const char* semantic = "Color";
		static constexpr const char* code = "C3";
	};
	template<> struct Map<Float4Color>
	{
		using SysType = DirectX::XMFLOAT4;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
		static constexpr const char* semantic = "Color";
		static constexpr const char* code = "C4";
	};
	template<> struct Map<BGRAColor>
	{
		using SysType = ::BGRAColor;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		static constexpr const char* semantic = "Color";
		static constexpr const char* code = "C8";
	};
public:
	class Element
	{
	public:
		Element(ElementType type, size_t offset)
			: type(type),
			offset(offset)
		{}
	public:
		size_t GetOffsetAfter()const noxnd
		{
			return offset + Size();
		}
		size_t GetOffset()const noexcept
		{
			return offset;
		}
		size_t Size()const noxnd
		{
			return SizeOf(type);
		}
		static constexpr size_t SizeOf(ElementType type)noxnd
		{
			using namespace DirectX;
			switch (type)
			{
			case VertexLayout::ElementType::Position2D:
				return sizeof(XMFLOAT2);
			case VertexLayout::ElementType::Position3D:
				return sizeof(XMFLOAT3);
			case VertexLayout::ElementType::Texture2D:
				return sizeof(XMFLOAT2);
			case VertexLayout::ElementType::Normal:
				return sizeof(XMFLOAT3);
			case VertexLayout::ElementType::Float3Color:
				return sizeof(XMFLOAT3);
			case VertexLayout::ElementType::Float4Color:
				return sizeof(XMFLOAT4);
			case VertexLayout::ElementType::BGRAColor:
				return sizeof(unsigned int);
			default:
				assert("Invalid Element Type" && false);
				return 0u;
			}
		}
		ElementType GetType()const noexcept
		{
			return type;
		}
		D3D11_INPUT_ELEMENT_DESC VertexLayout::Element::GetDesc() const noxnd
		{
			switch (type)
			{
			case Position2D:
				return GenerateDesc<Position2D>(GetOffset());
			case Position3D:
				return GenerateDesc<Position3D>(GetOffset());
			case Texture2D:
				return GenerateDesc<Texture2D>(GetOffset());
			case Normal:
				return GenerateDesc<Normal>(GetOffset());
			case Tangent:
				return GenerateDesc<Tangent>(GetOffset());
			case Bitangent:
				return GenerateDesc<Bitangent>(GetOffset());
			case Float3Color:
				return GenerateDesc<Float3Color>(GetOffset());
			case Float4Color:
				return GenerateDesc<Float4Color>(GetOffset());
			case BGRAColor:
				return GenerateDesc<BGRAColor>(GetOffset());
			}
			assert("Invalid element type" && false);
			return{ "INVALID",0,DXGI_FORMAT_UNKNOWN,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 };
		}
		const char* GetCode() const noexcept
		{
			switch (type)
			{
			case Position2D:
				return Map<Position2D>::code;
			case Position3D:
				return Map<Position3D>::code;
			case Texture2D:
				return Map<Texture2D>::code;
			case Normal:
				return Map<Normal>::code;
			case Tangent:
				return Map<Tangent>::code;
			case Bitangent:
				return Map<Bitangent>::code;
			case Float3Color:
				return Map<Float3Color>::code;
			case Float4Color:
				return Map<Float4Color>::code;
			case BGRAColor:
				return Map<BGRAColor>::code;
			}
			assert("Invalid element type" && false);
			return "Invalid";
		}
	private:
		template<ElementType type>
		static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc(size_t offset) noexcept
		{
			return{ Map<type>::semantic,0,Map<type>::dxgiFormat,0,(UINT)offset,D3D11_INPUT_PER_VERTEX_DATA,0 };
		}
	private:
		ElementType type;
		size_t offset;
	};
public:
	template <ElementType Type>
	const Element& Resolve()const noxnd
	{
		for (auto& e : elements)
		{
			if (e.GetType() == Type)
			{
				return e;
			}
		}
#if _DEBUG
		assert(fmt::sprintf("Couldn't resolve type name %s", Typenames[Type]).c_str() && false);
#endif
		return elements.front();
	}
	const Element& ResolveByIndex(size_t index)const noxnd
	{
#if _DEBUG
		return elements.at(index);
#endif
		return elements[index];
	}

	template <ElementType Type>
	VertexLayout& Append()noxnd
	{
		elements.emplace_back(Type, Size());
		return *this;
	}
	size_t Size()const noxnd
	{
		return elements.empty() ? 0u : elements.back().GetOffsetAfter();
	}
	size_t GetElementCount() const noexcept
	{
		return elements.size();
	}
	std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const noxnd
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
		desc.reserve(GetElementCount());
		for (const auto& e : elements)
		{
			desc.push_back(e.GetDesc());
		}
		return desc;
	}
	std::string GetCode() const noxnd
	{
		std::string code;
		for (const auto& e : elements)
		{
			code += e.GetCode();
		}
		return code;
	}
private:
	std::vector<Element> elements;
};

class Vertex
{
	friend class VertexBuffer;
public:
	template<VertexLayout::ElementType Type>
	auto& Attr() noxnd
	{
		auto pAttribute = pData + layout.Resolve<Type>().GetOffset();
		return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType*>(pAttribute);
	}
	template<typename T>
	void SetAttributeByIndex(size_t i, T&& val) noxnd
	{
		const auto& element = layout.ResolveByIndex(i);
		auto pAttribute = pData + element.GetOffset();
		switch (element.GetType())
		{
		case VertexLayout::Position2D:
			SetAttribute<VertexLayout::Position2D>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Position3D:
			SetAttribute<VertexLayout::Position3D>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Texture2D:
			SetAttribute<VertexLayout::Texture2D>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Normal:
			SetAttribute<VertexLayout::Normal>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Tangent:
			SetAttribute<VertexLayout::Tangent>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Bitangent:
			SetAttribute<VertexLayout::Bitangent>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Float3Color:
			SetAttribute<VertexLayout::Float3Color>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Float4Color:
			SetAttribute<VertexLayout::Float4Color>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::BGRAColor:
			SetAttribute<VertexLayout::BGRAColor>(pAttribute, std::forward<T>(val));
			break;
		default:
			assert("Bad element type" && false);
		}
	}
protected:
	Vertex(unsigned char* pData, const VertexLayout& layout) noxnd
		: pData(pData), layout(layout)
	{

	}
private:
	template<typename First, typename ...Rest>
	void SetAttributeByIndex(size_t i, First&& first, Rest&&... rest) noxnd
	{
		SetAttributeByIndex(i, std::forward<First>(first));
		SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...);
	}
	template<VertexLayout::ElementType DestLayoutType, typename SrcType>
	void SetAttribute(unsigned char* pAttribute, SrcType&& val) noxnd
	{
		using Dest = typename VertexLayout::Map<DestLayoutType>::SysType;
		if constexpr (std::is_assignable<Dest, SrcType>::value)
		{
			*reinterpret_cast<Dest*>(pAttribute) = val;
		}
		else
		{
			assert("Parameter attribute type mismatch" && false);
		}
	}
private:
	unsigned char* pData = nullptr;
	const VertexLayout& layout;
};

class ConstVertex
{
public:
	ConstVertex(const Vertex& v) noxnd;
	template<VertexLayout::ElementType Type>
	const auto& Attr() const noxnd
	{
		return const_cast<Vertex&>(vertex).Attr<Type>();
	}
private:
	Vertex vertex;
};

class VertexBuffer
{
public:
	VertexBuffer(VertexLayout layout)noxnd
		:layout(std::move(layout))
	{}
public:
	const VertexLayout& GetLayout() const noexcept
	{
		return layout;
	}
	size_t Count()const noxnd
	{
		return buffer.size() / layout.Size();
	}
	size_t Size() const noxnd
	{
		return buffer.size();
	}
	template<typename ...Params>
	void EmplaceBack(Params&&... params) noxnd
	{
		assert(sizeof...(params) == layout.GetElementCount() && "Param count doesn't match number of vertex elements");
		buffer.resize(buffer.size() + layout.Size());
		Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
	}
	Vertex Back() noxnd
	{
		assert(!buffer.empty());
		return Vertex{buffer.data()+buffer.size() - layout.Size(), layout};
	}
	Vertex Front() noxnd
	{
		assert(buffer.empty());
		return Vertex{ buffer.data(), layout };
	}
	Vertex operator[](size_t i)noxnd
	{
		assert(i < Count());
		return Vertex{ buffer.data() + layout.Size()*i,layout };
	}

	ConstVertex Back() const noxnd
	{
		return const_cast<VertexBuffer*>(this)->Back();
	}
	ConstVertex Front() const noxnd
	{
		return const_cast<VertexBuffer*>(this)->Front();
	}
	ConstVertex operator[](size_t i) const noxnd
	{
		return const_cast<VertexBuffer&>(*this)[i];
	}
private:
	VertexLayout layout;
	std::vector<unsigned char> buffer;
};