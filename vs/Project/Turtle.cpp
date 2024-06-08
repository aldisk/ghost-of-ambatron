#include "Turtle.h"

Engine::Turtle::Turtle(Sprite* sprite, float velocity)
{
	this->sprite = sprite;
	state = Engine::TurtleState::DIE;
	groundDur = 0;
	groundTime = 3000;
	vel = velocity;
	health = 3;
	deadCounter = 0;
}

Engine::Turtle::~Turtle()
{

}

void Engine::Turtle::Update(float deltaTime, Turtle* bullet)
{
	if (state == Engine::TurtleState::DIE) {
		return;
	}

	if (health <= 0) {
		sprite->PlayAnim("explode");
		deadCounter += deltaTime;

		if (deadCounter > 300) { SetDie(); }
	}

	float x = GetX();
	float y = GetY();
	float xVelocity = vel;

	if (Engine::TurtleState::SPAWN == state && (x <= -300)) {
		state = Engine::TurtleState::DIE;
	}

	if (state == Engine::TurtleState::GROUND) {
		xVelocity = 0;
		if (groundDur >= groundTime) {
			state = Engine::TurtleState::DIE;
			groundDur = 0;
		}
		groundDur += deltaTime;
	}

	x -= xVelocity * deltaTime;
	sprite->SetPosition(x, y);
	sprite->Update(deltaTime);

	if (bullet != NULL && shootTime > 4000) {
		bullet->SetSpawn()->SetPosition(x - 15, y - 20);

		shootTime = 0;
	}
	else { shootTime += deltaTime; }
}

void Engine::Turtle::Draw()
{
	if (state == Engine::TurtleState::DIE) {
		return;
	}

	sprite->Draw();
}


Engine::Turtle* Engine::Turtle::SetPosition(float x, float y)
{
    sprite->SetPosition(x, y);
	return this;
}

Engine::Turtle* Engine::Turtle::SetSpawn()
{
	this->state = Engine::TurtleState::SPAWN;
	this->health = 3;
	this->deadCounter = 0;
	this->sprite->PlayAnim("idle");
	return this;
}

Engine::Turtle* Engine::Turtle::SetDie()
{
	this->state = Engine::TurtleState::DIE;
	return this;
}

Engine::Turtle* Engine::Turtle::GetHit()
{
	this->health -= 1;
	return this;
}

float Engine::Turtle::GetWidth()
{
	return sprite->GetScaleWidth();
}

float Engine::Turtle::GetHeight()
{
	return sprite->GetScaleHeight();
}

bool Engine::Turtle::IsDie()
{
	return Engine::TurtleState::DIE == state;
}

float Engine::Turtle::GetX()
{
	return sprite->GetPosition().x;
}

float Engine::Turtle::GetY()
{
	return sprite->GetPosition().y;
}

