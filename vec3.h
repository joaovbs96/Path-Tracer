#ifndef VEC3H
#define VEC3H

#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <float.h>
#include <random>

// Random Generators
thread_local uint32_t s_RndState = 1;

// XorShift32 & randomNumber by Aras Pranckevicius
// source: https://github.com/aras-p/ToyPathTracer/blob/master/Cpp/Source/Maths.cpp#L5-L18
static uint32_t XorShift32() {
	uint32_t x = s_RndState;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 15;
	s_RndState = x;
	return x;
}

float randomNumber() {
	return (XorShift32() & 0xFFFFFF) / 16777216.0f;
}

class vec3 {
public:
	vec3() {}

	vec3(float e0, float e1, float e2) {
		e[0] = e0;
		e[1] = e1;
		e[2] = e2;
		uv[0] = 0.0;
		uv[1] = 0.0;
	}

	// optional parameters UV for shading
	vec3(float e0, float e1, float e2, float uu, float vv) {
		e[0] = e0;
		e[1] = e1;
		e[2] = e2;
		uv[0] = uu;
		uv[1] = vv;
	}

	// Basic return functions
	inline float x() const {
		return e[0];
	}

	inline float y() const {
		return e[1];
	}

	inline float z() const {
		return e[2];
	}

	inline float r() const {
		return e[0];
	}

	inline float g() const {
		return e[1];
	}

	inline float b() const {
		return e[2];
	}

	inline float u() const {
		return uv[0];
	}

	inline float v() const {
		return uv[1];
	}

	// Operator overloading
	// +1 * vec3
	inline const vec3& operator+() const {
		return *this;
	}

	// -1 * vec3
	inline vec3 operator-() const {
		return vec3(-e[0], -e[1], -e[2]);
	}

	// vec3[i]
	inline float operator[](int i) const {
		return e[i];
	}

	// reference to vec3[i]
	inline float& operator[](int i) { 
		return e[i];
	}

	// overloading of +=, -=, etc
	inline vec3& operator+=(const vec3 &v2);
	inline vec3& operator-=(const vec3 &v2);
	inline vec3& operator*=(const vec3 &v2);
	inline vec3& operator/=(const vec3 &v2);
	inline vec3& operator*=(const float t);
	inline vec3& operator/=(const float t);

	// euclidean distance of vector
	inline float length() const {
		return sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]);
	}
	// square of distance
	inline float squared_length() const {
		return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
	}

	inline void make_unit_vector();

	float e[3];
	float uv[2];
};

inline std::istream& operator>>(std::istream &is, vec3 &t) {
	is >> t.e[0] >> t.e[1] >> t.e[2];
	return is;
}

inline std::ostream& operator<<(std::ostream &os, const vec3 &t) {
	os << t.e[0] << " " << t.e[1] << " " << t.e[2];
	return os;
}

inline void vec3::make_unit_vector() {
	float k = 1.0 / (sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]));
	e[0] *= k;
	e[1] *= k;
	e[2] *= k;
}

inline vec3 operator+(const vec3 &v1, const vec3 &v2) {
	return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

inline vec3 operator-(const vec3 &v1, const vec3 &v2) {
	return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

inline vec3 operator*(const vec3 &v1, const vec3 &v2) {
	return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

inline vec3 operator/(const vec3 &v1, const vec3 &v2) {
	return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

inline vec3 operator*(float t, const vec3 &v) {
	return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator/(const vec3 &v, float t) {
	return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}

inline vec3 operator*(const vec3 &v, float t) {
	return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline float dot(const vec3 &v1, const vec3 &v2) {
	return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

inline vec3 cross(const vec3 &v1, const vec3 &v2) {
	return vec3((v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
		-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0]),
		(v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));
}

inline vec3& vec3::operator+=(const vec3 &v) {
	e[0] += v.e[0];
	e[1] += v.e[1];
	e[2] += v.e[2];
	return *this;
}

inline vec3& vec3::operator-=(const vec3 &v) {
	e[0] -= v.e[0];
	e[1] -= v.e[1];
	e[2] -= v.e[2];
	return *this;
}

inline vec3& vec3::operator*=(const vec3 &v) {
	e[0] *= v.e[0];
	e[1] *= v.e[1];
	e[2] *= v.e[2];
	return *this;
}

inline vec3& vec3::operator/=(const vec3 &v) {
	e[0] /= v.e[0];
	e[1] /= v.e[1];
	e[2] /= v.e[2];
	return *this;
}

inline vec3& vec3::operator*=(const float t) {
	e[0] *= t;
	e[1] *= t;
	e[2] *= t;
	return *this;
}

inline vec3& vec3::operator/=(const float t) {
	float k = 1.0 / t;

	e[0] *= t;
	e[1] *= t;
	e[2] *= t;
	return *this;
}

inline vec3 unit_vector(vec3 v) {
	return v / v.length();
}

#endif