#pragma once
#include <vector>
#include <SFML\Graphics.hpp>
#include <Windows.h>
#include <memory>

class Desktop
{
public:
	Desktop() {};
	virtual ~Desktop() {};

	static void update();
	static std::tuple<bool, sf::Vector2i> collides(sf::IntRect, sf::Vector2f);
	static void ignoreWindow(sf::RenderWindow*);
private:
	struct CollisionLine
	{
		sf::Vector2i startPoint;
		bool vertical;
		int length;
		bool blockedFromTopLeft, blockedFromDownRight;
		std::tuple<bool, sf::Vector2i> collides(sf::IntRect box, sf::Vector2f velocity);
	};

	static std::shared_ptr<std::vector<CollisionLine>> windowColliders;
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lparam);
	static std::vector<HWND> windowsToIgnore;
};