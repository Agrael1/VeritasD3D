module;
#include <vector>
#include <optional>
#include <random>
#include <Scene/SolidSphere.h>
#include <Graphics.h>
export module Datastars;

export class Datastars
{
public:
	Datastars(Graphics& gfx)
	{
		std::mt19937 rng(std::random_device{}());
		std::uniform_real_distribution<float> rdist(0.05f, 2.0f);
		std::uniform_real_distribution<float> xdist(-20.0f, 20.0f);
		std::uniform_real_distribution<float> ydist(-20.0f, 20.0f);
		std::uniform_real_distribution<float> zdist(-20.0f, 20.0f);

		//for (auto& x:spheres)
		//{
		//	x.emplace(gfx, rdist(rng));
		//	x->SetPos({ xdist(rng), ydist(rng), zdist(rng) });
		//}
	}
public:
	void LinkTechniques(RG::RenderGraph& rg)
	{
		for (auto& x : spheres)
		{
			//x->LinkTechniques(rg);
		}
	}
	void Advance(float)
	{

	}
	void Submit()
	{
		for (auto& x : spheres)
		{
			//x->Submit();
		}
	}
private:
	static constexpr const size_t starcount = 64;
	//struct alignas(32) StarData
	//{
	//	float x[starcount];
	//	float y[starcount];
	//	float z[starcount];
	//	float m[starcount];
	//	float vx[starcount];
	//	float vy[starcount];
	//	float vz[starcount];
	//}data{};
	std::vector<SolidSphere> spheres;
};