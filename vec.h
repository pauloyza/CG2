#ifndef VEC_H
#define VEC_H

#include <math.h>
#include <complex>
#include <vector>
#include <fstream>

#ifndef M_PI
#define M_PI 3.1415926535897932
#endif

typedef std::complex<double> complex;

/*****************************************************************/
/******************************* VEC2 ****************************/
struct vec2{
	float x=0;
	float y=0;
	float& operator[](int i){
		return i==0? x: y;
	}
};

inline void print(vec2 v){
	printf("%g %g\n", v.x, v.y);
}

inline void print(const char* name, vec2 v){
	printf("%s = ", name);
	print(v);
}

inline vec2 operator+(vec2 u, vec2 v){
	return {u.x+v.x, u.y+v.y};
}

inline vec2 operator*(float a, vec2 v){
	return {a*v.x, a*v.y};
}

inline vec2 operator-(vec2 u, vec2 v){
	return {u.x-v.x, u.y-v.y};
}

inline vec2 operator-(vec2 v){
	return {-v.x, -v.y};
}

inline float dot(vec2 u, vec2 v){
	return u.x*v.x + u.y*v.y;
}

inline float norm(vec2 u){
	return sqrt(dot(u,u));
}

inline vec2 normalize(vec2 u){
	return (1/norm(u))*u;
}

inline complex toComplex(vec2 P){
	return {P.x, P.y};
}

inline vec2 toVec2(complex c){
	return {(float)c.real(), (float)c.imag()};
}

inline vec2 operator*(vec2 P, complex a){
	complex p = toComplex(P);
	return toVec2(p*a);
}

inline std::vector<vec2> loadCurve(const char* filename){
	std::ifstream in(filename);
	int n = 0;
	in >> n;
	std::vector<vec2> P(n);
	for(int i = 0; i < n; i++)
		in >> P[i].x >> P[i].y;
	return P;
}

inline float findMixParam(vec2 v, vec2 v0, vec2 v1){
	vec2 d = v1 - v0;
	return dot(d, v-v0)/dot(d,d);
}

inline vec2 lerp(float t, vec2 A, vec2 B){
	return (1-t)*A + t*B;
}

inline vec2 bilerp(float s, float t, vec2 A, vec2 B, vec2 C, vec2 D){
	vec2 E = lerp(s, A, B);
	vec2 F = lerp(s, C, D);
	return lerp(t, E, F);
}


/*****************************************************************/
/******************************* VEC3 ****************************/
struct vec3{
	float x = 0, y = 0, z = 0;
	float& operator[](int i){
		return i==0? x: i==1? y: z;
	}
};

inline vec3 toVec3(vec3 u){
	return u;
}

inline vec3 toVec3(vec2 u, float z = 0){
	return {u.x, u.y, z};
}

inline vec2 toVec2(vec3 u){
	return {u.x, u.y};
}

inline vec2 toVec2Homogeneous(vec3 u){
	return {u.x/u.z, u.y/u.z};
}

inline vec3 operator+(vec3 u, vec3 v){
	return {u.x + v.x, u.y + v.y, u.z + v.z};
}

inline vec3 operator*(float a, vec3 v){
	return {a*v.x, a*v.y, a*v.z};
}

inline vec3 operator-(vec3 u, vec3 v){
	return {u.x - v.x, u.y - v.y, u.z - v.z};
}

inline vec3 operator-(vec3 u){
	return {-u.x, -u.y, -u.z};
}

inline float dot(vec3 u, vec3 v){
	return u.x*v.x + u.y*v.y + u.z*v.z;
}

inline float norm(vec3 u){
	return sqrt(dot(u,u));
}

inline vec3 normalize(vec3 u){
	return (1/norm(u))*u;
}

inline vec3 cross(vec3 u, vec3 v){
	return {
		 u.y*v.z - u.z*v.y,
		 u.z*v.x - u.x*v.z,
		 u.x*v.y - u.y*v.x
	};
}

inline void print(vec3 u){
	printf("%g %g %g\n", u.x, u.y, u.z);
}

inline void print(const char* name, vec3 v){
	printf("%s = ", name);
	print(v);
}

inline vec3 lerp(float t, vec3 A, vec3 B){
	return (1-t)*A + t*B;
}

inline vec3 bilerp(float s, float t, vec3 A, vec3 B, vec3 C, vec3 D){
	vec3 E = lerp(s, A, B);
	vec3 F = lerp(s, C, D);
	return lerp(t, E, F);
}

/*****************************************************************/
/******************************* VEC4 ****************************/
struct vec4{
	float x = 0, y = 0, z = 0, w = 0;
	float& operator[](int i){
		return i==0? x: i==1? y: i==2? z: w;
	}
};

inline vec4 operator+(vec4 u, vec4 v){
	return {u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w};
}

inline vec4 operator*(float a, vec4 v){
	return {a*v.x, a*v.y, a*v.z, a*v.w};
}

inline vec4 operator-(vec4 u, vec4 v){
	return {u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w};
}

inline float dot(vec4 u, vec4 v){
	return u.x*v.x + u.y*v.y + u.z*v.z + u.w*v.w;
}

inline vec3 toVec3(vec4 u){
	return {u.x, u.y, u.z};
}

inline vec3 toVec3Homogeneous(vec4 u){
	return {u.x/u.w, u.y/u.w, u.z/u.w};
}

inline vec4 toVec4(vec2 u, float z=0, float w=1){
	return {u.x, u.y, z, w};
}
inline vec4 toVec4(vec3 u, float w=1){
	return {u.x, u.y, u.z, w};
}
inline vec4 toVec4(vec4 u){
	return u;
}

inline void print(vec4 u){
	printf("%g %g %g %g\n", u.x, u.y, u.z, u.w);
}

inline void print(const char* name, vec4 v){
	printf("%s = ", name);
	print(v);
}

inline vec4 lerp(float t, vec4 v0, vec4 v1){
	return (1-t)*v0 + t*v1;
}

inline vec4 bilerp(float s, float t, vec4 A, vec4 B, vec4 C, vec4 D){
	vec4 E = lerp(s, A, B);
	vec4 F = lerp(s, C, D);
	return lerp(t, E, F);
}
#endif
