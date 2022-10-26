#pragma once
#include <filesystem>
#include <span>
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
        mesh3d() = default;
        ~mesh3d() { ttf_free_mesh3d(xmesh); }
        mesh3d(const mesh3d&) = delete;
        mesh3d(mesh3d&& other) noexcept : xmesh(other.xmesh) { other.xmesh = nullptr; }

    public:
        [[nodiscard]] ttf_mesh3d_t** put() noexcept
        {
            clear();
            return &xmesh;
        }
        void clear() noexcept
        {
            if (xmesh)
            {
                ttf_free_mesh3d(xmesh);
                xmesh = nullptr;
            }
        }
        const ttf_mesh3d_t& get()const
        {
            return *xmesh;
        }
    private:
        ttf_mesh3d_t* xmesh = nullptr;
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
            ttf::mesh3d mesh;
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
        ttf_bridge(ttf_t* p):xttf(p){};
        ttf_bridge(std::span<uint8_t> memory) noexcept { ttf_load_from_mem(memory.data(), memory.size(), &xttf, false); }
        ttf_bridge(const ttf_bridge&) = delete;
        ttf_bridge(ttf_bridge&& other) noexcept : xttf(other.xttf) { other.xttf = nullptr; }
        ~ttf_bridge() { ttf_free(xttf); }

    public:
        std::span<const char16_t> chars() const noexcept { return { reinterpret_cast<const char16_t*>(xttf->chars), size_t(xttf->nchars) }; }
        glyph_view find_glyph(char16_t c) const noexcept
        {
            int idx = ttf_find_glyph(xttf, c);
            return idx == -1 ? nullptr : xttf->glyphs + idx;
        }

    private:
        ttf_t* xttf = nullptr;
    };
}  // namespace ttf