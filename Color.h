#ifndef COLOR_H
#define COLOR_H

#include <math.h>
#include "vec.h"

typedef unsigned char Byte;

struct Color{
	Byte r, g, b;
};

const Color white = {255, 255, 255};
const Color red = {255, 0, 0};
const Color blue = {0, 0, 255};
const Color green = {0, 255, 0};
const Color black = {0, 0, 0};
const Color cyan = {0, 255, 255};
const Color yellow = {255, 255, 0};
const Color magenta = {255, 0, 255};
const Color gray = {128, 128, 128};
const Color orange = {255, 165, 0};
const Color purple = {128, 0, 128};
const Color brown = {160, 128, 96};

inline float ByteToFloat(Byte v){
	return v/255.0f;
}

inline float clamp(float v, float a, float b){
	if(v < a)
		return a;
	else if(v > b)
		return b;
	return v;
}

inline Byte floatToByte(float v){
	return round(255*clamp(v, 0.0f, 1.0f));
}

inline vec3 toVec3(Color C){
	return {
		ByteToFloat(C.r),
		ByteToFloat(C.g),
		ByteToFloat(C.b)
	};
}

inline Color toColor(vec3 v){
	return {
		floatToByte(v[0]),
		floatToByte(v[1]),
		floatToByte(v[2])
	};
}

inline Color lerp(float t, Color A, Color B){
	vec3 vA = toVec3(A);
	vec3 vB = toVec3(B);
	return toColor(lerp(t, vA, vB));
}

inline Color bilerp(float u, float v, Color A, Color B, Color C, Color D){
	vec3 vA = toVec3(A);
	vec3 vB = toVec3(B);
	vec3 vC = toVec3(C);
	vec3 vD = toVec3(D);
	return toColor(bilerp(u, v, vA, vB, vC, vD));
}

#endif
