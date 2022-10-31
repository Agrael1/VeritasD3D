#pragma once
#include <filesystem>
#include <span>
#include <cassert>
#include <DirectXMath.h>
#include "ttf2mesh.h"

namespace ttf
{
	class mesh
	{
	public:
		mesh() = default;
		~mesh() { ttf_free_mesh(xmesh); }
		mesh(mesh&& other) noexcept : xmesh(other.xmesh) { other.xmesh = nullptr; }

	public:
		[[nodiscard]] ttf_mesh_t** put() noexcept
		{
			clear();
			return &xmesh;
		}
		void clear() noexcept
		{
			if (xmesh)
			{
				ttf_free_mesh(xmesh);
				xmesh = nullptr;
			}
		}
		std::span<float> get_vertices()
		{
			return { reinterpret_cast<float*>(xmesh->vert), size_t(xmesh->nvert) * 3 };
		}
		std::span<int> get_indices()
		{
			return { reinterpret_cast<int*>(xmesh->faces), size_t(xmesh->nfaces) * 3 };
		}
		const ttf_mesh_t& get()const
		{
			return *xmesh;
		}

	private:
		ttf_mesh_t* xmesh = nullptr;
	};

	class mesh3d
	{
	public:
		mesh3d(const ttf_glyph_t* xglyph)
			:glyph(xglyph)
		{}
		~mesh3d() { ttf_free_mesh3d(xmesh); }
		mesh3d(const mesh3d&) = delete;
		mesh3d(mesh3d&& other) noexcept : xmesh(other.xmesh), glyph(other.glyph) { other.xmesh = nullptr; other.glyph = nullptr; }

	public:
		[[nodiscard]] ttf_mesh3d_t** put() noexcept
		{
			clear();
			return &xmesh;
		}
		operator bool()const
		{
			return xmesh != nullptr;
		}
		void clear() noexcept
		{
			if (xmesh)
			{
				ttf_free_mesh3d(xmesh);
				xmesh = nullptr;
			}
		}
		std::span<const float> get_vertices()const
		{
			return { reinterpret_cast<const float*>(xmesh->vert), size_t(xmesh->nvert) * 3 };
		}
		std::span<const DirectX::XMFLOAT3> get_vertices_xm()const
		{
			return { reinterpret_cast<const DirectX::XMFLOAT3*>(xmesh->vert), size_t(xmesh->nvert) };
		}
		std::span<const float> get_normals()const
		{
			return { reinterpret_cast<const float*>(xmesh->normals), size_t(xmesh->nvert) * 3 };
		}
		std::span<const int> get_indices()const
		{
			return { reinterpret_cast<const int*>(xmesh->faces), size_t(xmesh->nfaces) * 3 };
		}
		const ttf_mesh3d_t& get()const
		{
			assert(xmesh && "Mesh was not set");
			return *xmesh;
		}
		const ttf_glyph_t& get_glyph()const
		{
			assert(glyph && "Glyph was not set!");
			return *glyph;
		}
	private:
		ttf_mesh3d_t* xmesh = nullptr;
		const ttf_glyph_t* glyph;
	};

	class glyph_view
	{
		friend class ttf_bridge;

	private:
		glyph_view(ttf_glyph_t* glyph) noexcept : glyph(glyph) {}

	public:
		const ttf_glyph_t& get() const noexcept { return *glyph; }
		bool valid() const noexcept { return glyph; }
		operator bool() const noexcept { return valid(); }
		ttf::mesh to_mesh(uint8_t quality) const noexcept
		{
			ttf::mesh mesh;
			ttf_glyph2mesh(const_cast<ttf_glyph_t*>(glyph), mesh.put(), quality, TTF_FEATURES_DFLT);
			return mesh;
		}
		ttf::mesh3d to_mesh3d(uint8_t quality, float depth) const noexcept
		{
			ttf::mesh3d mesh{ glyph };
			ttf_glyph2mesh3d(const_cast<ttf_glyph_t*>(glyph), mesh.put(), quality, TTF_FEATURES_DFLT, depth);
			return mesh;
		}
	private:
		const ttf_glyph_t* glyph;
	};

	class ttf_bridge
	{
	public:
		ttf_bridge(std::filesystem::path p);
		ttf_bridge(ttf_t* p) :xttf(p) {};
		ttf_bridge(std::span<uint8_t> memory) noexcept { ttf_load_from_mem(memory.data(), memory.size(), &xttf, false); }
		ttf_bridge(const ttf_bridge&) = delete;
		ttf_bridge(ttf_bridge&& other) noexcept : xttf(other.xttf) { other.xttf = nullptr; }
		~ttf_bridge() { ttf_free(xttf); }

	public:
		std::span<const char16_t> chars() const noexcept { return { reinterpret_cast<const char16_t*>(xttf->chars), size_t(xttf->nchars) }; }
		glyph_view find_glyph(char16_t c) const noexcept
		{
			int idx = ttf_find_glyph(xttf, c);
			return idx == -1 ? xttf->glyphs : xttf->glyphs + idx;
		}
		float get_line_space()const
		{
			return  xttf->os2.usWinAscent + -xttf->os2.usWinDescent + xttf->os2.sTypoLineGap;
		}
	private:
		ttf_t* xttf = nullptr;
	};
}  // namespace ttf