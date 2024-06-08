#ifndef INGAMESCREEN_H
#define INGAMESCREEN_H

#include "Screen.h"
#include "Turtle.h"
#include "Setting.h"
#include "Texture.h"
#include "Sprite.h"
#include "Music.h"
#include "Sound.h"
#include "Text.h"
#include <vector>
#include <string>
#include "ScreenManager.h"

namespace Engine {
	class InGameScreen :
		public Engine::Screen
	{
	public:
		InGameScreen();
		void Init();
		void Update();
		void Draw();
	private:
		Engine::Sprite* CreateSprite(Texture* texture);

		Shader* defaultSpriteShader;
		Quad* defaultQuad;
		Setting* setting;

		void SpawnObjects();
		vector<Turtle*> objects;
		vector<Turtle*> enemyBullets;
		vector<Turtle*> playerBullets;
		float spawnDuration = 0, maxSpawnTime = 0, numObjectsInPool = 0, numObjectPerSpawn = 0;
		float shootCooldown = 100;
		Sprite* playerSprite = NULL;
		Sprite* backgroundSprite = NULL;
		Music* music = NULL;;
		Sound* fireSound;
		Sound* explodeSound;
		Text* text = NULL;
		float yVelocity = 0, gravity = 0;
		bool jump = false, debug = false;

		vector<Sprite*> backgrounds, middlegrounds;
		void MoveLayer(vector<Sprite*>& bg, float speed);
		void DrawLayer(vector<Sprite*>& bg);
		void AddToLayer(vector<Sprite*>& bg, string name);
		float offset = 0;
		
		int score = 0, playerHealth = 3;

		Text* Score;
		Text* HealthText;
		Text* GameOver;
	};
}

#endif