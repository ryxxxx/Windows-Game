#include "Desktop.h"
#include <iostream>

std::shared_ptr<std::vector<std::tuple<HWND ,Desktop::CollisionLine>>> Desktop::windowColliders = std::make_shared<std::vector<std::tuple<HWND, Desktop::CollisionLine>>>();
std::vector<HWND> Desktop::windowsToIgnore = std::vector<HWND>();
std::map<std::string, std::string> Desktop::options = std::map<std::string, std::string>();

std::tuple<bool, sf::Vector2i> Desktop::CollisionLine::collides(sf::IntRect box, sf::Vector2f velocity)
{
	if (vertical)
	{
		if (blockedFromTopLeft && velocity.y > -1.f)
		{
			if (box.top<startPoint.y &&
				box.top + box.height>startPoint.y &&
				box.left + box.width > startPoint.x &&
				box.left < startPoint.x + length)
			{
				return std::tuple<bool, sf::Vector2i>(true, sf::Vector2i(box.left, startPoint.y - box.height));
			}
		}
		if (blockedFromDownRight && velocity.y < 0)
		{
			if (box.top<startPoint.y &&
				box.top + box.height>startPoint.y &&
				box.left + box.width > startPoint.x &&
				box.left < startPoint.x + length)
			{
				return std::tuple<bool, sf::Vector2i>(true, sf::Vector2i(box.left, startPoint.y));
			}
		}
	}
	else
	{
		if (blockedFromTopLeft && velocity.x > 0)
		{
			if (box.left<startPoint.x &&
				box.left + box.width>startPoint.x &&
				box.top + box.height > startPoint.y &&
				box.top < startPoint.y + length)
			{
				return std::tuple<bool, sf::Vector2i>(true, sf::Vector2i(startPoint.x - box.width, box.top));
			}
		}
		if (blockedFromDownRight && velocity.x < 0)
		{
			if (box.left<startPoint.x &&
				box.left + box.width>startPoint.x &&
				box.top + box.height > startPoint.y &&
				box.top < startPoint.y + length)
			{
				return std::tuple<bool, sf::Vector2i>(true, sf::Vector2i(startPoint.x, box.top));
			}
		}
	}
	return std::tuple<bool, sf::Vector2i>(false, sf::Vector2i(0, 0));
}

BOOL HasColliderWindow(HWND hwnd)
{	
	//Check for Taskbar
	char class_name[128];
	GetClassName(hwnd, class_name, sizeof(class_name));
	std::string compareStr = class_name;
	if (compareStr == "Shell_TrayWnd" || compareStr == "Shell_SecondaryTrayWnd")
		return TRUE;

	if (compareStr == "Windows.UI.Core.CoreWindow" || compareStr == "ApplicationFrameWindow")
		return FALSE;

	//ist das fenster versteckt oder nicht
	if (!IsWindowVisible(hwnd))
		return FALSE;

	//kein Plan, aber funktioniertss
	HWND hwndTry, hwndWalk = NULL;

	hwndTry = GetAncestor(hwnd, GA_ROOTOWNER);
	while (hwndTry != hwndWalk)
	{
		hwndWalk = hwndTry;
		hwndTry = GetLastActivePopup(hwndWalk);
		if (IsWindowVisible(hwndTry))
			break;
	}
	if (hwndWalk != hwnd)
		return FALSE;

	// Tool windows should not be displayed either, these do not appear in the
	// task bar.
	if (GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)
		return FALSE;

	//ist minimiert
	if (IsIconic(hwnd))
		return FALSE;

	return TRUE;
}

BOOL CALLBACK Desktop::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	if (std::find(windowsToIgnore.begin(), windowsToIgnore.end(), hwnd) != windowsToIgnore.end())
		return TRUE;
	if (!HasColliderWindow(hwnd))
		return TRUE;
	LPRECT rect = new RECT;
	GetWindowRect(hwnd, rect);
	//lineCollider für obere seite des fensters
	CollisionLine tempCollider1;
	tempCollider1.blockedFromTopLeft = true;
	tempCollider1.blockedFromDownRight = false;
	tempCollider1.vertical = true;
	tempCollider1.startPoint = sf::Vector2i{ static_cast<int>(rect->left + 7), static_cast<int>(rect->top) };
	tempCollider1.length = static_cast<int>((rect->right - 7) - (rect->left + 7));
	Desktop::windowColliders->push_back(std::tuple<HWND, Desktop::CollisionLine>(hwnd, tempCollider1));
	//lineCollider für linke seite des fensters
	CollisionLine tempCollider2;
	tempCollider2.blockedFromTopLeft = true;
	tempCollider2.blockedFromDownRight = false;
	tempCollider2.vertical = false;
	tempCollider2.startPoint = sf::Vector2i{ static_cast<int>(rect->left+7), static_cast<int>(rect->top) };
	tempCollider2.length = static_cast<int>((rect->bottom - 7) - rect->top);
	Desktop::windowColliders->push_back(std::tuple<HWND, Desktop::CollisionLine>(hwnd, tempCollider2));
	//lineCollider für rechte seite des fensters
	CollisionLine tempCollider3;
	tempCollider3.blockedFromTopLeft = false;
	tempCollider3.blockedFromDownRight = true;
	tempCollider3.vertical = false;
	tempCollider3.startPoint = sf::Vector2i{ static_cast<int>(rect->right-7), static_cast<int>(rect->top) };
	tempCollider3.length = static_cast<int>((rect->bottom - 7) - rect->top);
	Desktop::windowColliders->push_back(std::tuple<HWND, Desktop::CollisionLine>(hwnd, tempCollider3));
	DeleteObject(rect);
	delete rect;
	return TRUE;
}

void Desktop::update_colliders()
{
	windowColliders->clear();
	EnumWindows(EnumWindowsProc, NULL);
	CollisionLine rightDesktopSide, leftDesktopSide, bottomDesktopSide, topDesktopSide;
	rightDesktopSide.blockedFromTopLeft = true;
	rightDesktopSide.blockedFromDownRight = false;
	rightDesktopSide.length = sf::VideoMode::getDesktopMode().height;
	rightDesktopSide.startPoint = sf::Vector2i(sf::VideoMode::getDesktopMode().width - 1, 0);
	rightDesktopSide.vertical = false;
	leftDesktopSide.blockedFromTopLeft = false;
	leftDesktopSide.blockedFromDownRight = true;
	leftDesktopSide.length = sf::VideoMode::getDesktopMode().height;
	leftDesktopSide.startPoint = sf::Vector2i(1, 0);
	leftDesktopSide.vertical = false;
	bottomDesktopSide.blockedFromTopLeft = true;
	bottomDesktopSide.blockedFromDownRight = false;
	bottomDesktopSide.length = sf::VideoMode::getDesktopMode().width;
	bottomDesktopSide.startPoint = sf::Vector2i(0, sf::VideoMode::getDesktopMode().height - 1);
	bottomDesktopSide.vertical = true;
	topDesktopSide.blockedFromTopLeft = false;
	topDesktopSide.blockedFromDownRight = true;
	topDesktopSide.length = sf::VideoMode::getDesktopMode().width;
	topDesktopSide.startPoint = sf::Vector2i(0, 1);
	topDesktopSide.vertical = true;
	windowColliders->push_back(std::tuple<HWND, Desktop::CollisionLine>(nullptr, rightDesktopSide));
	windowColliders->push_back(std::tuple<HWND, Desktop::CollisionLine>(nullptr, leftDesktopSide));
	windowColliders->push_back(std::tuple<HWND, Desktop::CollisionLine>(nullptr, bottomDesktopSide));
	windowColliders->push_back(std::tuple<HWND, Desktop::CollisionLine>(nullptr, topDesktopSide));
}

void Desktop::update_options()
{
	options.clear();
	std::ifstream optionsFile{ "..\\options.txt" };
	std::string line;
	while (!optionsFile.eof())
	{
		std::getline(optionsFile, line);
		line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
		if (line[0] == '#')
			continue;
		for (int i = 0; i < line.length(); i++)
		{
			if (line[i] == '=')
			{
				options[line.substr(0, i)] = line.substr(i + 1, line.length());
			}
		}
	}
}

std::tuple<bool, sf::Vector2i> Desktop::collides(sf::IntRect box, sf::Vector2f velocity)
{
	for (size_t i = 0; i < windowColliders->size(); i++)
	{
		std::tuple<bool, sf::Vector2i> collisionInfo = std::get<CollisionLine>((*windowColliders)[i]).collides(box, velocity);
		HWND hwnd = std::get<HWND>((*windowColliders)[i]);
		if (std::get<bool>(collisionInfo))
		{
			return collisionInfo;
		}
	}
	return std::tuple<bool, sf::Vector2i>(false, sf::Vector2i{ 0,0 });
}

void Desktop::ignoreWindow(sf::RenderWindow* window)
{
	windowsToIgnore.push_back(window->getSystemHandle());
}