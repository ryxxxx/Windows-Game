#pragma once
#include <SFML\Graphics.hpp>
#include <Windows.h>
#include <vector>

class Animation
{
public:
	Animation(std::string, float, sf::Vector2i, bool);
	virtual ~Animation();

	void draw(sf::RenderWindow&);
	void update(float);
	void reset();
	sf::Vector2i getSize();
private:
	struct Frame 
	{
		HRGN region;
		sf::IntRect textureRect;
	};
	std::vector<Frame> frames;
	sf::Texture texture;
	sf::Sprite sprite;
	size_t currentFrame = 0;
	float timeSinceLastUpdate = 0.f;
	unsigned int frameWidth, frameHeight;
	bool loop;
	bool done = false;

	float frameTime;
};