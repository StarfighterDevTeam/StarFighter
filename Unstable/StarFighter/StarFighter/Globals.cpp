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

int RandomizeIntBetweenFloats(float min_value, float max_value)
{
	int min = (int)floor(min_value + 0.5);
	int max = (int)floor(max_value + 0.5);
	int random_value = rand() % (max - min + 1);
	random_value += min;

	return random_value;
}

float RandomizeFloatBetweenValues(float min_value, float max_value)
{
	float random_value = (float) ((double) rand() / RAND_MAX);
	random_value *= (max_value - min_value);
	random_value += min_value;

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

float MaxBetweenValues(float x, float y)
{
	if (y > x)
		return y;
	else
		return x;
}

float MinBetweenValues(float x, float y)
{
	if (y < x)
		return y;
	else
		return x;
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

	float ratio = (1.f - cos(value * M_PI_F)) * 0.5f;
	//for stiffer curves, use "ratio = (1 - cos(value * value * M_PI_F)) * 0.5f;"

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

float GetAngleDegToTargetPosition(sf::Vector2f ref_position, float ref_rotation_in_deg, sf::Vector2f target_position)
{
	float angle = GetAngleRadFromVector(sf::Vector2f(target_position.x - ref_position.x, target_position.y - ref_position.y)) * 180.f / M_PI_F;
	float delta_angle = ref_rotation_in_deg - angle;
	if (delta_angle > 180)
		delta_angle -= 180.f * 2;
	else if (delta_angle < -180)
		delta_angle += 180.f * 2;

	return delta_angle;
}

float GetAngleRadFromVector(sf::Vector2f vector)
{
	const float a = vector.x;
	const float b = vector.y;

	if (a == 0 && b == 0)
		return 0;

	float distance_to_obj = (a * a) + (b * b);
	distance_to_obj = sqrt(distance_to_obj);

	float angle = acos(a / distance_to_obj);

	if (b > 0)
	{
		angle = -angle;
	}

	angle += M_PI_2_F;

	return angle;//PI = down, PI/2 = right, 0=up
}

bool NormalizeVector(sf::Vector2f* vector, float max_value)
{
	if (vector->x == 0 && vector->y == 0)
		return true;

	if (vector->x * vector->x + vector->y * vector->y > max_value * max_value)
	{
		float p = max_value / sqrt(vector->x * vector->x + vector->y * vector->y);
		vector->x *= p;
		vector->y *= p;

		return true;
	}
	
	return false;
}

void ScaleVector(sf::Vector2f* vector, float target_value)
{
	if (vector->x == 0 && vector->y == 0)
		return;

	float p = target_value / sqrt(vector->x * vector->x + vector->y * vector->y);
	vector->x *= p;
	vector->y *= p;
}

void ScaleVectorInt(sf::Vector2i* vector, int target_value)
{
	if (vector->x == 0 && vector->y == 0)
		return;

	float p = 1.f * target_value / (float)sqrt(vector->x * vector->x + vector->y * vector->y);
	vector->x = (int)(vector->x * p);
	vector->y = (int)(vector->y * p);
}

float GetDistanceBetweenPositions(sf::Vector2f position1, sf::Vector2f position2)
{
	Vector2f current_diff = sf::Vector2f(position1.x - position2.x, position1.y - position2.y);
	return GetVectorLength(current_diff);
}

float GetDistanceSquaredBetweenPositions(sf::Vector2f position1, sf::Vector2f position2)
{
	Vector2f current_diff = sf::Vector2f(position1.x - position2.x, position1.y - position2.y);
	return GetVectorLengthSquared(current_diff);
}

float GetVectorLength(sf::Vector2f vector)
{
	const float a = vector.x;
	const float b = vector.y;
	float s = (a * a) + (b * b);
	s = sqrt(s);
	s = floor(s);
	return s;
}

float GetVectorLengthSquared(sf::Vector2f vector)
{
	const float a = vector.x;
	const float b = vector.y;
	float s = (a * a) + (b * b);

	return s;
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

void BoundAbsoluteValue(float& input, float max_value)
{
	if (input < 0)
	{
		if (input < - max_value)
			input = max_value;
	}
	else
	{
		if (input > max_value)
			input = max_value;
	}
}

void BoundAngle(float& input, float max_angle)
{
	while (input < 0)
		input += max_angle;

	if (input > max_angle)
		input = fmod(input, max_angle);
}

bool IsInsideArea(sf::Vector2f bounds, sf::Vector2f coordinates, sf::Vector2f area_size)
{
	const float a = coordinates.x + bounds.x * 0.5f;
	const float b = coordinates.x - bounds.x * 0.5f;

	const float x = area_size.x;

	const float c = coordinates.y + bounds.y * 0.5f;
	const float d = coordinates.y - bounds.y * 0.5f;

	const float y = area_size.y;

	bool is_inside_area = true;
	if (a < 0 || b > x || c < 0 || d > y)
		is_inside_area = false;

	return is_inside_area;
}

sf::Vector2f GetVectorFromLengthAndAngle(const float length, const float angle)
{
	sf::Vector2f vector;
	vector.x = -length * sin(angle);
	vector.y = -length * cos(angle);
	return vector;
}

void DebugDrawSegment(sf::Vector2f point_a, sf::Vector2f point_b, sf::Color color, RenderTarget& screen)
{
	sf::Vertex line[2];

	line[0].position = point_a;
	line[1].position = point_b;

	line[0].color = color;
	line[1].color = color;

	screen.draw(line, 2, sf::Lines);
}

float computeDeltaAngleInDegrees(const float angle1, const float angle2)
{
	const float deltaAngle = angle2 - angle1;
	if (deltaAngle >= 180.f)
		return deltaAngle - 360.f;
	else if (deltaAngle <= -180.f)
		return deltaAngle + 360.f;
	else
		return deltaAngle;
}