#pragma once
#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "SEGame.h"

class Game : public SEGame
{
public:
	virtual void init(RenderWindow* window) override;
	virtual int LoadSFX() override;
};

#endif // GAME_H_INCLUDED
