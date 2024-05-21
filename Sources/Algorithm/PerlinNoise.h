#pragma once

#include <cstdint>
#include <array>

class PerlinNoise
{
public:
	constexpr PerlinNoise() = default;
	explicit PerlinNoise(const std::uint_fast32_t seed)
	{
		SetSeed(seed);
	}

	// --- SEED値を設定 ---
	void SetSeed(const std::uint_fast32_t seed);


	// --- オクターブなしのノイズを取得 ---
	template <typename... Args>
	double Noise(const Args... args) const noexcept
	{
		return SetNoise(args...) * 0.5 + 0.5;
	}

private:
	using Pint = std::uint_fast8_t;


	// --- パーリンノイズ生成関数 ---
	constexpr double GetFade(const double t) const noexcept
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	constexpr double GetLerp(const double t, const double a, const double b) const noexcept
	{
		return a + t * (b - a);
	}

	constexpr double MakeGrad(const Pint hash, const double u, const double v) const noexcept
	{
		return (((hash & 1) == 0) ? u : -u) + (((hash & 2) == 0) ? v : -v);
	}

	constexpr double MakeGrad(const Pint hash, const double x, const double y, const double z) const noexcept
	{
		return MakeGrad(hash, hash < 8 ? x : y, hash < 4 ? y : hash == 12 || hash == 14 ? x : z);
	}

	constexpr double GetGrad(const Pint hash, const double x, const double y, const double z) const noexcept
	{
		return MakeGrad(hash & 15, x, y, z);
	}


	// --- ノイズ生成 ---
	double SetNoise(double x = 0.0, double y = 0.0, double z = 0.0) const noexcept;


	std::array<Pint, 512> p{ {} };
};

