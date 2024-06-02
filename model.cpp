#include "model.h"

#include <vector>
#include <fstream>
#include <iostream>
#include <string>

Model::Model(std::string filename) 
    : verticies{}    
{
    std::ifstream inf{ filename }; 

    if (!inf)
    {
        std::cerr << "Could not open file.\n";
        return;
    }

    for (std::string line{}; std::getline(inf, line);) {
        if (line.length() == 0)
            continue;

        switch (line[0]) {
        case 'v':
            break;
        
        }
    }
}

