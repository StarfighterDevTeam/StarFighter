#ifndef WEATHER_H_INCLUDED
#define WEATHER_H_INCLUDED

#include "Game.h"

#define MAX_WEATHER_FORECASTS	10

enum WeatherCondition : int
{
	Sunny,
	MostlySunny,
	Cloudy,
	VeryCloudy,
	Rainy,
	Tempest,
	Foggy,
	COUNT,
};

struct WeatherForecast
{
	WeatherForecast()
	{
		condition = WeatherCondition::Sunny;
		windStrength = 0.f;
		windDirection = 0.f;
		seaState = 0;
		hygrometry = 0.f;
	}

	WeatherForecast(WeatherCondition _condition, float _windStrength, float _windDirection, int _seaState, float _hygrometry)
	{
		condition = _condition;
		windStrength = _windStrength;
		windDirection = _windDirection;
		seaState = _seaState;
		hygrometry = _hygrometry;
	}

	WeatherCondition condition;
	float hygrometry;//0% to 100%
	float windStrength;//in knots
	float windDirection;//in degrees
	int seaState;//Douglas scale, from 0 to 9
};

class Weather
{
public:
	Weather();
	~Weather() {};

	WeatherForecast m_forecasts[MAX_WEATHER_FORECASTS];

	WeatherForecast ComputeNextForecast(WeatherForecast previousForecast);
	void Update();
};


#endif //WEATHER_H_INCLUDED