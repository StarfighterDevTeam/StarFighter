#ifndef FILE_LOADER_UTILS_H_INCLUDED
#define FILE_LOADER_UTILS_H_INCLUDED

#include "Globals.h"

#include <vector>
#include <string>
using namespace std;

class FileLoaderUtils
{
public:
	static vector<vector<string> > FileLoader(string name){
		vector<vector<string> > fileConfig;

		std::ifstream  data(makePath(name));

		std::string line;
		while(std::getline(data,line))
		{

			std::stringstream  lineStream(line);
			std::string        cell;

			std::vector<string> parameters;
			bool isComment = false;
			while(std::getline(lineStream,cell,','))
			{
				if(cell[0] == '#')
				{
					isComment = true;
					break;
				}

				parameters.push_back(cell);
			}

			if(!isComment && !parameters.empty())
				fileConfig.push_back(parameters);
		}
		data.close();

		return fileConfig;
	}
};

#endif// FILE_LOADER_UTILS_H_INCLUDED