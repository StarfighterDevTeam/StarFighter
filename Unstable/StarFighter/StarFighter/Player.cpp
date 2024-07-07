#include "Player.h"

bool Player::draw()
{
	//remake draw pile if necessary
	//if (m_draw.empty())
	//{
	//	remakeDrawPile();
	//}

	//draw
	{
		m_hand.push_back(m_draw.front());
		m_draw.pop_front();
	}

	return true;
}

void Player::remakeDrawPile()
{
	for (Card& card : m_discard)
	{
		const int nbCardsInDiscard = (int)m_discard.size();
		const int randomCard = (rand() % nbCardsInDiscard);
		m_draw.push_back(m_discard[randomCard]);
		m_discard.erase(m_discard.begin() + randomCard);
	}
}

void Player::addToDiscard(Card card)
{
	m_discard.push_back(card);
}