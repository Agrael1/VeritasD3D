#pragma once
#include <cassert>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <optional>
#include <string>

// Shameless hack on switches to dynamically set allowed types
#define LEAF_ELEMENT_TYPES \
	X(Float)\
	X(Float2)\
	X(Float3)\
	X(Float4)\
	X(Matrix)\
	X(Bool)\
	X(Integer)

namespace DC
{
	namespace dx = DirectX;

	enum class Type
	{
		Empty,
		#define X(el) el,
		LEAF_ELEMENT_TYPES
		#undef X
		Struct,
		Array,
	};

	template<Type type>
	struct Map
	{
		static constexpr bool valid = false;
	};
	template<> struct Map< Type::Float >
	{
		using SysType = float;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr bool valid = true;
		static constexpr const char* code = "F1";
	};
	template<> struct Map< Type::Float2 >
	{
		using SysType = dx::XMFLOAT2;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr bool valid = true;
		static constexpr const char* code = "F2";
	};
	template<> struct Map< Type::Float3 >
	{
		using SysType = dx::XMFLOAT3;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr bool valid = true;
		static constexpr const char* code = "F3";
	};
	template<> struct Map< Type::Float4 >
	{
		using SysType = dx::XMFLOAT4;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr bool valid = true;
		static constexpr const char* code = "F4";
	};
	template<> struct Map< Type::Matrix >
	{
		using SysType = dx::XMFLOAT4X4;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr bool valid = true;
		static constexpr const char* code = "F4x4";
	};
	template<> struct Map< Type::Bool >
	{
		using SysType = bool;
		static constexpr size_t hlslSize = 4u;
		static constexpr bool valid = true;
		static constexpr const char* code = "B";
	};
	template<> struct Map< Type::Integer >
	{
		using SysType = int;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr const char* code = "IN";
		static constexpr bool valid = true;
	};

	#define X(el) static_assert(Map<Type::el>::valid, "Missing map implementation for " #el);
	LEAF_ELEMENT_TYPES
	#undef X

	template<typename T>
	struct ReverseMap
	{
		static constexpr bool valid = false;
	};
	#define X(el)\
	template<> struct ReverseMap<typename Map<Type::el>::SysType>\
	{\
		static constexpr bool valid = true;\
		static constexpr Type type = Type::el;\
	};
	LEAF_ELEMENT_TYPES
	#undef X

	class LayoutElement
	{
		struct ExtraDataBase
		{
			virtual ~ExtraDataBase() = default;
		};
		struct LayoutPair
		{
			LayoutPair(Type type, std::string name, size_t count = 1) 
				:type(type), name(std::move(name)), count(count){};
		public:
			Type type;
			size_t count;
			std::string name;
		};

		friend class RawLayout;
		friend struct ExtraData;
	public:
		LayoutElement() noexcept = default;
		LayoutElement(Type typeIn) noxnd;
	public:
		std::pair<size_t, const LayoutElement*> CalculateIndexingOffset(size_t offset, size_t index) const noxnd;
		std::string GetSignature() const noxnd;
		bool Exists() const noexcept;

		template<typename T>
		size_t Resolve() const noxnd
		{
			switch (type)
			{
			#define X(el) case Type::el: assert(typeid(Map<Type::el>::SysType) == typeid(T)); return *offset;
			LEAF_ELEMENT_TYPES
			#undef X
			default:
				assert("Tried to resolve non-leaf element" && false);
				return 0u;
			}
		}
	public: //struct accessors
		LayoutElement& operator[](std::string_view key)noxnd;
		const LayoutElement& operator[](std::string_view key) const noxnd;
	public: //array accessors
		LayoutElement& T() noxnd;
		const LayoutElement& T() const noxnd;
	public:
		size_t GetOffsetBegin()const noxnd;
		size_t GetOffsetEnd() const noxnd;
		size_t GetSizeInBytes() const noxnd;
	public:
		LayoutElement& Add(Type addedType, std::string name) noxnd;
		LayoutElement& Set(Type addedType, size_t size) noxnd;

		LayoutElement& Add(std::initializer_list<LayoutPair> pairs) noxnd
		{
			assert("Add to non-struct in layout" && type == Type::Struct);
			for (auto& pair : pairs)
			{
				if (pair.count>1)
				{
					Add(Type::Array, std::move(pair.name)).
					Set(pair.type, pair.count);
					continue;
				}
				Add(pair.type, std::move(pair.name));
			}
			return *this;
		}
	private:
		static bool CrossesBoundary(size_t offset, size_t size) noexcept;
		static size_t AdvanceIfCrossesBoundary(size_t offset, size_t size) noexcept;
		static size_t AdvanceToBoundary(size_t offset) noexcept;
		static LayoutElement& GetEmptyElement()noexcept;

		std::string GetSignatureForStruct() const noxnd;
		std::string GetSignatureForArray() const noxnd;
		bool ValidateSymbolName(const std::string& name) noexcept;
	private:
		size_t Finalize(size_t offsetIn) noxnd;
		size_t FinalizeForStruct(size_t offsetIn);
		size_t FinalizeForArray(size_t offsetIn);
	private:
		std::optional<size_t> offset;
		Type type = Type::Empty;
		std::unique_ptr<ExtraDataBase> pExtraData;
	};

	class Layout
	{
		friend class LayoutCodex;
		friend class Buffer;
	public:
		size_t GetSizeInBytes() const noexcept;
		std::string GetSignature() const noxnd;
	protected:
		Layout(std::shared_ptr<LayoutElement> pRoot) noexcept;
	protected:
		std::shared_ptr<LayoutElement> pRoot;
	};



	// Raw layout represents a layout that has not yet been finalized and registered
	// structure can be edited by adding layout nodes
	class RawLayout : public Layout
	{
		friend class LayoutCodex;
	public:
		RawLayout() noexcept;
	public:
		LayoutElement& operator[](std::string_view key) noxnd;		// key into the root Struct
		LayoutElement& Add(Type addedType, std::string name) noxnd
		{
			pRoot->Add(addedType, std::move(name));
			return *pRoot;
		}
		LayoutElement& Add(std::initializer_list<LayoutElement::LayoutPair> pairs) noxnd
		{
			return pRoot->Add(std::move(pairs));
		}
	private:
		void ClearRoot() noexcept; // reset this object with an empty struct at its root
		std::shared_ptr<LayoutElement> DeliverRoot() noexcept;// finalize the layout and then relinquish (by yielding the root layout element)
	};


	// CookedLayout represend a completed and registered Layout shell object
	// layout tree is fixed
	class CookedLayout : public Layout
	{
		friend class LayoutCodex;
		friend class Buffer;
	public:
		const LayoutElement& operator[](std::string_view key) const noxnd; // key into the root Struct (const to disable mutation of the layout)
		std::shared_ptr<LayoutElement> ShareRoot() const noexcept;// get a share on layout tree root
	private:
		CookedLayout(std::shared_ptr<LayoutElement> pRoot) noexcept; // this ctor used by Codex to return cooked layouts
		std::shared_ptr<LayoutElement> RelinquishRoot() const noexcept; // use to pilfer the layout tree
	};




	// proxy type that is emitted when keying/indexing into a Buffer
	// implement conversions/assignment that allows manipulation of the
	// raw bytes of the Buffer. This version is const, only supports reading
	// Refs can be further keyed/indexed to traverse the layout structure
	class ConstElementRef
	{
		friend class Buffer;
		friend class ElementRef;
	public:
		// this is a proxy type emitted when you use addressof& on the Ref
		// it allows conversion to pointer type, useful for using Buffer
		// elements with ImGui widget functions etc.
		class Ptr
		{
			friend ConstElementRef;
		public:
			// conversion for getting read-only pointer to supported SysType
			template<typename T>
			operator const T* () const noxnd
			{
				static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in pointer conversion");
				return &static_cast<const T&>(*ref);
			}
		private:
			Ptr(const ConstElementRef* ref) noexcept;
			const ConstElementRef* ref;
		};
	public:
		// check if the indexed element actually exists
		// this is possible because if you key into a Struct with a nonexistent key
		// it will still return an Empty LayoutElement that will enable this test
		// but will not enable any other kind of access
		bool Exists() const noexcept;
		// key into the current element as a struct
		ConstElementRef operator[](const std::string& key) const noxnd;
		// index into the current element as an array
		ConstElementRef operator[](size_t index) const noxnd;
		// emit a pointer proxy object
		Ptr operator&() const noxnd;
		// conversion for reading as a supported SysType
		template<typename T>
		operator const T& () const noxnd
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in conversion");
			return *reinterpret_cast<const T*>(pBytes + offset + pLayout->Resolve<T>());
		}
	private:
		// refs should only be constructable by other refs or by the buffer
		ConstElementRef(const LayoutElement* pLayout, const char* pBytes, size_t offset) noexcept;
	private:
		// this offset is the offset that is built up by indexing into arrays
		// accumulated for every array index in the path of access into the structure
		size_t offset;
		const LayoutElement* pLayout;
		const char* pBytes;
	};


	// version of ConstElementRef that also allows writing to the bytes of Buffer
	// see above in ConstElementRef for detailed description
	class ElementRef
	{
		friend class Buffer;
	public:
		class Ptr
		{
			friend ElementRef;
		public:
			// conversion to read/write pointer to supported SysType
			template<typename T>
			operator T* () const noxnd
			{
				static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in pointer conversion");
				return &static_cast<T&>(*ref);
			}
		private:
			Ptr(ElementRef* ref) noexcept;
			ElementRef* ref;
		};
	public:
		operator ConstElementRef() const noexcept;
		bool Exists() const noexcept;
		ElementRef operator[](const std::string& key) const noxnd;
		ElementRef operator[](size_t index) const noxnd;
		// optionally set value if not an empty Ref
		template<typename S>
		bool SetIfExists(const S& val) noxnd
		{
			if (Exists())
			{
				*this = val;
				return true;
			}
			return false;
		}
		Ptr operator&() const noxnd;
		// conversion for reading/writing as a supported SysType
		template<typename T>
		operator T& () const noxnd
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in conversion");
			return *reinterpret_cast<T*>(pBytes + offset + pLayout->Resolve<T>());
		}
		// assignment for writing to as a supported SysType
		template<typename T>
		T& operator=(const T& rhs) const noxnd
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in assignment");
			return static_cast<T&>(*this) = rhs;
		}
	private:
		// refs should only be constructable by other refs or by the buffer
		ElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset) noexcept;
		size_t offset;
		const LayoutElement* pLayout;
		char* pBytes;
	};


	class Buffer
	{
	public:
		Buffer(RawLayout&& lay) noxnd;
		Buffer(const CookedLayout& lay) noxnd;
		Buffer(CookedLayout&& lay) noxnd;
		Buffer(const Buffer&) noexcept;
		// have to be careful with this one...
		// the buffer that has once been pilfered must not be used :x
		Buffer(Buffer&&) noexcept;
	public:
		ElementRef operator[](std::string_view key) noxnd;
		ConstElementRef operator[](std::string_view key) const noxnd;
	public:
		// get the raw bytes
		const char* GetData() const noexcept;
		// size of the raw byte buffer
		size_t GetSizeInBytes() const noexcept;
		const LayoutElement& GetRootLayoutElement() const noexcept;
		// copy bytes from another buffer (layouts must match)
		void CopyFrom(const Buffer&) noxnd;
		// return another sptr to the layout root
		std::shared_ptr<LayoutElement> ShareLayoutRoot() const noexcept;
	private:
		std::shared_ptr<LayoutElement> pLayoutRoot;
		std::vector<char> bytes;
	};
}
#ifndef DCB_IMPL_SOURCE
#undef LEAF_ELEMENT_TYPES
#endif