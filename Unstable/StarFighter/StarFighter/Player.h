#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "Card.h"
#include <vector>
#include "deque"

class Player
{
public:
	Player() {};
	~Player() {};

	bool draw();
	void remakeDrawPile();
	void addToDiscard(Card card);

	std::vector<Card> m_hand;
	std::vector<Card> m_discard;
	std::deque<Card> m_draw;
};

#endif // PLAYER_H_INCLUDED