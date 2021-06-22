#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <iostream>
#include <array>
#include <vector>

template<class Vertex, class Prims>
auto assemble(const Prims& P, const std::vector<Vertex>& V)
{
	using Primitive = decltype(P.assemble(0, V));
	std::vector<Primitive> res(P.size());
	for(unsigned int i = 0; i < P.size(); i++)
		res[i] = P.assemble(i, V);
	return res;
}

///////////////////////////////////////////////////////////////////////
template<class V>
using Line = std::array<V, 2>;

class Lines{
	size_t n;
	public:
	Lines(size_t s): n{s/2}{}

	size_t size() const{ return n; }

	template<typename Vertex>
	Line<Vertex> assemble(unsigned int i, const std::vector<Vertex>& V) const{
		return { V[2*i], V[2*i+1] };
	}
};

class LineStrip{
	/*****************************/
	/* TAREFA: AULA 06 */
	size_t n;
	public:
	LineStrip(size_t s): n{s>1? s-1: 0}{}

	size_t size() const{ return n; }

	template<typename Vertex>
	Line<Vertex> assemble(unsigned int i, const std::vector<Vertex>& V) const{
		return { V[i], V[i+1] };
	}
	/*****************************/
};

class LineLoop{
	/*****************************/
	/* TAREFA: AULA 06 */
	size_t n;
	public:
	LineLoop(size_t s): n{s}{}

	size_t size() const{ return n; }

	template<typename Vertex>
	Line<Vertex> assemble(unsigned int i, const std::vector<Vertex>& V) const{
		return { V[i], V[(i+1)%n] };
	}
	/*****************************/
};

///////////////////////////////////////////////////////////////////////
template<class V>
using Triangle = std::array<V, 3>;

class Triangles{
	size_t n;
	public:
	Triangles(size_t s): n{s/3}{}

	size_t size() const{ return n; }

	template<typename Vertex>
	Triangle<Vertex> assemble(unsigned int i, const std::vector<Vertex>& V) const{
		return { V[3*i], V[3*i+1], V[3*i+2] };
	}
};

class TriangleStrip{
	/*****************************/
	/* TAREFA: AULA 06 */
	size_t n;
	public:
	TriangleStrip(size_t s): n{s>2? s-2: 0}{}

	size_t size() const{ return n; }

	template<typename Vertex>
	Triangle<Vertex> assemble(unsigned int i, const std::vector<Vertex>& V) const{
		return { V[i], V[i+1], V[i+2] };
	}
	/*****************************/
};

class TriangleFan{
	/*****************************/
	/* TAREFA: AULA 06 */
	size_t n;
	public:
	TriangleFan(size_t s): n{s>2? s-2: 0}{}

	size_t size() const{ return n; }

	template<typename Vertex>
	Triangle<Vertex> assemble(unsigned int i, const std::vector<Vertex>& V) const{
		return { V[0], V[i+1], V[i+2] };
	}
	/*****************************/
};

///////////////////////////////////////////////////////////////////////
template<class Primitives>
class Elements{
	std::vector<unsigned int> indices;
	Primitives P;
	
	public:
	Elements(const std::vector<unsigned int>& in): indices{in}, P{in.size()}{}

	size_t size() const{ return P.size(); }

	template<typename Vertex>
	auto assemble(unsigned int i, const std::vector<Vertex>& V) const{
		return assemble(P.assemble(i, indices), V);
	}
	
	template<typename Vertex>
	Line<Vertex> assemble(Line<unsigned int> indices, const std::vector<Vertex>& V) const{
		return { V[indices[0]], V[indices[1]] };
	}
	
	template<typename Vertex>
	Triangle<Vertex> assemble(Triangle<unsigned int> indices, const std::vector<Vertex>& V) const{
		return { V[indices[0]], V[indices[1]], V[indices[2]] };
	}

};

///////////////////////////////////////////////////////////////////////
template<class Primitives>
class TriLines{
	Primitives P;
	public:

	template<class... Args>
	TriLines(Args&&... args) : P{std::forward<Args>(args)...}{}

	size_t size() const{ return 3*P.size(); }

	template<typename Vertex>
	Line<Vertex> assemble(unsigned int i, const std::vector<Vertex>& V) const{
		unsigned j = i/3;
		unsigned k = i%3;

		Triangle<Vertex> tri = P.assemble(j, V);

		return { tri[k], tri[(k+1)%3] };
	}
};

#endif
