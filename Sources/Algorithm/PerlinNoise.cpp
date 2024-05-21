#include "PerlinNoise.h"

#include <algorithm>
#include <random>


// éQçl https://qiita.com/gis/items/ba7d715901a0e572b0e9

// --- SeedílÇê›íË ---
void PerlinNoise::SetSeed(const std::uint_fast32_t seed)
{
	for (std::size_t i = 0; i < 256; i++)
		p[i] = static_cast<Pint>(i);

	std::shuffle(p.begin(), p.begin() + 256, std::default_random_engine(seed));

	for (std::size_t i = 0; i < 256; i++)
		p[256 + i] = p[i];
}

double PerlinNoise::SetNoise(double x, double y, double z) const noexcept
{
    const std::size_t x_int = static_cast<std::size_t>(static_cast<std::size_t>(std::floor(x)) & 255);
    const std::size_t y_int = static_cast<std::size_t>(static_cast<std::size_t>(std::floor(y)) & 255);
    const std::size_t z_int = static_cast<std::size_t>(static_cast<std::size_t>(std::floor(z)) & 255);

    x -= std::floor(x);
    y -= std::floor(y);
    z -= std::floor(z);

    const double u = GetFade(x);
    const double v = GetFade(y);
    const double w = GetFade(z);

    const std::size_t a0 = static_cast<std::size_t>(p[x_int] + y_int);
    const std::size_t a1 = static_cast<std::size_t>(p[a0] + z_int);
    const std::size_t a2 = static_cast<std::size_t>(p[a0 + 1] + z_int);
    const std::size_t b0 = static_cast<std::size_t>(p[x_int + 1] + y_int);
    const std::size_t b1 = static_cast<std::size_t>(p[b0] + z_int);
    const std::size_t b2 = static_cast<std::size_t>(p[b0 + 1] + z_int);

    return GetLerp(w,
        GetLerp(v,
            GetLerp(u, GetGrad(p[a1], x, y, z), GetGrad(p[b1], x - 1, y, z)),
            GetLerp(u, GetGrad(p[a2], x, y - 1, z), GetGrad(p[b2], x - 1, y - 1, z))),
        GetLerp(v,
            GetLerp(u, GetGrad(p[a1 + 1], x, y, z - 1), GetGrad(p[b1 + 1], x - 1, y, z - 1)),
            GetLerp(u, GetGrad(p[a2 + 1], x, y - 1, z - 1), GetGrad(p[b2 + 1], x - 1, y - 1, z - 1))));
}
