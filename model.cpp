#include "model.h"

#include <cstddef>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>

Model::Model(std::string filename) :
	verticies{},
	faces{}
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

		// switch this to split line soon
		switch (line[0]) {
			case 'v':
				verticies.push_back(ModelParsing::parse_vertex(line));
				break;
			case 'f':
				faces.push_back(ModelParsing::parse_face(line));
				break;
			default:
				break;		
		}
	}
}

int Model::nfaces() const
{
	return faces.size();
}

Vector<3> Model::vertex(int i) const
{
	return verticies[i];
}

std::vector<FaceTuple> Model::face(int i) const
{
	return faces[i];
}

// given a string of input get the vertex value
Vector<3> ModelParsing::parse_vertex(const std::string &line)
{
	// ignoring w entry for simplicity
	std::vector<std::string> split_strs { ModelParsing::split_string(line) };
	return {
		std::stof(split_strs[1]),	// don't count 'v' char
		std::stof(split_strs[2]),
		std::stof(split_strs[3])
	};
}

std::vector<FaceTuple> ModelParsing::parse_face(const std::string &line)
{
	std::vector<std::string> face_tuple_strings = split_string(line);
	std::vector<FaceTuple> faces{};
	if (face_tuple_strings.size() < 3) {
		throw "faces must have at least 3 verticies";
	}
	for (int i = 1; i < face_tuple_strings.size(); i++) {
		faces.push_back(ModelParsing::parse_face_tuple(face_tuple_strings[i]));
	}
	return faces;	
}

// rename these variables they aren't great
FaceTuple ModelParsing::parse_face_tuple(const std::string &line)
{
	unsigned long start{ 0 };
	unsigned long stop{ line.find('/') };
	int u{ 0 };

	if (stop == -1) {
		u = std::stoi(line.substr(start, line.length()));
		return { u };
	} else {
		u = std::stoi(line.substr(start, stop - start));	
	}

	start = stop + 1;
	stop = line.find('/', start);
	int v{ 0 };	
	if (stop == -1) {
		v = std::stoi(line.substr(start, line.length()));
		return { u, v };
	} else {
		if (stop - start == 0) // in case of no second argument i.e. u//w
			v = 0;
		else
			v = std::stoi(line.substr(start, stop - start));	
	}

	int w{ 0 };
	start = stop + 1;
	w = std::stoi(line.substr(start, line.length()));
	return { u, v, w };
}

// splits string at space characters
std::vector<std::string> ModelParsing::split_string(const std::string &str)
{
	std::vector<std::string> strings {};	
	int start_idx { 0 };
	int len { 0 };
	bool in_string { false };
	for (int i = 0; i < str.length(); i++) {
		if (str[i] == ' ') {
			if (in_string) { // cut off the string and add to vector
				strings.push_back(str.substr(start_idx, len));	
				len = 0;
				in_string = false;
			}
		} else { // non white=space char
			if (!in_string) {
				in_string = true;
				start_idx = i;
			}
			len++;
		}
	}

	if (in_string)
		strings.push_back(str.substr(start_idx, len));

	return strings;
}
