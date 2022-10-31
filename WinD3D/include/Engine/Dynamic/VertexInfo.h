#pragma once
#include <DirectXMath.h>
#include <dxgiformat.h>
#include <d3d11_4.h>


#define DVTX_ELEMENT_AI_EXTRACTOR(member) //static SysType Extract( const aiMesh& mesh, size_t i ) noexcept {return *reinterpret_cast<const SysType*>(&mesh.member[i]);}

#define LAYOUT_ELEMENT_TYPES \
	X( Position2D ) \
	X( Position3D ) \
	X( Texture2D ) \
	X( Normal ) \
	X( Tangent ) \
	X( Bitangent ) \
	X( Float3Color ) \
	X( Float4Color ) \
	X( BGRAColor )

namespace ver::dv
{
	struct BGRAColor
	{
		unsigned char a;
		unsigned char r;
		unsigned char g;
		unsigned char b;
	};

	enum class ElementType : size_t
	{
#define X(el) el,
		LAYOUT_ELEMENT_TYPES
		Count
#undef X
	};

	class ElementInfo
	{
	public:
#if _DEBUG
		static constexpr std::array<std::wstring_view, size_t(ElementType::Count)> Typenames{
		#define X(el) L#el,
						LAYOUT_ELEMENT_TYPES
		#undef X
		};
#endif
	public:
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
			using SysType = BGRAColor;
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
	};

	template<template<ElementType> class F, typename... Args>
	inline constexpr auto Bridge(ElementType type, Args&&... args) noexcept
	{
		switch (type)
		{
#define X(el) case ElementType::el: return F<ElementType::el>::Exec( std::forward<Args>( args )... );
			LAYOUT_ELEMENT_TYPES
#undef X
		}
		assert("Invalid element type" && false);
		return F<ElementType::Count>::Exec(std::forward<Args>(args)...);
	}




	class Element
	{
		template<ElementType type>
		struct SysSizeLookup {
			static constexpr auto Exec() noexcept {
				return sizeof(ElementInfo::Map<type>::SysType);
			}
		};
		template<ElementType type>
		struct DescGenerate {
			static constexpr D3D11_INPUT_ELEMENT_DESC Exec(size_t offset) noexcept {
				return {
					ElementInfo::Map<type>::semantic,0,
					ElementInfo::Map<type>::dxgiFormat,
					0,(UINT)offset,D3D11_INPUT_PER_VERTEX_DATA,0
				};
			}
		};
		template<ElementType type>
		struct CodeLookup
		{
			static constexpr auto Exec() noexcept
			{
				return ElementInfo::Map<type>::code;
			}
		};
	public:
		constexpr Element(ElementType type, size_t offset) : _type(type), _offset(offset) {}
	public:
		constexpr bool operator==(const Element& other)const { return other._type == _type; }
	public:
		constexpr size_t offset_after()const noexcept { return _offset + size(); }
		constexpr size_t offset()const noexcept { return _offset; }
		constexpr size_t size()const noexcept { return size_of(_type); }
		constexpr ElementType type()const noexcept { return _type; }
	public:
		static constexpr size_t size_of(ElementType type)noexcept { return Bridge<SysSizeLookup>(type); }
	public:
		constexpr D3D11_INPUT_ELEMENT_DESC descriptor() const noexcept { return Bridge<DescGenerate>(_type, _offset); }
		constexpr const char* code() const noexcept { return Bridge<CodeLookup>(_type); }
	private:
		ElementType _type;
		size_t _offset;
	};

}

#undef DVTX_ELEMENT_AI_EXTRACTOR
#ifndef VERTEX_IMPL
#undef LAYOUT_ELEMENT_TYPES
#endif