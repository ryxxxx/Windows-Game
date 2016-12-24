#include "Menu.h"
#ifdef MENU
#include <iostream>

//Start executables
VOID startup(LPCTSTR lpApplicationName)
{
	// additional information
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// start the program up
	CreateProcess(lpApplicationName,   // the path
		NULL,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
		);
	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

Menu::Menu()
{
	animations["default"] = std::make_shared<Animation>("menu_default.png", 1, sf::Vector2i{ 1,1 }, false);
	animations["hover_play"] = std::make_shared<Animation>("menu_hover_play.png", 1, sf::Vector2i{ 1,1 }, false);
	animations["hover_settings"] = std::make_shared<Animation>("menu_hover_settings.png", 1, sf::Vector2i{ 1,1 }, false);
	animations["hover_exit"] = std::make_shared<Animation>("menu_hover_exit.png", 1, sf::Vector2i{ 1,1 }, false);
	currentAnimationTitle = "default";

	start();
}


Menu::~Menu()
{
}

void Menu::start()
{
	window.create(sf::VideoMode(0, 0), "Menu", sf::Style::None);
	window.setFramerateLimit(30);
	sf::Clock clock;
	float lastFrame = 0.f;
	while (!done)
	{
		float deltaTime = clock.getElapsedTime().asSeconds() - lastFrame;
		lastFrame = clock.getElapsedTime().asSeconds();
		animations[currentAnimationTitle]->update(deltaTime);
		window.clear();
		animations[currentAnimationTitle]->draw(window);
		window.display();
		update(deltaTime);
	}
}

void Menu::update(float deltaTime)
{
	sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) && wasPressed)
	{
		mouseReleased = true;
	}
	else
	{
		mouseReleased = false;
	}

	if (playButtonCollider.contains(mousePosition) && !dragging)
	{
		currentAnimationTitle = "hover_play";
		if (mouseReleased)
		{
			std::string currentLevel = "Player";
			std::string directory = "..\\" + currentLevel + "\\" + currentLevel + ".exe";
			startup(directory.c_str());
			window.close();
			done = true;
		}
	}
	else if (settingsButtonCollider.contains(mousePosition) && !dragging)
	{
		currentAnimationTitle = "hover_settings";
		if (mouseReleased)
		{
			ShellExecute(NULL, "open", "..\\options.txt", NULL, NULL, SW_SHOW);
		}
	}
	else if (exitButtonCollider.contains(mousePosition) && !dragging)
	{
		currentAnimationTitle = "hover_exit";
		if (mouseReleased)
		{
			window.close();
			done = true;
		}
	}
	else
	{
		currentAnimationTitle = "default";
		if (sf::IntRect{ 0,0,static_cast<int>(window.getSize().x),static_cast<int>(window.getSize().y) }.contains(mousePosition) || dragging)
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !wasPressed)
			{
				grabOffset = sf::Mouse::getPosition() - window.getPosition();
				dragging = true;
			}
			else if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				window.setPosition(sf::Mouse::getPosition() - grabOffset);
			}
			else if(!wasPressed && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				dragging = false;
			}
		}
	}
	
	handleEvents();

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		wasPressed = true;
	}
	else
	{
		wasPressed = false;
	}
}

void Menu::handleEvents()
{
	sf::Event e;
	while (window.pollEvent(e))
	{
		switch (e.type)
		{
		case sf::Event::Closed:
			window.close();
			done = true;
			break;
		default:
			break;
		}
	}
}
#endif