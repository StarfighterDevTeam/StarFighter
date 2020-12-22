#include "GeometryPattern.h"

GeometryPattern::GeometryPattern()
{
	m_pattern_type = NoMovePattern;
	m_speed = 0;
	m_clockwise = 0;
	m_width = 0;
	m_height = 0;
	m_starting_point = -1;
	m_offset = sf::Vector2f(0, 0);
}

GeometryPattern* GeometryPattern::LoadPattern(vector<string> line_data, int index)
{
	GeometryPattern* pattern = new GeometryPattern();
		//return pattern;//DEBUG

	if (line_data[index].compare("0") != 0)
	{
		pattern->m_speed = stoi(line_data[index + GEOMETRY_PATTERN_SPEED]);
		pattern->m_clockwise = stoi(line_data[index + GEOMETRY_PATTERN_CLOCKWISE]);

		if (line_data[index].compare("line") == 0)
		{
			pattern->m_pattern_type = Line_;
		}
		else if (line_data[index].compare("circle") == 0)
		{
			pattern->m_pattern_type = Circle_;

			pattern->m_width = stoi(line_data[index + GEOMETRY_PATTERN_WIDTH]);
			pattern->m_height = stoi(line_data[index + GEOMETRY_PATTERN_HEIGHT]);
			pattern->m_starting_point = stoi(line_data[index + GEOMETRY_PATTERN_STARTING_POINT]);
		}
		else if (line_data[index].compare("rectangle") == 0)
		{
			pattern->m_pattern_type = Rectangle_;

			pattern->m_width = stoi(line_data[index + GEOMETRY_PATTERN_WIDTH]);
			pattern->m_height = stoi(line_data[index + GEOMETRY_PATTERN_HEIGHT]);
			pattern->m_starting_point = stoi(line_data[index + GEOMETRY_PATTERN_STARTING_POINT]);
		}
	}

	return pattern;
}

void GeometryPattern::resetPattern()
{
	setPattern_v2(m_pattern_type, m_speed, m_clockwise, m_width, m_height, m_starting_point);
}

void GeometryPattern::setPattern_v2(GeometryPattern* pattern)
{
	setPattern_v2(pattern->m_pattern_type, pattern->m_speed, pattern->m_clockwise, pattern->m_width, pattern->m_height, pattern->m_starting_point);
}

void GeometryPattern::setPattern_v2(PatternType pattern_type, int speed, int clockwise, int width, int height, int starting_point)
{
	if (pattern_type == m_pattern_type && speed == m_speed && clockwise == m_clockwise && width == m_width && height == m_height && starting_point == m_starting_point)
		return;//identical pattern asked => ignored.

	if (clockwise == 0)//randomized "clockwise" variable
	{
		if (m_clockwise == 0)
		{
			srand(time(NULL));
			clockwise = ((rand() % 2) * 2) - 1;
		}
		else
			clockwise = m_clockwise;//keep current value
	}
	
	m_speed = speed;
	m_clockwise = clockwise;
	m_width = width;
	m_height = height;

	//starting point
	if (pattern_type >= 0 && (pattern_type != m_pattern_type || starting_point != m_starting_point))
	{
		m_starting_point = starting_point;
		m_pattern_type = pattern_type;
		int cc = m_clockwise * m_speed;

		switch (m_pattern_type)
		{
			case Rectangle_:
			{
				//0-----1----2
				//|          |
				//7          3
				//|          |
				//6-----5----4

				//case 0 + default value
				m_direction.x = cc >= 0 ? 1 : 0;
				m_direction.y = cc >= 0 ? 0 : 1;
				m_distance_left = cc >= 0 ? m_width : m_height;
	
				if (m_starting_point == 1)
				{
					m_direction.x = cc >= 0 ? 1 : -1;
					m_direction.y = 0;
					m_distance_left = m_width * 0.5;
				}
				else if (m_starting_point == 2)
				{
					m_direction.x = cc >= 0 ? 0 : -1;
					m_direction.y = cc >= 0 ? 1 : 0;
					m_distance_left = cc >= 0 ? m_height : m_width;
				}
				else if (m_starting_point == 3)
				{
					m_direction.x = 0;
					m_direction.y = cc >= 0 ? 1 : -1;
					m_distance_left = m_height * 0.5;
				}
				else if (m_starting_point == 4)
				{
					m_direction.x = cc >= 0 ? -1 : 0;
					m_direction.y = cc >= 0 ? 0 : -1;
					m_distance_left = cc >= 0 ? m_width : m_height;
				}
				else if (m_starting_point == 5)
				{
					m_direction.x = cc >= 0 ? -1 : 1;
					m_direction.y = 0;
					m_distance_left = m_width * 0.5;
				}
				else if (m_starting_point == 6)
				{
					m_direction.x = cc >= 0 ? 0 : -1;
					m_direction.y = cc >= 0 ? -1 : 0;
					m_distance_left = cc >= 0 ? m_height : m_width;
				}
				else if (m_starting_point == 7)
				{
					m_direction.x = 0;
					m_direction.y = cc >= 0 ? -1 : 1;
					m_distance_left = m_height * 0.5;
				}

				break;
			}
			case Circle_:
			{
				//      0
				//   11    1
				// 10       2
				// 9         3
				//  8       4
				//    7   5
				//      6
				m_theta = M_PI * m_starting_point / 6;

				break;
			}
			default:
				break;
		}
	}
}

sf::Vector2f GeometryPattern::getOffset_v2(float deltaTime, bool global_offset)
{
	int cc = m_clockwise * m_speed;

	if (global_offset == false)//if false, we want the offset relative to the previous frame only
	{
		m_offset.x = 0;
		m_offset.y = 0;
	}

	switch (m_pattern_type)
	{
		case NoMovePattern:
		{
			m_offset.x = 0;
			m_offset.y = 0;
			break;
		}
		case Line_:
		{
			m_offset.x += deltaTime * m_speed * m_clockwise;
			m_offset.y = 0;
			break;
		}
		case Circle_:
		{
			float old_theta = m_theta;
			m_theta += deltaTime * M_PI * m_speed * m_clockwise / 180;

			if (m_theta > 2 * M_PI)
				m_theta -= 2 * M_PI;
			else if (m_theta < 0)
				m_theta += 2 * M_PI;

			m_offset.x += -(sin(old_theta) - sin(m_theta)) * m_width;
			m_offset.y += (cos(old_theta) - cos(m_theta)) * m_height;;

			break;
		}
		case Rectangle_:
		{
			float move_x = deltaTime * m_speed * m_direction.x;
			float move_y = deltaTime * m_speed * m_direction.y;
			float moved = abs(move_x) + abs(move_y);

			sf::Vector2f offset_tmp = sf::Vector2f(0, 0);

			if (moved <= m_distance_left)//didn't cross a corner, moving in line
			{
				offset_tmp.x += move_x;
				offset_tmp.y += move_y;
				m_distance_left -= moved;
			}	
			else//crossed a corner, changing direction
			{
				//move up to the corner
				if (move_x > 0)
					offset_tmp.x = m_distance_left;
				else if (move_x < 0)
					offset_tmp.x = -m_distance_left;
				else if (move_y > 0)
					offset_tmp.y = m_distance_left;
				else
					offset_tmp.y = -m_distance_left;

				if (m_height == 0)//handling specific case of width=0
				{
					m_direction.x *= -1;
					m_distance_left = m_width - (abs(move_x) - abs(offset_tmp.x));
				}
				else if (m_width == 0)//handling specific case of height=0
				{
					m_direction.y *= -1;
					m_distance_left = m_height - (abs(move_y) - abs(offset_tmp.y));
				}
				//normal rectangle case: changing direction
				else if (m_direction.x == 0)//from y to x
				{
					m_distance_left = m_width - (abs(move_y) - abs(offset_tmp.y));//width/height of rectangle minus distance already used
					m_direction.y = 0;
					if ((cc > 0 && offset_tmp.y > 0) || (cc < 0 && offset_tmp.y < 0))
						m_direction.x = -1;
					else if ((cc > 0 && offset_tmp.y < 0) || (cc < 0 && offset_tmp.y > 0))
						m_direction.x = 1;
				}
				else//from x to y
				{
					m_distance_left = m_height - (abs(move_x) - abs(offset_tmp.x));//width/height of rectangle minus distance already used
					m_direction.x = 0;
					if ((cc > 0 && offset_tmp.x > 0) || (cc < 0 && offset_tmp.x < 0))
						m_direction.y = 1;
					else if ((cc > 0 && offset_tmp.x < 0) || (cc < 0 && offset_tmp.x > 0))
						m_direction.y = -1;
				}

				//move the remaining distance in the new direction
				offset_tmp.x += m_direction.x != 0 ? (abs(move_y) - abs(offset_tmp.y)) * m_direction.x : 0;
				offset_tmp.y += m_direction.y != 0 ? (abs(move_x) - abs(offset_tmp.x)) * m_direction.y : 0;
			}

			//add new delta offset to global offset
			m_offset.x += offset_tmp.x;
			m_offset.y += offset_tmp.y;

			break;
		}
	}

	return m_offset;
}