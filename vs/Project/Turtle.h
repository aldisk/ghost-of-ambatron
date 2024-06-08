#ifndef TURTLE_H
#define TURTLE_H

#include "Sprite.h"

namespace Engine {
	enum class TurtleState {
		SPAWN,
		GROUND,
		DIE
	};
	class Turtle
	{
	public:
		Turtle(Sprite* sprite, float velocity);
		Sprite* sprite = NULL;
		~Turtle();
		void Update(float deltaTime, Turtle* bullet);
		void Draw();
		Turtle* GetHit();
		Turtle* SetPosition(float x, float y);
		Turtle* SetSpawn();
		Turtle* SetDie();
		float GetWidth();
		float GetHeight();
		bool IsDie();
		float GetX();
		float GetY();
		int health;
		float deadCounter;
	protected:
		TurtleState state;
		float groundDur = 0, groundTime = 0, x = 0, y = 0, vel = 0, shootTime;
	};

}

#endif