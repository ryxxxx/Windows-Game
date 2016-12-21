#include "Player.h"

#ifdef PLAYER
#include "Desktop.h"
#include <iostream>

Player::Player()
{
	animations["idle"] = std::make_shared<Animation>("idle.png", 1, sf::Vector2i{ 2,1 }, true);
	animations["run_left"] = std::make_shared<Animation>("run_left.png", 0.1f, sf::Vector2i{ 4,1 }, true);
	animations["run_right"] = std::make_shared<Animation>("run_right.png", 0.1f, sf::Vector2i{ 4,1 }, true);
	animations["jump_left"] = std::make_shared<Animation>("jump_left.png", 1.25f, sf::Vector2i{ 3,1 }, false);
	animations["jump_right"] = std::make_shared<Animation>("jump_right.png", 1.25f, sf::Vector2i{ 3,1 }, false);
	animations["fall_left"] = std::make_shared<Animation>("fall_left.png", 100.f, sf::Vector2i{ 1,1 }, true);
	animations["fall_right"] = std::make_shared<Animation>("fall_right.png", 100.f, sf::Vector2i{ 1,1 }, true);

	currentAnimationTitle = "idle";
	start();
}

void Player::start()
{
	done = false;
	window.create(sf::VideoMode(0, 0), "", sf::Style::None);
	window.setFramerateLimit(60);
	Desktop::ignoreWindow(&window);
	position = sf::Vector2f(window.getPosition().x, window.getPosition().y);
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

void Player::update(float dt)
{
	window.getSize();
	sf::IntRect windowRect{ window.getPosition().x, window.getPosition().y,static_cast<int>(window.getSize().x),static_cast<int>(window.getSize().y) };
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && windowRect.contains(sf::Mouse::getPosition()))
	{
		isDragged = true;
		dragOffset = sf::Vector2i(-windowRect.left + sf::Mouse::getPosition().x, -windowRect.top + sf::Mouse::getPosition().y);
	}
	else if(!sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		isDragged = false;
	}
	sf::Vector2i tempSize = sf::Vector2i(static_cast<int>(window.getSize().x), static_cast<int>(window.getSize().y));
	std::tuple<bool, sf::Vector2i> groundCollision = Desktop::collides(sf::IntRect{ static_cast<int>(position.x >= 0 ? position.x + 0.5 : position.x - 0.5),static_cast<int>(position.y >= 0 ? position.y + 0.5 : position.y - 0.5) + 1,tempSize.x,tempSize.y }, sf::Vector2f(0, 1));
	if (std::get<bool>(groundCollision))
		grounded = true;
	else
		grounded = false;
	std::tuple<bool, sf::Vector2i> atWallRight = Desktop::collides(sf::IntRect{ static_cast<int>(position.x >= 0 ? position.x + 0.5 : position.x - 0.5) + 1,static_cast<int>(position.y >= 0 ? position.y + 0.5 : position.y - 0.5),tempSize.x,tempSize.y }, sf::Vector2f(1, 0));
	std::tuple<bool, sf::Vector2i> atWallLeft = Desktop::collides(sf::IntRect{ static_cast<int>(position.x >= 0 ? position.x + 0.5 : position.x - 0.5) - 1,static_cast<int>(position.y >= 0 ? position.y + 0.5 : position.y - 0.5),tempSize.x,tempSize.y }, sf::Vector2f(-1, 0));
	bool atWall = false;
	if (std::get<bool>(atWallRight) || std::get<bool>(atWallLeft))
		atWall = true;
	//movement on ground
	if (grounded)
	{
		//move left
		if (sf::Keyboard::isKeyPressed(leftKey))
		{
			if (velocity.x > 0.f)
				velocity.x = velocity.x - friction*dt - verticalAcceleration*dt;
			else if (velocity.x > -maxVerticalVelocity)
				velocity.x = velocity.x - verticalAcceleration*dt;
			else
				velocity.x = -maxVerticalVelocity;
		}
		//move right
		else if (sf::Keyboard::isKeyPressed(rightKey))
		{
			if (velocity.x < 0.f)
				velocity.x = velocity.x + friction*dt + verticalAcceleration*dt;
			else if (velocity.x < maxVerticalVelocity)
				velocity.x = velocity.x + verticalAcceleration*dt;
			else
				velocity.x = maxVerticalVelocity;
		}
		//dont move
		else
		{
			if (velocity.x > maxVerticalVelocity / 10)
				velocity.x -= friction*dt;
			else if (velocity.x < -maxVerticalVelocity / 10)
				velocity.x += friction*dt;
			else
				velocity.x = 0.f;
		}
		//jump
		timeSinceLastJump += dt;
	}
	else
	{
		//move left
		if (sf::Keyboard::isKeyPressed(leftKey))
		{
			if (velocity.x > -maxVerticalVelocity)
				velocity.x = velocity.x - verticalAcceleration*dt / 2;
			else
				velocity.x = -maxVerticalVelocity;
		}
		//move right
		else if (sf::Keyboard::isKeyPressed(rightKey))
		{
			if (velocity.x < maxVerticalVelocity)
				velocity.x = velocity.x + verticalAcceleration*dt / 2;
			else
				velocity.x = maxVerticalVelocity;
		}
		velocity.y += gravity*dt;
		if (atWall && velocity.y>0.f)
		{
			velocity.y = wallVelocity;
		}
	}
	//jump
	timeSinceJumpStarted += dt;
	if (sf::Keyboard::isKeyPressed(upKey))
	{
		if (atWall && !jumpKeyWasDown)
		{
			timeSinceLastJump = 0;
			timeSinceJumpStarted = 0;
			velocity.y = -jumpPower*0.8f;
			stoppedJumping = false;
			if (std::get<bool>(atWallLeft))
				velocity.x = wallJumpSpeed;
			else
				velocity.x = -wallJumpSpeed;
		}
		else if (grounded && timeSinceLastJump>jumpCoolDown)
		{
			timeSinceLastJump = 0;
			timeSinceJumpStarted = 0;
			velocity.y = -jumpPower;
			stoppedJumping = false;
		}
		else if (timeSinceJumpStarted < timeForJumpAcceleration && velocity.y > -maxJumpPower && !stoppedJumping)
		{
			velocity.y -= jumpAcceleration*dt;
		}
		jumpKeyWasDown = true;
	}
	else
	{
		stoppedJumping = true;
		jumpKeyWasDown = false;
	}
	//collision
	timeSinceLastColliderUpdate += dt;
	if (timeSinceLastColliderUpdate > colliderUpdate)
	{
		Desktop::update();
		timeSinceLastColliderUpdate = 0.f;
	}
	sf::Vector2f tempPos{ position.x + velocity.x*dt, position.y };
	std::tuple<bool,sf::Vector2i> collidesX = Desktop::collides(sf::IntRect{ static_cast<int>(tempPos.x >= 0 ? tempPos.x + 0.5 : tempPos.x - 0.5),static_cast<int>(tempPos.y >= 0 ? tempPos.y + 0.5 : tempPos.y - 0.5),tempSize.x,tempSize.y }, velocity);
	bool runsLeft = false, runsRight = false;
	if (std::get<bool>(collidesX))
	{
		if (velocity.x < 0.f)
			runsLeft = true;
		else
			runsRight = true;
		position = sf::Vector2f(std::get<sf::Vector2i>(collidesX).x, std::get<sf::Vector2i>(collidesX).y);
		velocity.x = 0.f;
	}
	else
	{
		position.x = tempPos.x;
	}
	tempPos.y += velocity.y*dt;
	std::tuple<bool, sf::Vector2i> collidesY = Desktop::collides(sf::IntRect{ static_cast<int>(tempPos.x >= 0 ? tempPos.x + 0.5 : tempPos.x - 0.5),static_cast<int>(tempPos.y >= 0 ? tempPos.y + 0.5 : tempPos.y - 0.5),tempSize.x,tempSize.y }, velocity);
	if (std::get<bool>(collidesY))
	{
		position = sf::Vector2f(std::get<sf::Vector2i>(collidesY).x, std::get<sf::Vector2i>(collidesY).y);
		velocity.y = 0.f;
	}
	else
	{
		position.y = tempPos.y;
	}
	//animation state
	if (grounded)
	{
		if (velocity.x > 0.01f || runsRight)
			changeAnimation("run_right");
		else if (velocity.x < -0.01f || runsLeft)
			changeAnimation("run_left");
		else
			changeAnimation("idle");
	}
	else
	{
		if (velocity.y>0.f)
		{
			if (velocity.x > 0 || runsRight)
				changeAnimation("fall_right");
			else
				changeAnimation("fall_left");
		}
		else
		{
			if (velocity.x > 0 || runsRight)
				changeAnimation("jump_right");
			else
				changeAnimation("jump_left");
		}
	}
	if (!isDragged)
		window.setPosition(sf::Vector2i(static_cast<int>(position.x + 0.5), static_cast<int>(position.y + 0.5)));
	else
	{
		window.setPosition(sf::Mouse::getPosition() - dragOffset);
		position = sf::Vector2f(sf::Mouse::getPosition().x - dragOffset.x, sf::Mouse::getPosition().y - dragOffset.y);
		velocity.x = 0;
		velocity.y = 0;
		changeAnimation("idle");
	}
	::SetWindowPos(window.getSystemHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
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

void Player::changeAnimation(std::string title)
{
	if (title != currentAnimationTitle)
	{
		animations[currentAnimationTitle]->reset();
		currentAnimationTitle = title;
	}
}
#endif PLAYER