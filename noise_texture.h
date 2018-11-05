#ifndef NOISE_TEXTUREH
#define NOISE_TEXTUREH

#include "texture.h"
#include "perlin.h"

class noise_texture : public texture {
	public:
		noise_texture() {}

		noise_texture(float sc) : scale(sc) {}

		virtual vec3 value(float u, float v, const vec3& p) const {
			return vec3(1, 1, 1) * 0.5 * (1 + sin(scale * p.x() + 5 * noise.turb(scale * p)));
		}

		perlin noise;
		float scale;
};


#endif // !NOISE_TEXTUREH
