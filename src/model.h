#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include "vector.h"

typedef struct TextureCoord_t {
    float u;
    float v;
    float w;

    TextureCoord_t(float u) : u{u}, v{0}, w{0} {
        if (u < 0 || u > 1)
            throw "value for u must be in range [0, 1]";
    }
    TextureCoord_t(float u, float v) : u{u}, v{v}, w{0} {
        if (u < 0 || u > 1)
            throw "value for u must be in range [0, 1]";
        if (v < 0 || v > 1)
            throw "value for v must be in range [0, 1]";
    }
    TextureCoord_t(float u, float v, float w) : u{u}, v{v}, w{w} {
        if (u < 0 || u > 1)
            throw "value for u must be in range [0, 1]";
        if (v < 0 || v > 1)
            throw "value for v must be in range [0, 1]";
        if (w < 0 || w > 1)
            throw "value for w must be in range [0, 1]";
    }
} TextureCoord;

// index values of 0 for text and or normal will represent null values
// need to handle case for entries in first and third location but not second
typedef struct FaceTuple_t {
    int vertex;
    int texture;
    int normal;

    FaceTuple_t(int vertex) : vertex{vertex}, texture{0}, normal{0} {}
    FaceTuple_t(int vertex, int texture)
        : vertex{vertex}, texture{texture}, normal{0} {}
    FaceTuple_t(int vertex, int texture, int normal)
        : vertex{vertex}, texture{texture}, normal{normal} {}
} FaceTuple;

class Model {
   private:
    std::vector<Vector<4>> verticies;
    std::vector<std::vector<FaceTuple>> faces;
    std::vector<Vector<4>> normals;

   public:
    Model(std::string filename);
    int nfaces() const;
    Vector<4> vertex(int i) const;
    Vector<4> normal(int i) const;
    std::vector<FaceTuple> face(int i) const;
};

namespace ModelParsing {
Vector<4> parse_vector(const std::string& line);
std::vector<FaceTuple> parse_face(const std::string& line);
FaceTuple parse_face_tuple(const std::string& line);
std::vector<std::string> split_string(const std::string& str);
}  // namespace ModelParsing

#endif
