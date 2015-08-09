#include "PatternBobby.h"

PatternBobby::PatternBobby()
{
	this->currentPattern = PatternType::NoMovePattern;
}

PatternBobby* PatternBobby::PatternLoader(vector<string> line_data, int index)
{
	PatternBobby* bobby = new PatternBobby();

	vector<float> v;
	PatternType pattern_type = PatternType::NoMovePattern;

	if (line_data[index].compare("0") != 0)
	{
		bobby->patternSpeed = stoi(line_data[index + BobbyPatternData::BOBBY_PATTERN_SPEED]);//angular speed, horizontal speed

		if (line_data[index].compare("line") == 0)
		{
			pattern_type = PatternType::Line_;
			v.push_back(stoi(line_data[index + BobbyPatternData::BOBBY_PATTERN_ARG2])); // opposite sign of speed (-1), random sign (0), normal (1)
		}
		else if (line_data[index].compare("circle") == 0)
		{
			pattern_type = PatternType::Circle_;
			v.push_back(stoi(line_data[index + BobbyPatternData::BOBBY_PATTERN_ARG1])); // radius
			v.push_back(stoi(line_data[index + BobbyPatternData::BOBBY_PATTERN_ARG2]));  // counterclockwise (-1), random (0), clockwise (1)
		}
		else if (line_data[index].compare("oscillator") == 0)
		{
			pattern_type = PatternType::Oscillator;
			v.push_back(stoi(line_data[index + BobbyPatternData::BOBBY_PATTERN_ARG1])); // radius
			v.push_back(stoi(line_data[index + BobbyPatternData::BOBBY_PATTERN_ARG2]));  // counterclockwise (-1), random (0), clockwise (1)
		}
		else if (line_data[index].compare("rectangle") == 0)
		{
			pattern_type = PatternType::Rectangle_;
			v.push_back(stoi(line_data[index + BobbyPatternData::BOBBY_PATTERN_ARG1])); // width
			v.push_back(stoi(line_data[index + BobbyPatternData::BOBBY_PATTERN_ARG2]));  // height
		}
	}

	bobby->currentPattern = pattern_type;
	bobby->patternParams = v;

	return bobby;
}

void PatternBobby::SetPattern(PatternType pt, float patternSpeed, const vector<float>&  args)
{
	//Note that patternSpeed is 
	// - px/sec on the canvas for Rectangle
	// - time to do 360° for the circle

	this->currentPattern = pt;
	this->patternParams = args;
	this->patternSpeed = patternSpeed;

	switch(this->currentPattern)
	{
		case PatternType::Line_:
		{
			//ARGS 
			// 0 = opposite speed ?
			// v = speed in px/sec
			CheckArgSize(1);

			if(this->patternParams.at(0) == 0)
			{
				srand(time(NULL));
				//random between 1 and -1
				this->patternParams.at(0) = ((rand() % 2) * 2) - 1;
			}

			break;
		}

		case PatternType::Rectangle_:
		{
			//ARGS 
			// 0 = width
			// 1 = height
			// v = speed in px/sec
			CheckArgSize(2);

			_distance_left = patternParams.at(0);
			_direction = sf::Vector2i(1,0);

			break;
		}

		case PatternType::Circle_:
		{
			//ARGS 
			// 0 = rayon
			// 1 = counterclockwise (-1), random (0), clockwise (1)
			// v = vitesse angulaire (degres/s)
			CheckArgSize(2);

			if (this->patternParams.at(1) == 0)
			{
				srand(time(NULL));
				//random between 1 and -1
				this->patternParams.at(1) = ((rand() % 2) * 2) - 1;
			}
			this->patternSpeed = patternSpeed*M_PI/180; //converting speed to radians
			this->_curSandboxPosition_polar = sf::Vector2f(patternParams.at(0), -M_PI_2*patternParams.at(1)); //starts on top of the circle (-pi/2)
			this->_curSandboxPosition_cartesian = ToCartesianCoords(this->_curSandboxPosition_polar);

			break;
		}

		case PatternType::Oscillator:
		{
			//ARGS 
			// 0 = amplitude
			// 1 = counterclockwise (-1), random (0), clockwise (1)
			// v = speed of oscillation
			CheckArgSize(2);

			if (this->patternParams.at(1) == 0)
			{
				srand(time(NULL));
				//random between 1 and -1
				this->patternParams.at(1) = ((rand() % 2) * 2) - 1;
			}

			this->patternSpeed = patternSpeed*2*M_PI/patternParams.at(0); //converting speed to radians (2pi = 1 amplitude)

 			this->_curSandboxPosition_polar = sf::Vector2f(patternParams.at(0)/2, patternParams.at(1)*M_PI/180); // r = ampl/2 + converting angle to radians
			this->_curSandboxPosition_cartesian = ToCartesianCoords(this->_curSandboxPosition_polar);

			this->_currTheta = 0; //starting @the middle. -PI/2 to start at the left max value

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
	
		case PatternType::Line_:
		{
			//ARGS 
			// 0 = xspeed
			offset.x = patternSpeed*this->patternParams.at(0)*seconds;
			offset.y = 0;
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

					_distance_left = (abs(_direction.x) == 1 ? patternParams.at(0) : patternParams.at(1)) - abs(_distance_left-moved);
				}
				else
				{
					//longueur ou largeur ?
					_distance_left = abs(_direction.x) == 1 ? patternParams.at(0) : patternParams.at(1);
				}				
			}

			if (absolute_coordinate)
			{
				if (_direction.x == 1)
				{
					offset.x = patternParams.at(0) / 2 - _distance_left;
					offset.y = - patternParams.at(1) / 2;
				}
				else if (_direction.x == -1)
				{
					offset.x = - (patternParams.at(0) / 2 - _distance_left);
					offset.y = patternParams.at(1) / 2;
				}
				else if (_direction.y == 1)
				{
					offset.x = patternParams.at(0) / 2;
					offset.y = patternParams.at(1) / 2 - _distance_left;
				}
				else if (_direction.y == -1)
				{
					offset.x = -(patternParams.at(0) / 2);
					offset.y = -(patternParams.at(1) / 2 - _distance_left);
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
			new_angle = this->_curSandboxPosition_polar.y + (patternParams.at(1) >= 0 ? seconds*this->patternSpeed : -seconds*this->patternSpeed);
			this->_curSandboxPosition_polar.y = fmod(new_angle, 2*M_PI);

			//Our next position:
			next = ToCartesianCoords(this->_curSandboxPosition_polar);

			//return offset = diff between new and old position
			if (!absolute_coordinate)
			{
				offset.x = this->patternParams.at(1) * (next.x - this->_curSandboxPosition_cartesian.x);
				offset.y = this->patternParams.at(1) * (next.y - this->_curSandboxPosition_cartesian.y);
			}
			//or the new position only:
			else
			{
				offset.x = this->patternParams.at(1) * next.x;
				offset.y = this->patternParams.at(1) * next.y;
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
				offset.x = this->patternParams.at(1) * (next.x - this->_curSandboxPosition_cartesian.x);
				offset.y = this->patternParams.at(1) * (next.y - this->_curSandboxPosition_cartesian.y);
			}
			//or the new position only:
			else
			{
				offset.x = this->patternParams.at(1) * next.x;
				offset.y = this->patternParams.at(1) * next.y;
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

void PatternBobby::CheckArgSize(size_t expected)
{
	if(this->patternParams.size() < expected)
	{
		throw invalid_argument(TextUtils::format("PatternBobby error: Invalid # or arges for pattern '%d' (received %d, expected %d)", this->currentPattern, this->patternParams.size(),expected));
	}
}