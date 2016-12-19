#pragma once
#include "ProgramDefinition.h"

#ifdef PLAYER
#include "WLEntity.h"

class Player : public WLEntity
{
public:
	Player();
	~Player() {};
private:
	void start();
	void update(float);
	void changeAnimation(std::string);

	std::string currentAnimationTitle;
	bool done;
	sf::RenderWindow window;
	const float verticalAcceleration = 1300.f;
	const float maxVerticalVelocity = 600.f;
	const float friction = 2700.f;
	const float wallVelocity = 50.f;
	const float jumpPower = 500.f;
	const float maxJumpPower = 1100.f;
	const float jumpAcceleration = 2000.f;
	const float gravity = 2000.f;
	const float jumpCoolDown = 0.1f;
	const float colliderUpdate = 0.f;//go up for less processing power use
	const float timeForJumpAcceleration = 0.18f;
	const float wallJumpSpeed = 300.f;
	float timeSinceLastJump = 0.3f;
	float timeSinceLastColliderUpdate = 0.f;
	float timeSinceJumpStarted = 0.f;
	bool stoppedJumping = true;
	bool grounded = false;
	bool jumpKeyWasDown = false;
	sf::Vector2f position;
	sf::Vector2f velocity;
};
#endif