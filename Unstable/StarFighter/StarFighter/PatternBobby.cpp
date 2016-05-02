#include "PatternBobby.h"

PatternBobby::PatternBobby()
{
	m_currentPattern = NoMovePattern;
}

PatternBobby* PatternBobby::PatternLoader(vector<string> line_data, int index)
{
	PatternBobby* bobby = new PatternBobby();

	PatternType pattern_type = NoMovePattern;

	if (line_data[index].compare("0") != 0)
	{
		bobby->m_patternSpeed = stoi(line_data[index + BOBBY_PATTERN_SPEED]);//angular speed, horizontal speed

		if (line_data[index].compare("line") == 0)
		{
			pattern_type = Line_;
			bobby->m_patternParams.push_back(stoi(line_data[index + BOBBY_PATTERN_ARG2])); // opposite sign of speed (-1), random sign (0), normal (1)
		}
		else if (line_data[index].compare("circle") == 0)
		{
			pattern_type = Circle_;
			bobby->m_patternParams.push_back(stoi(line_data[index + BOBBY_PATTERN_ARG1])); // radius
			bobby->m_patternParams.push_back(stoi(line_data[index + BOBBY_PATTERN_ARG2]));  // counterclockwise (-1), random (0), clockwise (1)
		}
		else if (line_data[index].compare("oscillator") == 0)
		{
			pattern_type = Oscillator;
			bobby->m_patternParams.push_back(stoi(line_data[index + BOBBY_PATTERN_ARG1])); // radius
			bobby->m_patternParams.push_back(stoi(line_data[index + BOBBY_PATTERN_ARG2]));  // counterclockwise (-1), random (0), clockwise (1)
			bobby->m_patternParams.push_back(stoi(line_data[index + BOBBY_PATTERN_ARG3]));  // counterclockwise (-1), random (0), clockwise (1)
		}
		else if (line_data[index].compare("rectangle") == 0)
		{
			pattern_type = Rectangle_;
			bobby->m_patternParams.push_back(stoi(line_data[index + BOBBY_PATTERN_ARG1])); // width
			bobby->m_patternParams.push_back(stoi(line_data[index + BOBBY_PATTERN_ARG2]));  // height
		}
	}

	bobby->m_currentPattern = pattern_type;

	return bobby;
}

void PatternBobby::SetPattern(PatternType pt, float patternSpeed, vector<float> args)
{
	//Note that patternSpeed is 
	// - px/sec on the canvas for Rectangle
	// - time to do 360° for the circle

	m_currentPattern = pt;
	m_patternParams = args;
	m_patternSpeed = patternSpeed;

	switch(m_currentPattern)
	{
		case Line_:
		{
			//ARGS 
			// 0 = opposite speed ?
			// v = speed in px/sec
			CheckArgSize(1);

			if(m_patternParams[0] == 0)
			{
				srand(time(NULL));
				//random between 1 and -1
				m_patternParams[0] = ((rand() % 2) * 2) - 1;
			}

			break;
		}

		case Rectangle_:
		{
			//ARGS 
			// 0 = width
			// 1 = height
			// v = speed in px/sec
			CheckArgSize(2);

			m_distance_left = m_patternParams[0];
			m_direction = sf::Vector2i(1,0);

			break;
		}

		case Circle_:
		{
			//ARGS 
			// 0 = rayon
			// 1 = counterclockwise (-1), random (0), clockwise (1)
			// v = vitesse angulaire (degres/s)
			CheckArgSize(2);

			if (m_patternParams[1] == 0)
			{
				srand(time(NULL));
				//random between 1 and -1
				m_patternParams[1] = ((rand() % 2) * 2) - 1;
			}
			m_patternSpeed = patternSpeed*M_PI/180; //converting speed to radians
			m_curSandboxPosition_polar = sf::Vector2f(m_patternParams[0], -M_PI_2*m_patternParams[1]); //starts on top of the circle (-pi/2)
			m_curSandboxPosition_cartesian = ToCartesianCoords(m_curSandboxPosition_polar);

			break;
		}

		case Oscillator:
		{
			//ARGS 
			// 0 = amplitude
			// 1 = counterclockwise (-1), random (0), clockwise (1)
			// 2 = centered(1), not centered (0)
			// v = speed of oscillation
			CheckArgSize(3);

			if (m_patternParams[1] == 0)
			{
				srand(time(NULL));
				//random between 1 and -1
				m_patternParams[1] = ((rand() % 2) * 2) - 1;
			}

			m_patternSpeed = patternSpeed*2*M_PI/m_patternParams[0]; //converting speed to radians (2pi = 1 amplitude)

			m_curSandboxPosition_polar = sf::Vector2f(m_patternParams[0] / 2, m_patternParams[1] * M_PI / 180); // r = ampl/2 + converting angle to radians
			
			m_curSandboxPosition_cartesian = m_patternParams[2] == 1 ? sf::Vector2f(0, 0) : ToCartesianCoords(m_curSandboxPosition_polar);
			
			m_currTheta = M_PI/2; //starting @the middle.

			break;
		}
	}
}

sf::Vector2f  PatternBobby::GetOffset(float seconds, bool absolute_coordinate)
{
	static sf::Vector2f offset;

	switch(m_currentPattern)
	{
		case NoMovePattern:
		{
			offset.x=0;
			offset.y=0;
			break;
		}
	
		case Line_:
		{
			//ARGS 
			// 0 = xspeed
			offset.x = m_patternSpeed * m_patternParams[0]*seconds;
			offset.y = 0;
			break;							
		}

		case Rectangle_:
		{
			//ARGS 
			// 0 = longueur
			// 1 = largeur

			//just move on the line
			offset.x = m_direction.x * m_patternSpeed*seconds;
			offset.y = m_direction.y * m_patternSpeed*seconds;
			static float moved= abs(offset.x) + abs(offset.y);

			if(m_distance_left > moved)
			{
				//Moving on the edge, like a boss
				m_distance_left -= moved;
			}
			else
			{
				offset.x = m_direction.x*m_distance_left;
				offset.y = m_direction.y*m_distance_left;

				//Changing direction
				if(abs(m_direction.x) == 1)
				{
					m_direction.y = m_direction.x;
					m_direction.x = 0;
				}
				else
				{
					m_direction.x = -m_direction.y;
					m_direction.y = 0;
				}

				if(m_distance_left < moved)
				{
					offset.x += m_direction.x*abs(m_distance_left-moved);
					offset.y += m_direction.y*abs(m_distance_left-moved);

					m_distance_left = (abs(m_direction.x) == 1 ? m_patternParams[0] : m_patternParams[1]) - abs(m_distance_left-moved);
				}
				else
				{
					//longueur ou largeur ?
					m_distance_left = abs(m_direction.x) == 1 ? m_patternParams[0] : m_patternParams[1];
				}				
			}

			if (absolute_coordinate)
			{
				if (m_direction.x == 1)
				{
					offset.x = m_patternParams[0] / 2 - m_distance_left;
					offset.y = - m_patternParams[1] / 2;
				}
				else if (m_direction.x == -1)
				{
					offset.x = - (m_patternParams[0] / 2 - m_distance_left);
					offset.y = m_patternParams[1] / 2;
				}
				else if (m_direction.y == 1)
				{
					offset.x = m_patternParams[0] / 2;
					offset.y = m_patternParams[1] / 2 - m_distance_left;
				}
				else if (m_direction.y == -1)
				{
					offset.x = -(m_patternParams[0] / 2);
					offset.y = -(m_patternParams[1] / 2 - m_distance_left);
				}	
			}

			break;
		}

		case Circle_:
		{
			//ARGS 
			// 0 = rayon
			// v = vitesse angulaire (degres/s)

			static float new_angle;
			static sf::Vector2f next;

			//Updating our current theta [modulo 2PI]
			new_angle = m_curSandboxPosition_polar.y + (m_patternParams[1] >= 0 ? seconds*m_patternSpeed : -seconds*m_patternSpeed);
			m_curSandboxPosition_polar.y = fmod(new_angle, 2*M_PI);

			//Our next position:
			next = ToCartesianCoords(m_curSandboxPosition_polar);

			//return offset = diff between new and old position
			if (!absolute_coordinate)
			{
				offset.x = m_patternParams[1] * (next.x - m_curSandboxPosition_cartesian.x);
				offset.y = m_patternParams[1] * (next.y - m_curSandboxPosition_cartesian.y);
			}
			//or the new position only:
			else
			{
				offset.x = m_patternParams[1] * next.x;
				offset.y = m_patternParams[1] * next.y;
			}

			m_curSandboxPosition_cartesian.x = next.x;
			m_curSandboxPosition_cartesian.y = next.y;

			break;
		}

		case Oscillator:
		{
			//ARGS 
			// 0 = rayon
			// v = vitesse angulaire (degres/s)
			static sf::Vector2f next;

			//Updating our current theta [modulo 2PI]
			m_currTheta = fmod(m_currTheta + seconds*m_patternSpeed, 2*M_PI);

			//Our next position (r is updated according to cos(theta))
			next.x = m_curSandboxPosition_polar.x*cos(m_currTheta);
			next.y = m_curSandboxPosition_polar.y;
			ToCartesianCoords(&next);

			//printf("theta : %f, next: %f\n", m_currTheta, next.x);
			//first oscillation of a "centered" oscillator pattern is twice smaller. here we detect that this first oscillation is over and we switch back to full amplitude oscillations
			
			//return offset = diff between new and old position
			if (!absolute_coordinate)
			{
				offset.x = m_patternParams[1] * (next.x - m_curSandboxPosition_cartesian.x);
				offset.y = m_patternParams[1] * (next.y - m_curSandboxPosition_cartesian.y);
			}
			//or the new position only:
			else
			{
				offset.x = m_patternParams[1] * next.x;
				offset.y = m_patternParams[1] * next.y;
			}

			m_curSandboxPosition_cartesian.x = next.x;
			m_curSandboxPosition_cartesian.y = next.y;

			break;
		}

		default:
		{
			throw invalid_argument(TextUtils::format("Game error: Unknow pattern # '%d'", m_currentPattern));
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
	if(m_patternParams.size() < expected)
	{
		throw invalid_argument(TextUtils::format("PatternBobby error: Invalid # or arges for pattern '%d' (received %d, expected %d)", m_currentPattern, m_patternParams.size(),expected));
	}
}