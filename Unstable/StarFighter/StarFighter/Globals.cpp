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
		min = values.y;
	
	return min;
}

float MinBetweenIntValues(int valueA, int valueB)
{
	int min = valueA;
	if (valueB < min)
		min = valueB;

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

	float ratio = (value - input_min) / (input_max - input_min);
	float cos_ratio = (1 - cos(ratio * M_PI)) / 2;
	//for stiffer curves, use "cos_ratio = (1 - cos(ratio * ratio * M_PI)) / 2;"

	float ouput = output_min + cos_ratio * (output_max - output_min);

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

float GetAngleAbsDeltaToTarget(float angle_input, float angle_target)
{
	float delta = angle_input - angle_target;

	if (delta > 180)
		delta -= 360;

	else if (delta < -180)
		delta += 360;

	return abs(delta);
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

void BoundAngle(float& input, float max_angle)
{
	while (input < 0)
		input += max_angle;

	if (input > max_angle)
		input = fmod(input, max_angle);
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

void GetAngleRadAndLengthOfVector(sf::Vector2f vector, float* output_length, float* output_angle)
{
	const float a = vector.x;
	const float b = vector.y;

	if (a == 0 && b == 0)
	{
		*output_length = 0.f;
		*output_angle = 0.f;
		return;
	}

	*output_length = (a * a) + (b * b);
	*output_length = sqrt(*output_length);

	*output_angle = acos(a / *output_length);

	if (b < 0)
	{
		*output_angle = -*output_angle;
	}

	*output_angle += M_PI_2;
}

float GetAngleRadToTargetPosition(sf::Vector2f ref_position, float ref_rotation_in_deg, sf::Vector2f target_position)
{
	float angle = GetAngleRadBetweenPositions(target_position, ref_position);
	float delta_angle = angle - (ref_rotation_in_deg * M_PI / 180.f);
	if (delta_angle > M_PI)
		delta_angle -= M_PI * 2;
	else if (delta_angle < -M_PI)
		delta_angle += M_PI * 2;

	return delta_angle;
}

float GetAngleDegToTargetPosition(sf::Vector2f ref_position, float ref_rotation_in_deg, sf::Vector2f target_position)
{
	float angle = GetAngleRadBetweenPositions(target_position, ref_position) * 180.f / M_PI;
	float delta_angle = angle - ref_rotation_in_deg;
	if (delta_angle > 180)
		delta_angle -= 180.f * 2;
	else if (delta_angle < -180)
		delta_angle += 180.f * 2;

	return delta_angle;
}

float GetAngleRadForVector(sf::Vector2f vector)
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

	angle += M_PI_2;
	//angle = (fmod(angle, 2 * M_PI));

	return angle;
}

sf::Vector2f RotateVector(sf::Vector2f vector, float angle_rad)
{
	float length;
	float angle;
	GetAngleRadAndLengthOfVector(vector, &length, &angle);

	angle += angle_rad;

	sf::Vector2f output_vector = GetVectorFromLengthAndAngle(length, angle);

	return output_vector;
}

void SetVectorRotation(sf::Vector2f* vector, float angle_rad)
{
	float length = GetVectorLength(*vector);
	sf::Vector2f new_vector = GetVectorFromLengthAndAngle(length, angle_rad);
}
sf::Vector2f GetVectorFromLengthAndAngle(float absolute_speed, float curAngle)
{
	sf::Vector2f speed;
	speed.x = -absolute_speed * sin(curAngle);
	speed.y = absolute_speed * cos(curAngle);

	return speed;
}

float GetAngleRadBetweenPositions(sf::Vector2f ref_position, sf::Vector2f position2)
{
	const sf::Vector2f diff = sf::Vector2f(ref_position.x - position2.x, ref_position.y - position2.y);
	float target_angle = GetAngleRadForVector(diff);

	const float a = diff.x;
	const float b = diff.y;

	float distance_to_obj = (a * a) + (b * b);
	distance_to_obj = sqrt(distance_to_obj);

	float angle;
	angle = acos(a / distance_to_obj);

	if (b < 0)
	{
		angle = -angle;
	}

	angle += M_PI_2;

	return angle;
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

bool BoundVector(sf::Vector2f* vector, float min_value, float max_value)
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
	else if (vector->x * vector->x + vector->y * vector->y < min_value * min_value)
	{
		float p = min_value / sqrt(vector->x * vector->x + vector->y * vector->y);
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

void AddValueToVector(sf::Vector2f* vector, float added_value)
{
	if (vector->x == 0 && vector->y == 0)
		return;

	float target_value = GetVectorLength(*vector) + added_value;
	float p = target_value / sqrt(vector->x * vector->x + vector->y * vector->y);
	vector->x *= p;
	vector->y *= p;
}

bool IntersectSegments(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
	//segment 1: [p0, p1], segment 2: [p2, p3]
	float s1_x, s1_y, s2_x, s2_y;
	s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
	s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

	float s, t;
	s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
	t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
	{
		// Collision detected
		if (i_x != NULL)
			*i_x = p0_x + (t * s1_x);
		if (i_y != NULL)
			*i_y = p0_y + (t * s1_y);
		return true;
	}

	return false; // No collision
}

float DistancePointToSement(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float *i_x, float *i_y)
{
	//distance of point p0_ to segment [p1_, p2_]
	const float px = p2_x - p1_x;
	const float py = p2_y - p1_y;
	float u = ((p0_x - p1_x)*px + (p0_y - p1_y)*py) / (px*px + py*py);
	u = (u > 1) ? 1 : (u < 0) ? 0 : u;
	const float x = p1_x + u*px;
	const float y = p1_y + u*py;
	const float dx = x - p0_x;
	const float dy = y - p0_y;
	const float dist = sqrt(dx*dx + dy*dy);

	// Orthogonal projected point
	if (i_x != NULL)
		*i_x = x;
	if (i_y != NULL)
		*i_y = y;

	return dist;
}


int GetPixelDistanceFromEdge(int pixel_index, int width, int height)
{
	int line = (pixel_index / 4 / width);
	int row = (pixel_index / 4) % width;

	int x_ = row;
	if (width - row - 1 < x_)
		x_ = width - row;

	int y_ = line;
	if (height - line - 1 < y_)
		y_ = height - line;

	int distance = x_;
	if (y_ < distance)
		distance = y_;

	distance++;//min value to return is 1
	return distance;
}

void GlowEffect(int blur_radius, sf::Uint8* pixels, int width, int height, int stroke_size)
{
	if (blur_radius > 0)
	{
		for (int i = 0; i < width * height * 4; i += 4)
		{
			if ((i / 4) <= width * blur_radius || (i / 4) >(height - 1 * blur_radius)*width || (i / 4) % width <= 0 + (blur_radius - 1) || (i / 4) % width >= (width - 1 * blur_radius))
			{
				int nominator = GaussianBlurDistribution(GetPixelDistanceFromEdge(i, width, height));
				int denominator = GaussianBlurDistribution(blur_radius + 1);
				pixels[i + 3] = (Uint8)(ceil(1.f * nominator / denominator * 255)); // Alpha
			}
			else if ((i / 4) <= width * (blur_radius + stroke_size) || (i / 4) >(height - 1 * (blur_radius + stroke_size))*width || (i / 4) % width <= 0 + ((blur_radius + stroke_size) - 1) || (i / 4) % width >= (width - 1 * (blur_radius + stroke_size)))
			{
				//stroke: do nothing
			}
			else if ((i / 4) <= width * (2 * blur_radius + stroke_size) || (i / 4) >(height - 1 * (2 * blur_radius + stroke_size))*width || (i / 4) % width <= 0 + ((2 * blur_radius + stroke_size) - 1) || (i / 4) % width >= (width - 1 * (2 * blur_radius + stroke_size)))
			{
				int nominator = GaussianBlurDistribution((blur_radius - 1) - (GetPixelDistanceFromEdge(i, width, height) - stroke_size - blur_radius - 1));
				int denominator = GaussianBlurDistribution(blur_radius + 1);
				pixels[i + 3] = (Uint8)(ceil(1.f * nominator / denominator * 255)); // Alpha
			}
		}
	}
}

int GaussianBlurDistribution(int x)
{
	return x*x;
}

sf::Uint8* CreateRectangleWithStroke(sf::Vector2f size, sf::Color color, int stroke_size)
{
	//pixel array creation
	const int W = size.x;
	const int H = size.y;

	sf::Uint8* pixels = new sf::Uint8[W * H * 4];

	for (int i = 0; i < W * H * 4; i += 4)
	{
		pixels[i] = color.r;			// R
		pixels[i + 1] = color.g;		// G
		pixels[i + 2] = color.b;	// B
		pixels[i + 3] = 255;
	}
	//inside stroke
	for (int i = 0; i < W * H * 4; i += 4)
	{
		if (stroke_size < 1 || (i / 4) <= W * (stroke_size) || (i / 4) >(H - 1 * (stroke_size))*W || (i / 4) % W <= 0 + ((stroke_size)-1) || (i / 4) % W >= (W - 1 * (stroke_size))) // A
		{
			pixels[i + 3] = 255;
		}
		else
		{
			pixels[i + 3] = RECTANGLE_INSIDE_ALPHA;
		}
	}

	return pixels;
}

sf::Uint8* CreateCircleWithStroke(float radius, sf::Color color, int stroke_size)
{
	//pixel array creation
	const int W = radius * 2;
	const int H = radius * 2;

	sf::Uint8* pixels = new sf::Uint8[W * H * 4];

	for (int i = 0; i < W * H * 4; i += 4)
	{
		int x = (i / 4) % W;
		int y = (i / 4) / W;
		int dx = W / 2 - x;
		int dy = H / 2 - y;
		float delta = sqrt(dx * dx + dy * dy);// -radius * radius;

		if (stroke_size > 0 && delta <= radius - stroke_size)
		{
			pixels[i] = color.r;						// R
			pixels[i + 1] = color.g;					// G
			pixels[i + 2] = color.b;					// B
			pixels[i + 3] = RECTANGLE_INSIDE_ALPHA;		// A
		}
		else if (delta <= radius)
		{
			pixels[i] = color.r;			// R
			pixels[i + 1] = color.g;		// G
			pixels[i + 2] = color.b;		// B
			pixels[i + 3] = 255;			// A
		}
		else
		{
			pixels[i] = color.r;			// R
			pixels[i + 1] = color.g;		// G
			pixels[i + 2] = color.b;		// B
			pixels[i + 3] = 0;				// A
		}
	}

	return pixels;
}

bool IsInsideArea(sf::Vector2f bounds, sf::Vector2f coordinates, sf::Vector2f area_size)
{
	const float a = coordinates.x - bounds.x / 2;
	const float b = coordinates.x + bounds.x / 2;

	const float x = area_size.x;

	const float c = coordinates.y - bounds.y / 2;
	const float d = coordinates.y + bounds.y / 2;

	const float y = area_size.y;

	bool is_inside_area = true;
	if (a < 0 || b > x || c < 0 || d > y)
	{
		is_inside_area = false;
	}

	return is_inside_area;
}