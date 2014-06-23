#include "PatternBobby.h"

PatternBobby::PatternBobby()
{
	this->currentPattern = PatternType::NoMovePattern;
}

void PatternBobby::SetPattern(PatternType pt, int patternSpeed, vector<int>*  args)
{
	this->currentPattern = pt;
	this->patternParams = args;
	this->patternSpeed = patternSpeed;

	switch(this->currentPattern)
	{
	case PatternType::Rectangle_:
		{
			_distance_left = args->at(0);
			_direction = sf::Vector2i(1,0);
			break;
		}
	}
}


sf::Vector2f  PatternBobby::GetOffset(float seconds)
{
	static sf::Vector2f offset;

	switch(this->currentPattern)
	{
	case PatternType::NoMovePattern:
		{
			offset.x=0;
			offset.y=0;
			break;
		}
	case PatternType::Rectangle_:
		{
			//ARGS 
			// 0 = longueur
			// 1 = largeur

			//just move on the line
			offset.x = _direction.x*patternSpeed*seconds;
			offset.y = _direction.y*patternSpeed*seconds;
			float moved= abs(offset.x) + abs(offset.y);

			if(_distance_left > moved)
			{
				//Moving on the edge, like a boss
				_distance_left -= moved;
			}else
			{

				offset.x = _direction.x*_distance_left;
				offset.y = _direction.y*_distance_left;

				//Changing direction
				if(abs(_direction.x) == 1)
				{
					_direction.y = _direction.x;
					_direction.x = 0;
				}else
				{
					_direction.x = -_direction.y;
					_direction.y = 0;
				}

				if(_distance_left < moved)
				{
					offset.x += _direction.x*abs(_distance_left-moved);
					offset.y += _direction.y*abs(_distance_left-moved);

					_distance_left = (abs(_direction.x==1) ? patternParams->at(0) : patternParams->at(1)) - abs(_distance_left-moved);
				}else
				{
					//longueur ou largeur ?
					_distance_left = abs(_direction.x==1) ? patternParams->at(0) : patternParams->at(1);
				}				
			}
			break;
		}


	default:
		{
			throw invalid_argument(TextUtils::format("Game error: Unknow pattern # '%d'", this->currentPattern));
		}
	}

	return offset;
}