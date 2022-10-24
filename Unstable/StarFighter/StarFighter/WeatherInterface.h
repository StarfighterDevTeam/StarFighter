#ifndef WEATHER_INTERFACE_H_INCLUDED
#define WEATHER_INTERFACE_H_INCLUDED

#include "Game.h"
#include "GameEntity.h"
#include "Weather.h"

struct WeatherInterface
{
public:
	WeatherInterface();
	~WeatherInterface();

	GameEntity* m_panel;
	GameEntity* m_conditions[MAX_WEATHER_FORECASTS];
	GameEntity* m_winds[MAX_WEATHER_FORECASTS];

	void Init(const Weather* weather);
	void Destroy();

	void Update();
	void Draw(sf::RenderTexture& screen);

private:
	const Weather* m_weather;
};

#endif //WEATHER_INTERFACE_H_INCLUDED