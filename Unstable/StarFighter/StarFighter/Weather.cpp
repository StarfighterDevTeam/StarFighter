#include "Weather.h"
#include <random>

extern Game* CurrentGame;

#define MAX_WIND_STRENGTH					180.f
#define MIN_HYGROMETRY_INCREMENTATION		0.f
#define MAX_HYGROMETRY_INCREMENTATION		0.1f
#define MIN_HYGROMETRY_LOSS_AFTER_RAIN		0.05f
#define MAX_HYGROMETRY_LOSS_AFTER_RAIN		0.3f

Weather::Weather()
{
	m_forecasts[0] = WeatherForecast();

	for (int i = 1; i < MAX_WEATHER_FORECASTS; i++)
		m_forecasts[i] = ComputeNextForecast(m_forecasts[i - 1]);
}

void Weather::Update()
{
	for (int i = 1; i < MAX_WEATHER_FORECASTS; i++)
		m_forecasts[i - 1] = m_forecasts[i];

	m_forecasts[MAX_WEATHER_FORECASTS - 1] = ComputeNextForecast(m_forecasts[MAX_WEATHER_FORECASTS - 2]);
}

WeatherForecast Weather::ComputeNextForecast(WeatherForecast previousForecast)
{
	float windDirection = RandomizeFloatBetweenValues(0, 359);
	float windStrength = RandomizeFloatBetweenValues(0, MAX_WIND_STRENGTH);
	int seaState = (int)Lerp(windStrength, 0, MAX_WIND_STRENGTH, 0, 9);

	//hygrometry
	WeatherCondition condition = WeatherCondition::Sunny;
	float hygrometry = previousForecast.hygrometry + RandomizeFloatBetweenValues(MIN_HYGROMETRY_INCREMENTATION, MAX_HYGROMETRY_INCREMENTATION);
	if (previousForecast.condition == Tempest || previousForecast.condition == Foggy)
		hygrometry -= RandomizeFloatBetweenValues(MIN_HYGROMETRY_LOSS_AFTER_RAIN, MAX_HYGROMETRY_LOSS_AFTER_RAIN);
	else if (previousForecast.condition == Rainy)
		hygrometry -= MIN_HYGROMETRY_LOSS_AFTER_RAIN;

	Boundf(hygrometry, 0.f, 1.f);

	//weather condition
	if (hygrometry < 0.3f)
		condition = WeatherCondition::Sunny;
	else if (hygrometry < 0.5f)
		condition = WeatherCondition::MostlySunny;
	else if (hygrometry < 0.7f)
		condition = WeatherCondition::Cloudy;
	else if (hygrometry < 0.9f)
		condition = WeatherCondition::Rainy;
	else if (windStrength < 50.f)
		condition = WeatherCondition::Foggy;
	else
		condition = WeatherCondition::Tempest;

	return WeatherForecast(condition, windStrength, windDirection, seaState, hygrometry);
}