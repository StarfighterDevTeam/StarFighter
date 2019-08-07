#include "Globals.h"

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

int RandomizeSign()
{
	//50% chance of being 1, 50% chance of being -1
	int sign = 1;

	int random_value = RandomizeIntBetweenValues(1, 2);
	if (random_value > 1)
	{
		sign = -1;
	}

	return sign;
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

std::string StringCut(std::string str, const size_t length)
{
	size_t current_length = length;
	size_t current_cut = 0;
	size_t str_size = str.size();
	while (str_size - current_cut > length)
	{
		for (size_t i = current_cut + length; i > 0; i--)
		{
			if (str[i] == ' ')
			{
				str[i] = '\n';
				current_cut = i;
				current_length = str_size - current_cut;
				break;
			}
		}
	}

	return str;
}

float Lerp(float value, float input_min, float input_max, float output_min, float output_max)
{
	if (input_min == input_max)
		return output_max;

	if (value < input_min)
		return output_min;

	if (value > input_max)
		return output_max;

	float ratio = (value - input_min) / (input_max - input_min);

	float lerp = output_min + ratio * (output_max - output_min);

	return lerp;
}

float CosInterpolation(float value, float input_min, float input_max, float output_min, float output_max)
{
	if (input_min == input_max)
		return output_max;

	if (value < input_min)
		return output_min;

	if (value > input_max)
		return output_max;

	float ratio = (1 - cos(value * M_PI)) / 2;
	//for stiffer curves, use "ratio = (1 - cos(value * value * M_PI)) / 2;"

	float ouput = output_min + ratio * (output_max - output_min);

	return ouput;
}

sf::Color GrayScaleColor(sf::Color input_color, float ratio)
{
	Uint8 r = input_color.r;
	Uint8 g = input_color.g;
	Uint8 b = input_color.b;

	Uint8 avg = (r + g + b) / 3;

	Uint8 r2 = Uint8(avg * ratio + r * (1 - ratio));
	Uint8 g2 = Uint8(avg * ratio + g * (1 - ratio));
	Uint8 b2 = Uint8(avg * ratio + b * (1 - ratio));

	return sf::Color(r2, g2, b2, input_color.a);
}

void ScaleVector(sf::Vector2f* vector, float target_value)
{
	if (vector->x == 0 && vector->y == 0)
		return;

	float p = target_value / sqrt(vector->x * vector->x + vector->y * vector->y);
	vector->x *= p;
	vector->y *= p;
}

float GetAngleForVector(sf::Vector2f vector)
{
	const float a = vector.x;
	const float b = vector.y;

	if (a == 0 && b == 0)
		return 0.f;

	float distance_to_obj = (a * a) + (b * b);
	distance_to_obj = sqrt(distance_to_obj);

	float angle;
	angle = acos(a / distance_to_obj);

	if (b < 0)
	{
		angle = -angle;
	}

	return angle * 180.f / M_PI;
}


bool IsInsideAngleCoverage(float input, float angle_coverage, float angle_target)
{
	float delta = input - angle_target;

	if (delta > 180)
		delta -= 360;

	else if (delta < -180)
		delta += 360;

	return abs(delta) <= angle_coverage / 2;
}

void Bound(float& input, float min, float max)
{
	if (min > max)
	{
		float _min = min;
		min = max;
		max = _min;
	}

	if (input < min)
		input = min;

	if (input > max)
		input = max;
}