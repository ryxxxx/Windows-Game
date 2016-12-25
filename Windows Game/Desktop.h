#pragma once
#include <vector>
#include <SFML\Graphics.hpp>
#include <Windows.h>
#include <memory>
#include <algorithm>
#include <map>
#include <fstream>

class Desktop
{
public:
	Desktop() {};
	virtual ~Desktop() {};

	static void update_colliders();
	static void update_options();
	static std::tuple<bool, sf::Vector2i> collides(sf::IntRect, sf::Vector2f);
	static void ignoreWindow(sf::RenderWindow*);
	static std::map<std::string, std::string> options;
private:
	struct CollisionLine
	{
		sf::Vector2i startPoint;
		bool vertical;
		int length;
		bool blockedFromTopLeft, blockedFromDownRight;
		std::tuple<bool, sf::Vector2i> collides(sf::IntRect box, sf::Vector2f velocity);
	};

	static std::shared_ptr<std::vector<std::tuple<HWND, CollisionLine>>> windowColliders;
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lparam);
	static std::vector<HWND> windowsToIgnore;
};