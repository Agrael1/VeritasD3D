#pragma once
#include <utils.h>
#include <span>
#include <unordered_map>
#include <DirectXMath.h>
#include <string>


namespace ver::ph
{
	struct string_hash {
		using is_transparent = void;
		[[nodiscard]] size_t operator()(const char* txt) const {
			return std::hash<std::string_view>{}(txt);
		}
		[[nodiscard]] size_t operator()(std::string_view txt) const {
			return std::hash<std::string_view>{}(txt);
		}
		[[nodiscard]] size_t operator()(const std::string& txt) const {
			return std::hash<std::string>{}(txt);
		}
	};

	class Physics
	{
		static constexpr bool record_allocations = true;
	public:
		Physics();
	public:
		ver::ph::physx_ptr<physx::PxTriangleMesh> MakeTriangleMesh(std::span<DirectX::XMFLOAT3> vertices, std::span<uint16_t> indices)const;
		ver::ph::physx_ptr<physx::PxRigidActor> MakeActor(ver::ph::physx_ptr<physx::PxTriangleMesh> shape, const physx::PxMaterial& mat, float scale);
		physx::PxMaterial* RegisterMaterial(std::string name, float staticFriction, float dynamicFriction, float restitution);
		physx::PxMaterial* GetMaterial(std::string_view name)
		{
			auto x = materials.find(name);
			return x == materials.end() ?
				nullptr : materials.find(name)->second.get();
		}
	public:
		template<class Self>
		auto&& get_physics(this Self&& s)noexcept
		{
			return *s.physics;
		}
		template<class Self>
		auto&& get_dispatcher(this Self&& s)noexcept
		{
			return *s.dispatcher;
		}
		template<class Self>
		auto&& get_cooking(this Self&& s)noexcept
		{
			return *s.cooking;
		}
	private:
		physx::PxDefaultAllocator da;
		physx::PxDefaultErrorCallback de;
		ver::ph::physx_ptr<physx::PxFoundation> foundation;
		ver::ph::physx_ptr<physx::PxPvd> debugger;
		ver::ph::physx_ptr<physx::PxPhysics> physics;
		ver::ph::physx_ptr<physx::PxCooking> cooking;
		std::unique_ptr<physx::PxCpuDispatcher> dispatcher;
		std::unordered_map<std::string, ver::ph::physx_ptr<physx::PxMaterial>, string_hash, std::equal_to<>> materials;
		std::unordered_map<std::string, ver::ph::physx_ptr<physx::PxShape>, string_hash, std::equal_to<>> shapes;
	};
}