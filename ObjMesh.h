#ifndef READ_OBJ_H
#define READ_OBJ_H

#include <vector>
#include <iostream>
#include <map>
#include "Primitives.h"

struct MaterialInfo{
	std::string name;
	float Ns;
	float d;
	int illum;
	vec3 Kd;
	vec3 Ks;
	vec3 Ka;
	std::string map_Ka;
	std::string map_Kd;
	std::string map_Ks;
	std::string map_Bump;
};
	
struct MaterialRange{
	MaterialInfo mat;
	unsigned int first;
	unsigned int count;
};
	

using MeshMaterial = std::map<std::string, MaterialInfo>;

std::istream& operator>>(std::istream& in, MeshMaterial& v){
	std::string str;
	MaterialInfo* info = nullptr;
	while(in >> str){
		// Ignore comments
		if(str[0] == '#'){
			std::getline(in, str);
			continue;
		}

		// Material
		if(str == "newmtl"){
			std::string name;
			in >> name;
			info = &v[name];
			info->name = name;
		}
		if(info == nullptr)
			continue;

		if(str == "Ns"){
			in >> info->Ns;
		}else if(str == "d"){
			in >> info->d;
		}else if(str == "illum"){
			in >> info->illum;
		}else if(str == "Kd"){
			vec3& v = info->Kd;
			in >> v.x >> v.y >> v.z;
		}else if(str == "Ks"){
			vec3& v = info->Ks;
			in >> v.x >> v.y >> v.z;
		}else if(str == "Ka"){
			vec3& v = info->Ka;
			in >> v.x >> v.y >> v.z;
		}else if(str == "map_Ka"){
			in >> info->map_Ka;
		}else if(str == "map_Kd"){
			in >> info->map_Kd;
		}else if(str == "map_Bump"){
			in >> info->map_Bump;
		}else if(str == "map_Ks"){
			in >> info->map_Ks;
		}
	}
	return in;
}

struct ObjMesh{
	struct Vertex{
		vec3 position;
		vec2 texCoords;
		vec3 normal;
	};
	struct VertIndices{
		int pos = -1;
		int tex = -1;
		int nor = -1;
		
		friend std::istream& operator>>(std::istream& in, VertIndices& v){
			in >> v.pos;
			if(in.eof())
				return in;
			v.tex = -1;
			v.nor = -1;
			if(in.peek() == '/'){
				in.ignore();
				
				if(in.peek() != '/')
					in >> v.tex;
			
				if(in.eof())
					return in;
				if(in.peek() == '/'){
					in.ignore();
					in >> v.nor;
				}
			}
			return in;
		}

		bool operator<(VertIndices other) const{
			std::array<int, 3> a{pos, tex, nor};
			std::array<int, 3> b{other.pos, other.tex, other.nor};
			// Lexicographical less
			return a < b;
		}
	};

	struct Face{
		std::vector<VertIndices> verts;
		friend std::istream& operator>>(std::istream& in, Face& f){
			VertIndices v;
			while(in >> v)
				f.verts.push_back(v);
			return in;
		}
	};


	struct Group{
		unsigned int n_faces;
		std::string material;
	};

	std::string path;

	std::vector<vec3> position;
	std::vector<vec3> normal;
	std::vector<vec2> texCoords;
	std::vector<Face> faces;
	std::vector<Group> groups;
	MeshMaterial mesh_material;

	ObjMesh(std::string filename){
		auto pos = filename.find_last_of('/');
		path = filename.substr(0, pos+1);

		std::ifstream in(filename);
		std::string line;
		while(getline(in, line))
			parseLine(line);
	}

	void parseLine(std::string line){
		std::string op;
		std::istringstream in(line);
		in >> op;
		
		// Ignore comments
		if(op[0] == '#')
			return;

		if(op == "v"){
			vec3 v = {0, 0, 0};
			in >> v.x >> v.y >> v.z;
			position.push_back(v);
		}else if(op == "vn"){
			vec3 v = {0, 0, 0};
			in >> v.x >> v.y >> v.z;
			normal.push_back(v);
		}else if(op == "vt"){
			vec2 v = {0, 0};
			in >> v.x >> v.y;
			texCoords.push_back(v);
		}else if(op == "f"){
			Face f;
			in >> f;
			faces.push_back(f);
			if(groups.empty())
				groups.push_back(Group{0, ""});
			groups.back().n_faces++;
		}else if(op == "mtllib"){
			std::string mtlfile;
			std::getline(in, mtlfile);
			while(!mtlfile.empty() && std::isspace(mtlfile.front()))
				mtlfile.erase(mtlfile.begin());
			while(!mtlfile.empty() && std::isspace(mtlfile.back()))
				mtlfile.pop_back();

			std::ifstream mtl{path + '/' + mtlfile};
			mtl >> mesh_material;
		}else if(op == "usemtl"){
			std::string mtl;
			in >> mtl;
			groups.push_back(Group{0, mtl});
		}
	}

	Vertex getVertex(VertIndices v) const{
		Vertex vert;
		vert.position = {position[v.pos-1]};
		if(v.tex > 0)
			vert.texCoords = texCoords[v.tex-1];
		if(v.nor > 0)
			vert.normal = normal[v.nor-1];
		return vert;
	}

	std::vector<Vertex> getTriangles() const{
		std::vector<Vertex> tris;

		for(Face face: faces){
			TriangleFan F{face.verts.size()};
			for(Triangle<VertIndices> tri: assemble(F, face.verts)){
				tris.push_back(getVertex(tri[0]));
				tris.push_back(getVertex(tri[1]));
				tris.push_back(getVertex(tri[2]));
			}
		}

		return tris;
	}

	std::vector<MaterialRange> getMaterials(MaterialInfo standard_material={}) const{
		std::vector<MaterialRange> mats;
		
		int f = 0;
		int offset = 0;
		for(Group G: groups){
			MaterialRange range;

			auto it = mesh_material.find(G.material);
			range.mat = (it != mesh_material.end())? it->second: standard_material;

			range.first = offset;

			range.count = 0;
			for(unsigned int i = 0; i < G.n_faces; i++, f++){
				const Face& face = faces[f];
				TriangleFan F{face.verts.size()};
				range.count += 3*F.size();
			}
			offset += range.count;

			mats.push_back(range);
		}
	
		return mats;
	}


	vec3 center() const{
		vec3 c = {0, 0, 0};
		for(vec3 v: position)
			c = c + v;
		return (1.0/position.size())*c;
	}
	
};



#endif
