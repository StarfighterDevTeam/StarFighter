#ifndef CARD_H_INCLUDED
#define CARD_H_INCLUDED

enum CardType
{
	Air,
	Fire,
	Nature,
	Water,
	Madness,
};

enum CardValue
{
	Power1,
	Power2,
	Power3,
};

class Card
{
public:
	Card(){};
	Card(CardType type, CardValue value) { m_type = type; m_value = value; };
	~Card() {};

	CardType m_type;
	CardValue m_value;
};

#endif // CARD_H_INCLUDED