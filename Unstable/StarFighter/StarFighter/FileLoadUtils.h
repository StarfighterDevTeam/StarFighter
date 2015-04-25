#ifndef FILE_LOADER_UTILS_H_INCLUDED
#define FILE_LOADER_UTILS_H_INCLUDED

#include <vector>
#include <string>
using namespace std;

#define CONFIG_FILE_SEPARATOR	','
#define CONFIG_FILE_COMMENT		'#'

class FileLoaderUtils
{
public:
	static vector<vector<string>>* FileLoader(string name){
		vector<vector<string>>* fileConfig = new vector<vector<string>>;

		std::ifstream  data(name);

		std::string line;
		while (std::getline(data, line))
		{

			std::stringstream  lineStream(line);
			std::string        cell;

			std::vector<string> parameters;
			while (std::getline(lineStream, cell, CONFIG_FILE_SEPARATOR))
			{
				if (cell[0] == CONFIG_FILE_COMMENT)
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

	static void FileWriter(string fileName, vector<vector<string>>* inputData){

		ofstream data(fileName.c_str(), ios::in | ios::trunc);
		if (data)  // si l'ouverture a réussi
		{
			// lignes
			for (vector<vector<string>>::iterator line = inputData->begin(); line != inputData->end(); line++)
			{
				//cellules
				for (vector<string>::iterator cell = line->begin(); cell != line->end(); cell++)
				{
					data << *cell;
					if (cell != line->end())
					{
						data << CONFIG_FILE_SEPARATOR;
					}
				}
				if (line != inputData->end())
				{
					data << endl;
				}
			}
			data.close();  // on ferme le fichier
		}
		else  // si l'ouverture a échoué
		{
			cerr << "Failed to open PLAYER SAVE FILE !" << endl;
		}
	}
};

#endif// FILE_LOADER_UTILS_H_INCLUDED