#pragma once
#include <vector>
#include <array>
#include <DirectXMath.h>
#include <Framework\noexcept_if.h>
#include <Framework\Utility.h>
#include <Fmtlib\include\fmt\printf.h>
#include <Engine\Graphics.h>


namespace DV
{
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
		enum class ElementType : size_t
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
		static std::array<std::wstring, size_t(ElementType::Count)> Typenames;
#endif
		template<ElementType> struct Map;
		template<> struct Map<ElementType::Position2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P2";
		};
		template<> struct Map<ElementType::Position3D>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P3";
		};
		template<> struct Map<ElementType::Texture2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Texcoord";
			static constexpr const char* code = "T2";
		};
		template<> struct Map<ElementType::Normal>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Normal";
			static constexpr const char* code = "N";
		};
		template<> struct Map<ElementType::Tangent>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Tangent";
			static constexpr const char* code = "Nt";
		};
		template<> struct Map<ElementType::Bitangent>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Bitangent";
			static constexpr const char* code = "Nb";
		};
		template<> struct Map<ElementType::Float3Color>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C3";
		};
		template<> struct Map<ElementType::Float4Color>
		{
			using SysType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C4";
		};
		template<> struct Map<ElementType::BGRAColor>
		{
			using SysType = DV::BGRAColor;
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
			bool operator==(const Element& other)const //for equality check
			{
				return other.type == type;
			}
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
				case VertexLayout::ElementType::Tangent:
					return sizeof(XMFLOAT3);
				case VertexLayout::ElementType::Bitangent:
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
				case ElementType::Position2D:
					return GenerateDesc<ElementType::Position2D>(GetOffset());
				case ElementType::Position3D:
					return GenerateDesc<ElementType::Position3D>(GetOffset());
				case ElementType::Texture2D:
					return GenerateDesc<ElementType::Texture2D>(GetOffset());
				case ElementType::Normal:
					return GenerateDesc<ElementType::Normal>(GetOffset());
				case ElementType::Tangent:
					return GenerateDesc<ElementType::Tangent>(GetOffset());
				case ElementType::Bitangent:
					return GenerateDesc<ElementType::Bitangent>(GetOffset());
				case ElementType::Float3Color:
					return GenerateDesc<ElementType::Float3Color>(GetOffset());
				case ElementType::Float4Color:
					return GenerateDesc<ElementType::Float4Color>(GetOffset());
				case ElementType::BGRAColor:
					return GenerateDesc<ElementType::BGRAColor>(GetOffset());
				}
				assert("Invalid element type" && false);
				return{ "INVALID",0,DXGI_FORMAT_UNKNOWN,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 };
			}
			const char* GetCode() const noexcept
			{
				switch (type)
				{
				case ElementType::Position2D:
					return Map<ElementType::Position2D>::code;
				case ElementType::Position3D:
					return Map<ElementType::Position3D>::code;
				case ElementType::Texture2D:
					return Map<ElementType::Texture2D>::code;
				case ElementType::Normal:
					return Map<ElementType::Normal>::code;
				case ElementType::Tangent:
					return Map<ElementType::Tangent>::code;
				case ElementType::Bitangent:
					return Map<ElementType::Bitangent>::code;
				case ElementType::Float3Color:
					return Map<ElementType::Float3Color>::code;
				case ElementType::Float4Color:
					return Map<ElementType::Float4Color>::code;
				case ElementType::BGRAColor:
					return Map<ElementType::BGRAColor>::code;
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
			auto error = fmt::sprintf(L"Couldn't resolve type name %s", Typenames[size_t(Type)]);
			_wassert(error.c_str(),__FILEW__,__LINE__);
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
		VertexLayout& operator +(ElementType Type)noxnd
		{
			elements.emplace_back(Type, Size());
			return *this;
		}
		friend bool operator==(const VertexLayout& lhs, const VertexLayout& rhs)
		{
			return lhs.elements == rhs.elements;
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
		Vertex& operator=(const Vertex& prev)
		{
			assert(layout == prev.layout);
			std::copy(prev.pData, prev.pData + layout.Size(), pData);
			return *this;
		}

		template<VertexLayout::ElementType Type>
		auto& Attr() noxnd
		{
			auto pAttribute = pData + layout.Resolve<Type>().GetOffset();
			return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType*>(pAttribute);
		}
		template<VertexLayout::ElementType Type>
		void Set(typename VertexLayout::Map<Type>::SysType&& val)
		{
			const auto& element = layout.Resolve<Type>();
			auto pAttribute = pData + element.GetOffset();

			SetAttribute<Type>(pAttribute, std::forward<typename VertexLayout::Map<Type>::SysType>(val));
		}
		template<typename T>
		void SetAttributeByIndex(size_t i, T&& val) noxnd
		{
			const auto& element = layout.ResolveByIndex(i);
			auto pAttribute = pData + element.GetOffset();
			switch (element.GetType())
			{
			case VertexLayout::ElementType::Position2D:
				SetAttribute<VertexLayout::ElementType::Position2D>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::ElementType::Position3D:
				SetAttribute<VertexLayout::ElementType::Position3D>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::ElementType::Texture2D:
				SetAttribute<VertexLayout::ElementType::Texture2D>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::ElementType::Normal:
				SetAttribute<VertexLayout::ElementType::Normal>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::ElementType::Tangent:
				SetAttribute<VertexLayout::ElementType::Tangent>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::ElementType::Bitangent:
				SetAttribute<VertexLayout::ElementType::Bitangent>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::ElementType::Float3Color:
				SetAttribute<VertexLayout::ElementType::Float3Color>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::ElementType::Float4Color:
				SetAttribute<VertexLayout::ElementType::Float4Color>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::ElementType::BGRAColor:
				SetAttribute<VertexLayout::ElementType::BGRAColor>(pAttribute, std::forward<T>(val));
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
			: layout(std::move(layout))
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
		const unsigned char* data()const noxnd
		{
			return buffer.data();
		}
		template<typename ...Params>
		void EmplaceBack(Params&&... params) noxnd
		{
			assert(sizeof...(params) == layout.GetElementCount() && "Param count doesn't match number of vertex elements");
			buffer.resize(buffer.size() + layout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
		}
		void Reserve(size_t size)
		{
			buffer.resize(buffer.size() + layout.Size() * size);
		}
		Vertex Back() noxnd
		{
			assert(!buffer.empty());
			return Vertex{ buffer.data() + buffer.size() - layout.Size(), layout };
		}
		Vertex Front() noxnd
		{
			assert(buffer.empty());
			return Vertex{ buffer.data(), layout };
		}
		Vertex operator[](size_t i)noxnd
		{
			assert(i < Count());
			return Vertex{ buffer.data() + layout.Size() * i,layout };
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

	typedef DV::VertexLayout::ElementType Type;
}