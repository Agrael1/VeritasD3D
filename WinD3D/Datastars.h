#pragma once
#include <vector>
#include <optional>
#include <random>
#include "Star.h"
#include <Graphics.h>
#include <span>

class Datastars
{
public:
	Datastars(Graphics& gfx)
		:BlackHole(gfx, 0.1)
	{
		CreateGravitySystem();
		BlackHole.SetPos({ 0.0f,0.0f,0.0f });
		BlackHole.SetColor({ 0.0f,0.0f,0.0f });
		BlackHole.UpdateColor(gfx);

		for (auto& x : spheres)
		{
			x = std::make_unique<Star>(gfx, 0.02f);
			x->SetColor({ 1.0f,0.7f,1.0f });
			x->UpdateColor(gfx);
		}
	}
public:
	void PositionStep(__m256d dt)
	{
		for (int i = 0; i < BODYCOUNT / 4; i++)
		{
			__m256d accelerationFactor = _mm256_mul_pd(xm[i], _mm256_mul_pd(dt, _mm256_set1_pd(0.5)));

			__m256d velocityishX = velx[i];
			__m256d velocityishY = vely[i];
			__m256d velocityishZ = velz[i];

			velocityishX = _mm256_fmadd_pd(forcex[i], accelerationFactor, velocityishX);
			velocityishY = _mm256_fmadd_pd(forcey[i], accelerationFactor, velocityishY);
			velocityishZ = _mm256_fmadd_pd(forcez[i], accelerationFactor, velocityishZ);

			posx[i] = _mm256_fmadd_pd(velocityishX, dt, posx[i]);
			posy[i] = _mm256_fmadd_pd(velocityishY, dt, posy[i]);
			posz[i] = _mm256_fmadd_pd(velocityishZ, dt, posz[i]);

			pforcex[i] = forcex[i];
			pforcey[i] = forcey[i];
			pforcez[i] = forcez[i];
		}
	}
	void AccelerationStep(__m256d dt)
	{
		auto halfdt = _mm256_mul_pd(dt, _mm256_set1_pd(0.5));
		for (int i = 0; i < BODYCOUNT / 4; i++)
		{
			auto accelerationFactor = _mm256_mul_pd(xinm[i], halfdt);

			velx[i] = _mm256_fmadd_pd(_mm256_add_pd(forcex[i], pforcex[i]), accelerationFactor, velx[i]);
			vely[i] = _mm256_fmadd_pd(_mm256_add_pd(forcey[i], pforcey[i]), accelerationFactor, vely[i]);
			velz[i] = _mm256_fmadd_pd(_mm256_add_pd(forcez[i], pforcez[i]), accelerationFactor, velz[i]);

			pforcex[i] = forcex[i];
			pforcey[i] = forcey[i];
			pforcez[i] = forcez[i];

			forcex[i] = _mm256_setzero_pd();
			forcey[i] = _mm256_setzero_pd();
			forcez[i] = _mm256_setzero_pd();
		}
	}
	void XUpdateForces()
	{
		//applicator.handleBlackHoles = function() {
		auto bposx = _mm256_set1_pd(0.0);
		auto bposy = _mm256_set1_pd(0.0);
		auto bposz = _mm256_set1_pd(0.0);
		auto blm = _mm256_set1_pd(BlackHoleMass);
		auto uscrcp = _mm256_set1_pd(UNIVERSE_SCALE);
		auto uscrcp2 = _mm256_set1_pd(UNIVERSE_SCALE * UNIVERSE_SCALE);
		auto g = _mm256_set1_pd(GRAVITATIONAL_CONSTANT);
		auto eps2 = _mm256_set1_pd(GRAVITY_EPSILON * GRAVITY_EPSILON);
		auto dforce = _mm256_set1_pd(DARK_FORCE_COEFFICIENT * UNIVERSE_SCALE);
		auto ones = _mm256_set1_pd(1.0);
		union
		{
			double x;
			uint64_t y = 0xffffffffffffffff;
		}r;
		auto fix1 = _mm256_set_pd(r.x, 0, 0, 0);

		for (int i = 0; i < BODYCOUNT / 4; i++)
		{
			auto dx = _mm256_sub_pd(bposx, posx[i]);
			auto dy = _mm256_sub_pd(bposy, posy[i]);
			auto dz = _mm256_sub_pd(bposz, posz[i]);

			auto sqrtDist = _mm256_fmadd_pd(dx, dx, _mm256_fmadd_pd(dy, dy, _mm256_mul_pd(dz, dz)));
			auto dist = _mm256_div_pd(ones, _mm256_sqrt_pd(sqrtDist));
			dx = _mm256_mul_pd(dx, dist);
			dy = _mm256_mul_pd(dy, dist);
			dz = _mm256_mul_pd(dz, dist);

			auto xr = _mm256_fmadd_pd(sqrtDist, uscrcp2, eps2);
			xr = _mm256_sqrt_pd(_mm256_mul_pd(xr, _mm256_mul_pd(xr, xr)));
			auto mm = _mm256_mul_pd(xm[i], blm);
			auto force = _mm256_div_pd(_mm256_mul_pd(g, mm), xr);
			force = _mm256_fmadd_pd(_mm256_mul_pd(dforce, mm), dist, force);

			forcex[i] = _mm256_fmadd_pd(dx, force, forcex[i]);
			forcey[i] = _mm256_fmadd_pd(dy, force, forcey[i]);
			forcez[i] = _mm256_fmadd_pd(dz, force, forcez[i]);
		}

		//applicator.handlePairForces = function(bodyCountToUpdatePairForcesFor){

		for (int i = 0; i < BODYCOUNT / 4; i++)
		{
			auto ax = posx[i];
			auto ay = posy[i];
			auto az = posz[i];
			auto m = xm[i];
			auto fx = forcex[i];
			auto fy = forcey[i];
			auto fz = forcez[i];

			for (int o = i * 4 + 1; o < BODYCOUNT; o++)
			{
				auto bx = _mm256_loadu_pd(particles.positionX + o);
				auto by = _mm256_loadu_pd(particles.positionY + o);
				auto bz = _mm256_loadu_pd(particles.positionZ + o);
				auto bm = _mm256_loadu_pd(particles.mass + o);


				auto dx = _mm256_sub_pd(bx, ax);
				auto dy = _mm256_sub_pd(by, ay);
				auto dz = _mm256_sub_pd(bz, az);

				auto sqrtDist = _mm256_fmadd_pd(dx, dx, _mm256_fmadd_pd(dy, dy, _mm256_mul_pd(dz, dz)));
				auto dist = _mm256_div_pd(ones, _mm256_sqrt_pd(sqrtDist));
				dx = _mm256_mul_pd(dx, dist);
				dy = _mm256_mul_pd(dy, dist);
				dz = _mm256_mul_pd(dz, dist);

				auto xr = _mm256_fmadd_pd(sqrtDist, uscrcp2, eps2);
				xr = _mm256_sqrt_pd(_mm256_mul_pd(xr, _mm256_mul_pd(xr, xr)));
				auto mm = _mm256_mul_pd(m, bm);
				auto force = _mm256_div_pd(_mm256_mul_pd(g, mm), xr);

				auto dfx = _mm256_mul_pd(dx, force);
				auto dfy = _mm256_mul_pd(dy, force);
				auto dfz = _mm256_mul_pd(dz, force);

				forcex[i] = _mm256_add_pd(dfx, fx);
				forcey[i] = _mm256_add_pd(dfy, fy);
				forcez[i] = _mm256_add_pd(dfz, fz);

				_mm256_storeu_pd(particles.forceX + o, _mm256_sub_pd(_mm256_loadu_pd(particles.forceX + o), dfx));
				_mm256_storeu_pd(particles.forceY + o, _mm256_sub_pd(_mm256_loadu_pd(particles.forceY + o), dfy));
				_mm256_storeu_pd(particles.forceZ + o, _mm256_sub_pd(_mm256_loadu_pd(particles.forceZ + o), dfz));
			}
		}
	}
	void XFlatten()
	{
		for (int i = 0; i < BODYCOUNT / 4; i++)
		{
			auto z = _mm256_set1_pd(-0.0);
			auto ab = _mm256_andnot_pd(z, posy[i]);
			ab = _mm256_cmp_pd(ab, _mm256_set1_pd(100.0), 0x1E);

			auto m1 = _mm256_cmp_pd(posy[i], z, 0x1E);
			auto xm1 = _mm256_cmp_pd(vely[i], z, 0x1E);
			m1 = _mm256_and_pd(m1, xm1);

			auto m2 = _mm256_cmp_pd(posy[i], z, 0x1);
			auto xm2 = _mm256_cmp_pd(vely[i], z, 0x1);
			m2 = _mm256_and_pd(m2, xm2);
			m1 = _mm256_or_pd(m1, m2);
			m2 = _mm256_and_pd(ab, m1);

			m1 = _mm256_mul_pd(vely[i], _mm256_set1_pd(0.5));
			vely[i] = _mm256_blendv_pd(vely[i], m1, m2);
		}
	}

	void LinkTechniques(RG::RenderGraph& rg)
	{
		BlackHole.LinkTechniques(rg);
		for (auto& x : spheres)
		{
			x->LinkTechniques(rg);
		}
	}
	void UnlinkTechniques()
	{
		BlackHole.UnlinkTechniques();
		for (auto& x : spheres)
		{
			x->UnlinkTechniques();
		}
	}
	void Advance(float dt)
	{
		dt *= 10000000000;
		auto xdt = _mm256_set1_pd(dt);
		PositionStep(xdt);
		XUpdateForces();
		if (!upd)XFlatten();

		AccelerationStep(xdt);

		for (size_t i = 0; auto & s:spheres)
		{
			s->SetPos(DirectX::XMFLOAT3(particles.positionX[i] / 500, particles.positionY[i] / 500, particles.positionZ[i] / 500));
			i++;
		}

		upd = (upd + 1) % 100;
	}
	void Submit()
	{
		BlackHole.Submit();
		for (auto& x : spheres)
		{
			x->Submit();
		}
	}
private:
	void CreateGravitySystem()
	{
		namespace dx = DirectX;
		double side = 4300.0;
		std::uniform_real_distribution<float> zdist(0.0f, 1.0f);
		double mass = 0.0;
		for (int p = 0; p < BODYCOUNT; p++)
		{
			float angle = dx::XM_PI * 2.0 * zdist(rng);
			angle += 0.10 * sin(angle * dx::XM_PI * 2.0);

			float pX = 0.0;
			float pY = 0.0;
			float pZ = 0.0;

			float xVel = 0.0f;
			float yVel = 0.0f;
			float zVel = 0.0f;

			float dist = side * 0.5 * zdist(rng);
			dist += side * 0.04 * -cos(angle * dx::XM_PI * 2);
			mass = TYPICAL_STAR_MASS * 2 /** zdist(rng) * zdist(rng)*/;

			pX = dist * cos(angle);
			pY = (zdist(rng) - 0.5) * (side - dist) * 0.7;
			pZ = dist * sin(angle);

			float bh_pos_x = 0.0;
			float bh_pos_y = 0.0;
			float bh_pos_z = 0.0;

			float vel_x = pX;
			float vel_y = pY;
			float vel_z = pX;

			float vel_size = sqrt(vel_x * vel_x + vel_y * vel_y + vel_z * vel_z);

			vel_x /= vel_size;
			vel_y /= vel_size;
			vel_z /= vel_size;

			float requiredSpeed = typicalStarSpeed * 1.8 + typicalStarSpeed * 0.1 * log(1.1 + (10 * dist / side));

			xVel = vel_z * requiredSpeed;
			yVel = vel_y * requiredSpeed;
			zVel = -vel_x * requiredSpeed;

			particles.positionX[p] = pX + bh_pos_x;
			particles.positionY[p] = pY + bh_pos_y;
			particles.positionZ[p] = pZ + bh_pos_z;

			particles.velocityX[p] = xVel;
			particles.velocityY[p] = yVel;
			particles.velocityZ[p] = zVel;


			particles.forceX[p] = 0.0;
			particles.forceY[p] = 0.0;
			particles.forceZ[p] = 0.0;

			particles.prevForceX[p] = 0.0;
			particles.prevForceY[p] = 0.0;
			particles.prevForceZ[p] = 0.0;

			particles.mass[p] = mass;
			particles.invMass[p] = 1.0 / mass;
		}

		particles.mass[BODYCOUNT + 3] = BlackHoleMass;
		particles.positionX[BODYCOUNT + 3] = 0.0f;
		particles.positionY[BODYCOUNT + 3] = 0.0f;
		particles.positionZ[BODYCOUNT + 3] = 0.0f;
		particles.velocityX[BODYCOUNT + 3] = 0.0f;
		particles.velocityY[BODYCOUNT + 3] = 0.0f;
		particles.velocityZ[BODYCOUNT + 3] = 0.0f;
	}
private:
	static constexpr const double G = 6.67408e-11;
	static constexpr const double TYPICAL_STAR_MASS = 1.98847e+30;
	static constexpr const double StellarRadius = 7.e+5;

	static constexpr const double LIGHT_YEAR = 9.4607e+15;
	static constexpr const double MILKY_WAY_DIAMETER = 140000 * LIGHT_YEAR;
	static constexpr const double UNIVERSE_SCALE = 1000 / MILKY_WAY_DIAMETER;
	static constexpr const double UNIVERSE_SCALE_RECIPROCAL = 1.0 / UNIVERSE_SCALE;
	static constexpr const double BlackHoleMass = TYPICAL_STAR_MASS * 7000;
	static constexpr const double LIGHT_YEAR_SCALED = LIGHT_YEAR * UNIVERSE_SCALE;
	static constexpr const double LIGHT_SPEED = 299792458;
	static constexpr const double LIGHT_SPEED_SCALED = LIGHT_SPEED * UNIVERSE_SCALE;
	static constexpr const double LIGHT_SPEED_SCALED_SQRD = LIGHT_SPEED_SCALED * LIGHT_SPEED_SCALED;
	static constexpr const double typicalStarSpeed = 5.6e+10 * UNIVERSE_SCALE;

	static constexpr const double G_SCALE = 1;
	static constexpr const double GRAVITATIONAL_CONSTANT = G_SCALE * G;
	static constexpr const double DARK_FORCE_COEFFICIENT = 4e-20 * GRAVITATIONAL_CONSTANT;

	static constexpr const double GRAVITY_EPSILON = 3e+19;

	static constexpr const size_t BODYCOUNT = 4096;
	static constexpr const size_t XBODYCOUNT = BODYCOUNT + 4;
	struct alignas(32) StarData
	{
		double mass[XBODYCOUNT];
		double invMass[XBODYCOUNT];
		double positionX[XBODYCOUNT];
		double positionY[XBODYCOUNT];
		double positionZ[XBODYCOUNT];

		double velocityX[XBODYCOUNT];
		double velocityY[XBODYCOUNT];
		double velocityZ[XBODYCOUNT];

		double forceX[XBODYCOUNT];
		double forceY[XBODYCOUNT];
		double forceZ[XBODYCOUNT];

		double prevForceX[XBODYCOUNT];
		double prevForceY[XBODYCOUNT];
		double prevForceZ[XBODYCOUNT];
	}particles{};
	std::array<std::unique_ptr<Star>, BODYCOUNT> spheres;
	SolidSphere BlackHole;

	std::span<__m256d> velx{ (__m256d*)particles.velocityX , BODYCOUNT / 4 };
	std::span<__m256d> vely{ (__m256d*)particles.velocityY , BODYCOUNT / 4 };
	std::span<__m256d> velz{ (__m256d*)particles.velocityZ , BODYCOUNT / 4 };

	std::span<__m256d> forcex{ (__m256d*)particles.forceX , BODYCOUNT / 4 };
	std::span<__m256d> forcey{ (__m256d*)particles.forceY , BODYCOUNT / 4 };
	std::span<__m256d> forcez{ (__m256d*)particles.forceZ , BODYCOUNT / 4 };

	std::span<__m256d> pforcex{ (__m256d*)particles.forceX , BODYCOUNT / 4 };
	std::span<__m256d> pforcey{ (__m256d*)particles.forceY , BODYCOUNT / 4 };
	std::span<__m256d> pforcez{ (__m256d*)particles.forceZ , BODYCOUNT / 4 };

	std::span<__m256d> posx{ (__m256d*)particles.positionX , BODYCOUNT / 4 };
	std::span<__m256d> posy{ (__m256d*)particles.positionY , BODYCOUNT / 4 };
	std::span<__m256d> posz{ (__m256d*)particles.positionZ , BODYCOUNT / 4 };

	std::span<__m256d> xinm{ (__m256d*)particles.invMass , BODYCOUNT / 4 };
	std::span<__m256d> xm{ (__m256d*)particles.mass , BODYCOUNT / 4 };

	std::mt19937 rng{ std::random_device{}() };
	size_t upd = 0;
};