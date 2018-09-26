#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include <stdlib.h>     /* srand, rand */
#define _USE_MATH_DEFINES
#include <math.h>

int RandomizeIntBetweenRatios(int value, float min_value, float max_value);
float RandomizeFloatBetweenRatios(float value, float min_ratio, float max_ratio);
float ProrataBetweenThreshold(float m_value, float min_threshold, float max_threshold);
int RandomizeIntBetweenValues(int min_value, int max_value);
int RandomizeIntBetweenFloats(float min_value, float max_value);
float RandomizeFloatBetweenValues(float min_value, float max_value);
int RandomizeSign();
float MaxBetweenValues(float x, float y);
float MinBetweenValues(float x, float y);
float Lerp(float value, float input_min, float input_max, float output_min, float output_max);
float Bound(float value, float output_min, float output_max);
float CosInterpolation(float value, float input_min, float input_max, float output_min, float output_max);

#endif // GLOBALS_H_INCLUDED