#include "PatternBobby.h"

PatternBobby::PatternBobby()
{
	this->currentPattern = PatternType::NoMovePattern;
}

PatternType PatternBobby::GetCurrentPatternType()
{
	return this->currentPattern;
}

void PatternBobby::SetPattern(PatternType pt, float patternSpeed, vector<float>*  args)
{
	//Note that patternSpeed is 
	// - px/sec on the canvas for Rectangle
	// - time to do 360° for the circle

	this->currentPattern = pt;
	this->patternParams = args;
	this->patternSpeed = patternSpeed;

	switch(this->currentPattern)
	{
	case PatternType::Rectangle_:
		{
			//ARGS 
			// 0 = longueur
			// 1 = largeur
			// v = px/sec
			CheckArgSize(2);

			_distance_left = patternParams->at(0);
			_direction = sf::Vector2i(1,0);

			break;
		}
	case PatternType::Circle_:
		{
			//ARGS 
			// 0 = rayon
			// 1 = >=0 for clockwise, other anti_clockwise
			// v = vitesse angulaire (degres/s)
			CheckArgSize(2);

			this->patternSpeed = patternSpeed*M_PI/180; //converting speed to radians
			this->_curSandboxPosition_polar = sf::Vector2f(patternParams->at(0),-M_PI_2); //starts on top of the circle (-pi/2)
			this->_curSandboxPosition_cartesian = ToCartesianCoords(this->_curSandboxPosition_polar);

			break;
		}
	case PatternType::Oscillator:
		{
			//ARGS 
			// 0 = amplitude
			// 1 = angle (en degres)
			// v = vitesse en px/sec
			CheckArgSize(2);

			this->patternSpeed = patternSpeed*2*M_PI/patternParams->at(0); //converting speed to radians (2pi = 1 amplitude)

			this->_curSandboxPosition_polar = sf::Vector2f(patternParams->at(0)/2,patternParams->at(1)*M_PI/180); // r = ampl/2 + converting angle to radians
			this->_curSandboxPosition_cartesian = ToCartesianCoords(this->_curSandboxPosition_polar);

			this->_currTheta = -M_PI_2; //starting @the middle

			break;
		}
	}
}

sf::Vector2f  PatternBobby::GetOffset(float seconds, bool absolute_coordinate)
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
			static float moved= abs(offset.x) + abs(offset.y);

			if(_distance_left > moved)
			{
				//Moving on the edge, like a boss
				_distance_left -= moved;
			}
			else
			{
				offset.x = _direction.x*_distance_left;
				offset.y = _direction.y*_distance_left;

				//Changing direction
				if(abs(_direction.x) == 1)
				{
					_direction.y = _direction.x;
					_direction.x = 0;
				}
				else
				{
					_direction.x = -_direction.y;
					_direction.y = 0;
				}

				if(_distance_left < moved)
				{
					offset.x += _direction.x*abs(_distance_left-moved);
					offset.y += _direction.y*abs(_distance_left-moved);

					_distance_left = (abs(_direction.x==1) ? patternParams->at(0) : patternParams->at(1)) - abs(_distance_left-moved);
				}
				else
				{
					//longueur ou largeur ?
					_distance_left = abs(_direction.x==1) ? patternParams->at(0) : patternParams->at(1);
				}				
			}
			break;
		}

	case PatternType::Circle_:
		{
			//ARGS 
			// 0 = rayon
			// v = vitesse angulaire (degres/s)

			static float new_angle;
			static sf::Vector2f next;

			//Updating our current theta [modulo 2PI]
			new_angle = this->_curSandboxPosition_polar.y +  (patternParams->at(1) >= 0 ? seconds*this->patternSpeed : -seconds*this->patternSpeed);
			this->_curSandboxPosition_polar.y = fmod(new_angle, 2*M_PI);

			//Our next position:
			next = ToCartesianCoords(this->_curSandboxPosition_polar);

			//return offset = diff between new and old position
			if (!absolute_coordinate)
			{
				offset.x = next.x - this->_curSandboxPosition_cartesian.x;
				offset.y = next.y - this->_curSandboxPosition_cartesian.y;
			}
			//or the new position only:
			else
			{
				offset.x = next.x;
				offset.y = next.y;
			}

			this->_curSandboxPosition_cartesian.x = next.x;
			this->_curSandboxPosition_cartesian.y = next.y;

			break;
		}

	case PatternType::Oscillator:
		{
			//ARGS 
			// 0 = rayon
			// v = vitesse angulaire (degres/s)
			static sf::Vector2f next;

			//Updating our current theta [modulo 2PI]
			this->_currTheta = fmod(this->_currTheta + seconds*this->patternSpeed, 2*M_PI);

			//Our next position (r is updated according to cos(theta))
			next.x = this->_curSandboxPosition_polar.x*cos(this->_currTheta);
			next.y = this->_curSandboxPosition_polar.y;
			ToCartesianCoords(&next);

			//return offset = diff between new and old position
			if (!absolute_coordinate)
			{
				offset.x = next.x - this->_curSandboxPosition_cartesian.x;
				offset.y = next.y - this->_curSandboxPosition_cartesian.y;
			}
			//or the new position only:
			else
			{
				offset.x = next.x;
				offset.y = next.y;
			}

			this->_curSandboxPosition_cartesian.x = next.x;
			this->_curSandboxPosition_cartesian.y = next.y;

			break;
		}

	default:
		{
			throw invalid_argument(TextUtils::format("Game error: Unknow pattern # '%d'", this->currentPattern));
		}
	}
	
	return offset;
	
	
}

sf::Vector2f PatternBobby::ToCartesianCoords(sf::Vector2f polarCoords)
{
	sf::Vector2f v;
	v.x = polarCoords.x*cos(polarCoords.y);
	v.y = polarCoords.x*sin(polarCoords.y);
	return v;
}

void PatternBobby::ToCartesianCoords(sf::Vector2f* polarCoords)
{
	static float r;
	r = polarCoords->x;
	polarCoords->x = polarCoords->x*cos(polarCoords->y);
	polarCoords->y = r*sin(polarCoords->y);
}

void PatternBobby::CheckArgSize(int expected)
{
	if(this->patternParams->size() < expected)
	{
		throw invalid_argument(TextUtils::format("PatternBobby error: Invalid # or arges for pattern '%d' (received %d, expected %d)", this->currentPattern, this->patternParams->size(),expected));
	}
}