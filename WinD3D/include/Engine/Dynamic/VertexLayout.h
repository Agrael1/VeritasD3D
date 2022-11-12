#pragma once
#include <Engine/Dynamic/VertexInfo.h>
#include <ranges>
#include <span>
#include <numeric>

namespace ver::dv
{
	class LayoutSpan
	{
	public:
		constexpr LayoutSpan(std::span<const Element> e) :elements(e) { }
	public:
		template <ElementType Type>
		constexpr const Element& Resolve()const noexcept
		{
			auto x = std::ranges::find_if(elements, [](const Element& e) {return e.type() == Type; });
			if (x != elements.end())
				return *x;

#if _DEBUG
			auto error = std::format(L"Couldn't resolve type name {}", ElementInfo::Typenames[size_t(Type)]);
			_wassert(error.c_str(), __FILEW__, __LINE__);
#endif
			return elements.front();
		}
		constexpr const Element& operator[](size_t index)const noexcept
		{
			return elements[index];
		}

		constexpr bool contains(ElementType type) const noexcept
		{
			return std::ranges::find_if(elements, [type](const Element& e) {return e.type() == type; }) != elements.end();
		}

		friend constexpr bool operator==(const LayoutSpan& lhs, const LayoutSpan& rhs)
		{
			return std::equal(lhs.elements.begin(), lhs.elements.end(), rhs.elements.begin());
		}

		constexpr size_t layout_size()const noexcept {
			return elements.empty() ? 0u : elements.back().offset_after();
		}
		constexpr size_t count() const noexcept
		{
			return elements.size();
		}
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const noexcept
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
			desc.reserve(count());
			for (const auto& e : elements)
			{
				desc.push_back(e.descriptor());
			}
			return desc;
		}
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DMultilayout() const noexcept
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
			desc.reserve(count());
			uint32_t slot = 0;
			for (const auto& e : elements)
			{
				auto d = e.descriptor();
				d.InputSlot = slot++;
				d.AlignedByteOffset = 0;
				desc.push_back(d);
			}
			return desc;
		}
		constexpr std::span<const Element> span()const noexcept
		{
			return elements;
		}
		constexpr std::string code()const noexcept
		{
			std::string a;
			for (auto& i : elements)
				a += i.code();
			return a;
		}
	private:
		std::span<const Element> elements;
	};


	class VertexLayout
	{
	public:
		constexpr VertexLayout() { elements.reserve(size_t(ElementType::Count)); }
		constexpr VertexLayout(ElementType ty) { elements.reserve(size_t(ElementType::Count)); append(ty); }

	public:
		constexpr operator LayoutSpan()const
		{
			return { elements };
		}

		template <ElementType Type>
		constexpr const Element& Resolve()const noexcept
		{
			auto x = std::ranges::find_if(elements, [](const Element& e) {return e.type() == Type; });
			if (x != elements.end())
				return *x;

			if constexpr (std::is_constant_evaluated())
			{
				static_assert(x == elements.end(), "Couldn't resolve type name");
			}
			else
			{
#if _DEBUG
				auto error = std::format(L"Couldn't resolve type name {}", ElementInfo::Typenames[size_t(Type)]);
				_wassert(error.c_str(), __FILEW__, __LINE__);
#endif
			}

			return elements.front();
		}
		constexpr const Element& ResolveByIndex(size_t index)const noexcept
		{
#if _DEBUG
			return elements.at(index);
#endif
			return elements[index];
		}

		constexpr VertexLayout& append(ElementType Type) noexcept
		{
			if (!contains(Type))elements.emplace_back(Type, layout_size());
			return *this;
		}

		constexpr bool contains(ElementType type) const noexcept
		{
			return std::ranges::find_if(elements, [type](const Element& e) {return e.type() == type; }) != elements.end();
		}

		friend constexpr bool operator==(const VertexLayout& lhs, const VertexLayout& rhs)
		{
			return lhs.elements == rhs.elements;
		}

		constexpr std::span<const Element> span()const noexcept
		{
			return elements;
		}
		constexpr size_t layout_size()const noexcept {
			return elements.empty() ? 0u : elements.back().offset_after();
		}
		constexpr size_t count() const noexcept
		{
			return elements.size();
		}
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const noexcept
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
			desc.reserve(count());
			for (const auto& e : elements)
			{
				desc.push_back(e.descriptor());
			}
			return desc;
		}
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DMultilayout() const noexcept
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
			desc.reserve(count());
			uint32_t slot = 0;
			for (const auto& e : elements)
			{
				auto d = e.descriptor();
				d.InputSlot = slot++;
				d.AlignedByteOffset = 0;
				desc.push_back(d);
			}
			return desc;
		}
		constexpr std::string code()const noexcept
		{
			std::string a;
			for (auto& i : elements)
				a += i.code();
			return a;
		}
	private:
		std::vector<Element> elements;
	};
}
