#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>

typedef struct Vertex_t {
    float x;
    float y;
    float z;
    float w; // optional argument (defaults to 0)

    Vertex_t(float x, float y) : 
        x{ x },
        y{ y },
        z{ 0 },
        w{ 1 }
    {}
    Vertex_t() : 
        x{ 0 },
        y{ 0 },
        z{ 0 },
        w{ 1 }
    {}
    Vertex_t(float x, float y, float z) :
        x{ x },
        y{ y },
        z{ z },
        w{ 1 }
    {}
    Vertex_t(float x, float y, float z, float w) :
        x{ x },
        y{ y },
        z{ z },
        w{ w }
    {}
} Vertex;

typedef struct TextureCoord_t {
	float u;
	float v;
	float w;

	TextureCoord_t(float u) : 
		u{ u },
		v{ 0 },
		w{ 0 }
	{
		if (u < 0 || u > 1)
			throw "value for u must be in range [0, 1]";	
	}
	TextureCoord_t(float u, float v) : 
		u{ u },
		v{ v },
		w{ 0 }
	{
		if (u < 0 || u > 1)
			throw "value for u must be in range [0, 1]";	
		if (v < 0 || v > 1)
			throw "value for v must be in range [0, 1]";	
	}
	TextureCoord_t(float u, float v, float w) : 
		u{ u },
		v{ v },
		w{ w }
	{
		if (u < 0 || u > 1)
			throw "value for u must be in range [0, 1]";	
		if (v < 0 || v > 1)
			throw "value for v must be in range [0, 1]";	
		if (w < 0 || w > 1)
			throw "value for w must be in range [0, 1]";	
	}
} TextureCoord;

typedef struct Normal_t {
	float x;
	float y;
	float z;
	
	Normal_t(float x, float y, float z) :
		x{ x },
		y{ y },
		z{ z }
	{}
} Normal;

// TODO parameter space verticies

// index values of 0 for text and or normal will represent null values
// need to handle case for entries in first and third location but not second
typedef struct FaceTuple_t {
	int vertex;
	int texture;
	int normal;

	FaceTuple_t(int vertex) : 
		vertex{ vertex },
		texture{ 0 },
		normal{ 0 }
	{}
	FaceTuple_t(int vertex, int texture) : 
		vertex{ vertex },
		texture{ texture },
		normal{ 0 }
	{}
	FaceTuple_t(int vertex, int texture, int normal) : 
		vertex{ vertex },
		texture{ texture },
		normal{ normal }
	{}
} FaceTuple;

class Model 
{
private:
    std::vector<Vertex> verticies;
public:
    Model(std::string filename);
};

namespace ModelParsing
{
	Vertex parse_vertex(const std::string &line);
	FaceTuple parse_face_tuple(const std::string &line);
	std::vector<std::string> split_string(const std::string &str);
}

#endif
