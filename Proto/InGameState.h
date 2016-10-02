#ifndef INGAMESTATE_H_INCLUDED
#define INGAMESTATE_H_INCLUDED

#include "SEInGameState.h"

class InGameState : public SEInGameState
{
public:
	virtual void Initialize(sf::RenderWindow* window, SEShip* playerShip) override;
	virtual void Update(sf::Time deltaTime) override;
	virtual void Draw() override;
	virtual void CreateSFPanel(SFPanelTypes panel_type, SEShip* playerShip) override;
	virtual void DestroySFPanel(SEShip* playerShip) override;
	virtual void LoadCSVFile(string scenes_file) override;
};

#endif //INGAMESTATE_H_INCLUDED
