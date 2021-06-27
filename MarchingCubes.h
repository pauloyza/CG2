#ifndef MARCHING_CUBES_H
#define MARCHING_CUBES_H

#include "vec.h"
#include <functional>

inline int getCubeIndex(float V[8]){
	int r = 0;
	for(int i = 0; i < 8; i++)
		r |= (V[i] < 0) << i;
	return r;
}

inline void processCube(vec3 P[8], float V[8], std::vector<vec3>& tri_vertices){
	#include "MarchingCubesTables.h"
	
	vec3 edge_vertex[12];
	for(int e = 0; e < 12; e++){
		int ia = indicesTable[e][0];
		int ib = indicesTable[e][1];
		
		float va = V[ia];
		float vb = V[ib];
			
		if((va < 0) != (vb < 0)){
			float t = -va/(vb-va);
			edge_vertex[e] = (1-t)*P[ia] + t*P[ib];
		}
	}
	
	int cube_index = getCubeIndex(V);

	for(int* t = triTable[cube_index]; *t != -1; t++)
		tri_vertices.push_back(edge_vertex[*t]);
}

inline std::vector<vec3> marchingCubes(
	std::function<float(float, float, float)> F, 
	int nx, int ny, int nz, 
	vec3 pmin, vec3 pmax
){
	std::vector<vec3> tri_vertices;
	
	vec3 dif = pmax - pmin;
	float dx = dif.x/nx;
	float dy = dif.y/ny;
	float dz = dif.z/nz;
	
	for(int i = 0; i < nx; i++)
	for(int j = 0; j < ny; j++)
	for(int k = 0; k < nz; k++){
		float x = pmin.x + i*dx; 
		float y = pmin.y + j*dy; 
		float z = pmin.z + k*dz;
		
		vec3 P[8] = {
			{x, y, z},	    {x+dx, y, z}, 
			{x+dx, y, z+dz},    {x, y, z+dz},
			{x, y+dy, z},	    {x+dx, y+dy, z}, 
			{x+dx, y+dy, z+dz}, {x, y+dy, z+dz}
		};
		
		float V[8];
		for(int vi = 0; vi < 8; vi++)
			V[vi] = F(P[vi].x, P[vi].y, P[vi].z);
		
		processCube(P, V, tri_vertices);
	}
	
	return tri_vertices;
}

#endif
