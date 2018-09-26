#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include <stdlib.h>     /* srand, rand */
#define _USE_MATH_DEFINES
#include <math.h>

int RandomizeIntBetweenRatios(int value, double min_value, double max_value);
double RandomizeFloatBetweenRatios(double value, double min_ratio, double max_ratio);
double ProrataBetweenThreshold(double m_value, double min_threshold, double max_threshold);
int RandomizeIntBetweenValues(int min_value, int max_value);
int RandomizeIntBetweenFloats(double min_value, double max_value);
double RandomizeFloatBetweenValues(double min_value, double max_value);
int RandomizeSign();
double MaxBetweenValues(double x, double y);
double MinBetweenValues(double x, double y);
double Lerp(double value, double input_min, double input_max, double output_min, double output_max);
double Bound(double value, double output_min, double output_max);
double CosInterpolation(double value, double input_min, double input_max, double output_min, double output_max);

#endif // GLOBALS_H_INCLUDED