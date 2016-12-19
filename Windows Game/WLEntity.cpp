#include "WLEntity.h"

WLEntity::WLEntity()
{
	window.create(sf::VideoMode(0, 0), "", sf::Style::None);
	run();
}