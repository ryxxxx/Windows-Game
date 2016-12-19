#include "Desktop.h"
#include <iostream>

std::shared_ptr<std::vector<Desktop::CollisionLine>> Desktop::windowColliders = std::make_shared<std::vector<Desktop::CollisionLine>>();
std::vector<HWND> Desktop::windowsToIgnore = std::vector<HWND>();

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
	Desktop::windowColliders->push_back(tempCollider1);
	//lineCollider für linke seite des fensters
	CollisionLine tempCollider2;
	tempCollider2.blockedFromTopLeft = true;
	tempCollider2.blockedFromDownRight = false;
	tempCollider2.vertical = false;
	tempCollider2.startPoint = sf::Vector2i{ static_cast<int>(rect->left+7), static_cast<int>(rect->top) };
	tempCollider2.length = static_cast<int>((rect->bottom - 7) - rect->top);
	Desktop::windowColliders->push_back(tempCollider2);
	//lineCollider für rechte seite des fensters
	CollisionLine tempCollider3;
	tempCollider3.blockedFromTopLeft = false;
	tempCollider3.blockedFromDownRight = true;
	tempCollider3.vertical = false;
	tempCollider3.startPoint = sf::Vector2i{ static_cast<int>(rect->right-7), static_cast<int>(rect->top) };
	tempCollider3.length = static_cast<int>((rect->bottom - 7) - rect->top);
	Desktop::windowColliders->push_back(tempCollider3);
	DeleteObject(rect);
	delete rect;
	return TRUE;
}

void Desktop::update()
{
	windowColliders->clear();
	EnumWindows(EnumWindowsProc, NULL);
}


std::tuple<bool, sf::Vector2i> Desktop::collides(sf::IntRect box, sf::Vector2f velocity)
{
	for (size_t i = 0; i < windowColliders->size(); i++)
	{
		std::tuple<bool, sf::Vector2i> collisionInfo = (*windowColliders)[i].collides(box, velocity);
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