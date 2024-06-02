#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include "renderer.h"


class Model 
{
private:
    std::vector<Point> verticies;
public:
    Model(std::string filename);
};

namespace ModelParsing
{
    bool is_vertex(std::string line);
    Point parse_vertex(std::string line);
}

#endif
