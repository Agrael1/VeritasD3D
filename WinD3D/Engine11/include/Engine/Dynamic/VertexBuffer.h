#pragma once
#include <Engine/Dynamic/VertexLayout.h>

namespace ver::dv
{
	class VertexBuffer;

	class Vertex
	{
		friend class ::ver::dv::VertexBuffer;
	private:
		// necessary for Bridge to SetAttribute
		template<ElementType type>
		struct AttributeSetting
		{
			template<typename T>
			static constexpr auto Exec(Vertex* pVertex, std::byte* pAttribute, T&& val) noexcept
			{
				return pVertex->SetAttribute<type>(pAttribute, std::forward<T>(val));
			}
		};
	public:
		Vertex& operator=(const Vertex& prev)
		{
			assert(layout == prev.layout);
			std::copy(prev.pData, prev.pData + layout.layout_size(), pData);
			return *this;
		}

		template<ElementType Type>
		decltype(auto) attribute()const noexcept
		{
			auto pAttribute = pData + layout.Resolve<Type>().offset();
			return *reinterpret_cast<typename ElementInfo::Map<Type>::SysType*>(pAttribute);
		}
		template<ElementType Type>
		void set(typename ElementInfo::Map<Type>::SysType&& val)
		{
			auto pAttribute = pData + layout.Resolve<Type>().offset();
			SetAttribute<Type>(pAttribute, std::forward<typename ElementInfo::Map<Type>::SysType>(val));
		}
		template<typename T>
		void set_at(size_t i, T&& val) noexcept
		{
			const auto& element = layout[i];
			auto pAttribute = pData + element.offset();
			Bridge<AttributeSetting>(
				element.type(), this, pAttribute, std::forward<T>(val)
				);
		}
	public:
		Vertex(std::byte* pData, LayoutSpan layout) noexcept
			: pData(pData), layout(layout)
		{}
	private:
		template<typename First, typename ...Rest>
		void SetAttributeByIndex(size_t i, First&& first, Rest&&... rest) noexcept
		{
			set_at(i, std::forward<First>(first));
			if constexpr (sizeof...(rest) > 0)
				SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...);
		}
		template<ElementType DestLayoutType, typename SrcType>
		void SetAttribute(std::byte* pAttribute, SrcType&& val) noexcept
		{
			using Dest = typename ElementInfo::Map<DestLayoutType>::SysType;
			if constexpr(std::is_assignable<Dest, SrcType>::value/*std::assignable_from<Dest, SrcType>*/)
				*reinterpret_cast<Dest*>(pAttribute) = val;
		}
	private:
		std::byte* pData = nullptr;
		LayoutSpan layout;
	};


	class VertexSpan
	{
	public:
		VertexSpan(LayoutSpan layout, std::span<std::byte> buffer) noexcept
			:_layout(layout), buffer(buffer) {}
	public:
		LayoutSpan layout() const noexcept { return _layout; }
		size_t count()const noexcept { return buffer.size() / _layout.layout_size(); }
		size_t size() const noexcept { return buffer.size(); }
		std::span<const std::byte> data()const noexcept { return buffer; }

		Vertex back() noexcept { return Vertex{ buffer.data() + buffer.size() - _layout.layout_size(), layout() }; }
		Vertex front() noexcept { return Vertex{ buffer.data(), layout() }; }
		Vertex operator[](size_t i)noexcept { return Vertex{ buffer.data() + _layout.layout_size() * i,layout() }; }
	private:
		LayoutSpan _layout;
		std::span<std::byte> buffer;
	};


	class VertexBuffer
	{
	public:
		VertexBuffer(VertexLayout layout, size_t size = 0) noexcept
			:_layout(std::move(layout))
		{
			reserve(size);
		}
		//VertexBuffer(VertexLayout layout, const aiMesh& mesh);
	public:
		operator VertexSpan()
		{
			return { _layout, buffer };
		}
		LayoutSpan layout() const noexcept { return _layout; }
		size_t count()const noexcept { return buffer.size() / _layout.layout_size(); }
		size_t size() const noexcept { return buffer.size(); }
		std::span<const std::byte> data()const noexcept { return buffer; }

		template<typename ...Params>
		void emplace_back(Params&&... params) noexcept
		{
			assert(sizeof...(params) == _layout.count() && "Param count doesn't match number of vertex elements");
			buffer.resize(buffer.size() + _layout.layout_size());
			auto x = back();
			x.SetAttributeByIndex(0u, std::forward<Params>(params)...);
		}

		void reserve(size_t size) { buffer.reserve(buffer.size() + _layout.layout_size() * size); }

		Vertex back() noexcept { return Vertex{ buffer.data() + buffer.size() - _layout.layout_size(), layout() }; }
		Vertex front() noexcept { return Vertex{ buffer.data(), layout() }; }
		Vertex operator[](size_t i)noexcept { return Vertex{ buffer.data() + _layout.layout_size() * i,layout() }; }
	private:
		VertexLayout _layout;
		std::vector<std::byte> buffer;
	};
}