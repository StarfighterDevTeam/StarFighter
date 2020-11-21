#ifndef FILE_LOADER_UTILS_H_INCLUDED
#define FILE_LOADER_UTILS_H_INCLUDED

#include "Globals.h"

#include <vector>
#include <string>
using namespace std;

class FileLoaderUtils
{
public:
	static vector<vector<string> >* FileLoader(string name){
		vector<vector<string> >* fileConfig = new vector<vector<string> >;

		std::ifstream  data(makePath(name));

		std::string line;
		while(std::getline(data,line))
		{

			std::stringstream  lineStream(line);
			std::string        cell;

			std::vector<string> parameters;
			while(std::getline(lineStream,cell,','))
			{
				if(cell[0] == '#')
					goto loop_end;

				parameters.push_back(cell);
			}

			//TODO: check if not empty
			fileConfig->push_back(parameters);
	loop_end:;
		}
		data.close();

		return fileConfig;
	}
};

#endif// FILE_LOADER_UTILS_H_INCLUDED