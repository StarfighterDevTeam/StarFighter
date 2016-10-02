#include "Globals.h"

// *** Linking dependencies ***
#pragma comment(lib, "..\\StarEngine\\SFML-2.4.0\\lib\\flac.lib")
#pragma comment(lib, "..\\StarEngine\\SFML-2.4.0\\lib\\freetype.lib")
#pragma comment(lib, "..\\StarEngine\\SFML-2.4.0\\lib\\jpeg.lib")
#pragma comment(lib, "..\\StarEngine\\SFML-2.4.0\\lib\\ogg.lib")
#pragma comment(lib, "..\\StarEngine\\SFML-2.4.0\\lib\\openal32.lib")
#pragma comment(lib, "..\\StarEngine\\SFML-2.4.0\\lib\\vorbis.lib")
#pragma comment(lib, "..\\StarEngine\\SFML-2.4.0\\lib\\vorbisenc.lib")
#pragma comment(lib, "..\\StarEngine\\SFML-2.4.0\\lib\\vorbisfile.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "winmm.lib")

#ifdef NDEBUG
	#pragma comment(lib, "..\\StarEngine\\SFML-2.4.0\\lib\\sfml-audio-s.lib")
	#pragma comment(lib, "..\\StarEngine\\SFML-2.4.0\\lib\\sfml-graphics-s.lib")
	#pragma comment(lib, "..\\StarEngine\\SFML-2.4.0\\lib\\sfml-network-s.lib")
	#pragma comment(lib, "..\\StarEngine\\SFML-2.4.0\\lib\\sfml-system-s.lib")
	#pragma comment(lib, "..\\StarEngine\\SFML-2.4.0\\lib\\sfml-window-s.lib")
#else
	#pragma comment(lib, "..\\StarEngine\\SFML-2.4.0\\lib\\sfml-audio-s-d.lib")
	#pragma comment(lib, "..\\StarEngine\\SFML-2.4.0\\lib\\sfml-graphics-s-d.lib")
	#pragma comment(lib, "..\\StarEngine\\SFML-2.4.0\\lib\\sfml-network-s-d.lib")
	#pragma comment(lib, "..\\StarEngine\\SFML-2.4.0\\lib\\sfml-system-s-d.lib")
	#pragma comment(lib, "..\\StarEngine\\SFML-2.4.0\\lib\\sfml-window-s-d.lib")
#endif

// ****************************


sf::RenderWindow *window;

//to do OPTIM
//std::unique_ptr<sf::RenderWindow> window;

std::string makePath(const std::string& srcPath)
{
#ifdef __APPLE__
	return resourcePath() + "Assets/" + srcPath;
#else
	return "Assets/" + srcPath;
#endif
}

void createSavesDirectory()
{
#ifdef __APPLE__
	char strPath[1024];
	snprintf(strPath, sizeof(strPath), "%s/Library/Application Support/StarFighter", getenv("HOME"));
	mkdir(strPath, 0755);
	snprintf(strPath, sizeof(strPath), "%s/Library/Application Support/StarFighter/Saves", getenv("HOME"));
	mkdir(strPath, 0755);
#else
	//::CreateDirectory(path, NULL);	// TODO: to be implemented on Windows
#endif
}

const char* getSavesPath()
{
#ifdef __APPLE__
	static char strPath[1024];
	snprintf(strPath, sizeof(strPath), "%s/Library/Application Support/StarFighter/Saves/", getenv("HOME"));
	return strPath;
#else
	return "";	// TODO: should use %APPDATA% / "My Games/Saves" on Windows
#endif
}

int RandomizeIntBetweenRatios(int value, sf::Vector2f min_max_values)
{
	double random_rate = ((double) rand() / RAND_MAX);
	random_rate *= (min_max_values.y - min_max_values.x);
	random_rate += min_max_values.x;
	int i = (int) ((random_rate*value) + 0.5);

	return i;
}

float RandomizeFloatBetweenRatios(float value, sf::Vector2f min_max_ratios)
{
	double random_rate = ((double) rand() / RAND_MAX);
	random_rate *= (min_max_ratios.y - min_max_ratios.x);
	random_rate += min_max_ratios.x;
	float f = (float) (random_rate*value);

	return f;
}

float ProrataBetweenThreshold(float m_value, sf::Vector2f min_max_threshold)
{
	float max = min_max_threshold.y - min_max_threshold.x;
	float value = m_value - min_max_threshold.x;
	float prorata = value / max;

	return prorata;
}

int RandomizeIntBetweenValues(int min_value, int max_value)
{
	int random_value = rand() % (max_value - min_value +1);
	random_value += min_value;

	return random_value;
}

int RandomizeIntBetweenFloats(sf::Vector2f min_max_values)
{
	int min = floor (min_max_values.x + 0.5);
	int max = floor (min_max_values.y + 0.5);
	int random_value = rand() % (max - min +1);
	random_value += min;

	return random_value;
}

float RandomizeFloatBetweenValues(sf::Vector2f min_max_values)
{
	float random_value = (float) ((double) rand() / RAND_MAX);
	random_value *= (min_max_values.y - min_max_values.x);
	random_value += min_max_values.x;

	return random_value;
}

float MaxBetweenValues(sf::Vector2f values)
{
	float max = values.x;
	if (values.y > max)
	{
		max = values.y;
	}
	return max;
}

float MinBetweenValues(sf::Vector2f values)
{
	float min = values.x;
	if (values.y < min)
	{
		min = values.y;
	}
	return min;
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to)
{
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}