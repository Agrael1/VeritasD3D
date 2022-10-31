#pragma once
#include <vector>
#include <array>
#include <DirectXMath.h>
#include <format>
#include <assimp/scene.h>
#include <d3d11_4.h>


#define DVTX_ELEMENT_AI_EXTRACTOR(member) static SysType Extract( const aiMesh& mesh, size_t i ) noexcept {return *reinterpret_cast<const SysType*>(&mesh.member[i]);}

#define LAYOUT_ELEMENT_TYPES \
	X( Position2D ) \
	X( Position3D ) \
	X( Texture2D ) \
	X( Normal ) \
	X( Tangent ) \
	X( Bitangent ) \
	X( Float3Color ) \
	X( Float4Color ) \
	X( BGRAColor ) \
	X( Count )

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
			#define X(el) el,
			LAYOUT_ELEMENT_TYPES
			#undef X
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
			DVTX_ELEMENT_AI_EXTRACTOR(mVertices)
		};
		template<> struct Map<ElementType::Position3D>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P3";
			DVTX_ELEMENT_AI_EXTRACTOR(mVertices)
		};
		template<> struct Map<ElementType::Texture2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Texcoord";
			static constexpr const char* code = "T2";
			DVTX_ELEMENT_AI_EXTRACTOR(mTextureCoords[0])
		};
		template<> struct Map<ElementType::Normal>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Normal";
			static constexpr const char* code = "N";
			DVTX_ELEMENT_AI_EXTRACTOR(mNormals)
		};
		template<> struct Map<ElementType::Tangent>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Tangent";
			static constexpr const char* code = "Nt";
			DVTX_ELEMENT_AI_EXTRACTOR(mTangents)
		};
		template<> struct Map<ElementType::Bitangent>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Bitangent";
			static constexpr const char* code = "Nb";
			DVTX_ELEMENT_AI_EXTRACTOR(mBitangents)
		};
		template<> struct Map<ElementType::Float3Color>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C3";
			DVTX_ELEMENT_AI_EXTRACTOR(mColors[0])
		};
		template<> struct Map<ElementType::Float4Color>
		{
			using SysType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C4";
			DVTX_ELEMENT_AI_EXTRACTOR(mColors[0])
		};
		template<> struct Map<ElementType::BGRAColor>
		{
			using SysType = DV::BGRAColor;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C8";
			DVTX_ELEMENT_AI_EXTRACTOR(mColors[0])
		};
		template<> struct Map<ElementType::Count>
		{
			using SysType = long double;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;
			static constexpr const char* semantic = "!INVALID!";
			static constexpr const char* code = "!INV!";
			DVTX_ELEMENT_AI_EXTRACTOR(mFaces)
		};

		template<template<VertexLayout::ElementType> class F, typename... Args>
		static constexpr auto Bridge(VertexLayout::ElementType type, Args&&... args) noxnd
		{
			switch (type)
			{
				#define X(el) case VertexLayout::ElementType::el: return F<VertexLayout::ElementType::el>::Exec( std::forward<Args>( args )... );
				LAYOUT_ELEMENT_TYPES
				#undef X
			}
			assert("Invalid element type" && false);
			return F<VertexLayout::ElementType::Count>::Exec(std::forward<Args>(args)...);
		}
	public:
		class Element
		{
		public:
			Element(ElementType type, size_t offset);
		public:
			bool operator==(const Element& other)const //for equality check
			{
				return other.type == type;
			}
		public:
			size_t GetOffsetAfter()const noxnd;
			size_t GetOffset()const noexcept;
			size_t Size()const noxnd;
		public:
			static constexpr size_t SizeOf(ElementType type)noxnd;
		public:
			ElementType GetType()const noexcept;
			D3D11_INPUT_ELEMENT_DESC GetDesc() const noxnd;
			const char* GetCode() const noexcept;
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
			auto error = std::format(L"Couldn't resolve type name {}", Typenames[size_t(Type)]);
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


		bool Has(ElementType type) const noexcept;		
		VertexLayout& operator +(ElementType Type)noxnd
		{
			if (!Has(Type))
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
	private:
		// necessary for Bridge to SetAttribute
		template<VertexLayout::ElementType type>
		struct AttributeSetting
		{
			template<typename T>
			static constexpr auto Exec(Vertex* pVertex, unsigned char* pAttribute, T&& val) noxnd
			{
				return pVertex->SetAttribute<type>(pAttribute, std::forward<T>(val));
			}
		};
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
			VertexLayout::Bridge<AttributeSetting>(
				element.GetType(), this, pAttribute, std::forward<T>(val)
			);
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
	public:
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
		VertexBuffer(VertexLayout layout, size_t size = 0) noxnd;
		VertexBuffer(VertexLayout layout, const aiMesh& mesh);
	public:
		const VertexLayout& GetLayout() const noexcept;
		size_t Count()const noxnd;
		size_t Size() const noxnd;
		const unsigned char* data()const noxnd;

		template<typename ...Params>
		void EmplaceBack(Params&&... params) noxnd
		{
			assert(sizeof...(params) == layout.GetElementCount() && "Param count doesn't match number of vertex elements");
			buffer.resize(buffer.size() + layout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
		}
		void Reserve(size_t size);
		Vertex Back() noxnd;
		Vertex Front() noxnd;
		Vertex operator[](size_t i)noxnd;

		ConstVertex Back() const noxnd;
		ConstVertex Front() const noxnd;
		ConstVertex operator[](size_t i) const noxnd
		{
			return const_cast<VertexBuffer&>(*this)[i];
		}
	private:
		VertexLayout layout;
		std::vector<unsigned char> buffer;
	};

	typedef VertexLayout::ElementType Type;
}
#undef DVTX_ELEMENT_AI_EXTRACTOR
#ifndef DVTX_SOURCE_FILE
#undef LAYOUT_ELEMENT_TYPES
#endif