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

    std::string strInput{};
    while (inf >> strInput)
        std::cout << strInput << "\n";
}
