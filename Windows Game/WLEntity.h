#pragma once
#include "Animation.h"
#include <SFML\Network.hpp>
#include <memory>

class WLEntity
{
public:
	WLEntity();
	virtual ~WLEntity() {};

protected:
	sf::RenderWindow window;
	std::map<std::string, std::shared_ptr<Animation>> animations;
};