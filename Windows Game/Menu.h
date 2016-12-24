#pragma once
#include "ProgramDefinition.h"
#ifdef MENU
#include "WLEntity.h"

class Menu : public WLEntity
{
public:
	Menu();
	~Menu();

private:
	void start();
	void update(float);
	void handleEvents();

	bool done = false;
	bool mouseReleased = false;
	bool wasPressed = false;
	bool dragging = false;
	const sf::IntRect playButtonCollider = sf::IntRect{ 13, 13, 55, 55 };
	const sf::IntRect settingsButtonCollider = sf::IntRect{ 75, 13, 55, 55 };
	const sf::IntRect exitButtonCollider = sf::IntRect{ 137, 13, 55, 55 };
	sf::Vector2i grabOffset;
	std::string currentAnimationTitle;
};
#endif