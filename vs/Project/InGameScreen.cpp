#include "InGameScreen.h"

Engine::InGameScreen::InGameScreen() 
{
}

void Engine::InGameScreen::Init()
{
	// Spawn setting
	maxSpawnTime = 2000;
	numObjectPerSpawn = 2;
	numObjectsInPool = 100;

	vec2 start = vec2(200, 90);
	defaultSpriteShader = game->GetDefaultSpriteShader();
	defaultQuad = game->GetDefaultQuad();
	setting = game->GetSettings();

	Texture* playerTexture = new Texture("enemyship.png");
	playerSprite = new Sprite(playerTexture, defaultSpriteShader, defaultQuad);

	playerSprite->SetNumXFrames(3)->SetNumYFrames(2)->AddAnimation("hit", 2, 4)
		->AddAnimation("idle", 0, 2)->AddAnimation("explode", 3, 5)->PlayAnim("idle")->SetScale(1.0f)->SetAnimationDuration(100);

	playerSprite->SetPosition(0, setting->screenHeight / 2);

	Texture* bgTexture = new Texture("parallax-space-backgound.png");
	backgroundSprite = (new Sprite(bgTexture, defaultSpriteShader, defaultQuad))->SetSize((float)setting->screenWidth, (float)setting->screenHeight);

	// Load a texture
	Texture* texture = new Texture("enemyship.png");
	for (int i = 0; i < numObjectsInPool; i++) {
		Turtle* o = new Turtle(CreateSprite(texture), 0.07f);
		objects.push_back(o);
	}

	Texture* rocketTexture = new Texture("rocket.png");
	for (int i = 0; i < numObjectsInPool * 3; i++) {
		Sprite* o = new Sprite(rocketTexture, defaultSpriteShader, defaultQuad);
		o->SetNumXFrames(4)->SetNumYFrames(1)->SetAnimationDuration(100)->AddAnimation("firing", 0, 3);
		o->SetRotation(90.0f);
		o->PlayAnim("firing");
		o->SetScale(3.0f);
		Turtle* bullet = new Turtle(o, 0.15f);
		enemyBullets.push_back(bullet);
	}

	for (int i = 0; i < numObjectsInPool * 3; i++) {
		Sprite* o = new Sprite(rocketTexture, defaultSpriteShader, defaultQuad);
		o->SetNumXFrames(4)->SetNumYFrames(1)->SetAnimationDuration(100)->AddAnimation("firing", 0, 3);
		o->SetRotation(270.0f);
		o->PlayAnim("firing");
		o->SetScale(3.0f);
		Turtle* bullet = new Turtle(o, -0.2f);
		playerBullets.push_back(bullet);
	}

	AddToLayer(backgrounds, "parallax-space-stars.png");

	AddToLayer(middlegrounds, "parallax-space-far-planets.png");

	Score = (new Text("8-bit Arcade In.ttf", 30, game->GetDefaultTextShader()))
		->SetText(to_string(score))->SetPosition(game->GetSettings()->screenWidth * 0.5f, 100)->SetColor(255, 255, 255);
	HealthText = (new Text("8-bit Arcade In.ttf", 30, game->GetDefaultTextShader()))
		->SetText("3/3")->SetPosition(game->GetSettings()->screenWidth * 0.5f, 50)->SetColor(255, 255, 255);

	GameOver = (new Text("8-bit Arcade In.ttf", 100, game->GetDefaultTextShader()))
		->SetText("GAME OVER")->SetPosition(game->GetSettings()->screenWidth * 0.4f, game->GetSettings()->screenHeight * 0.5f)->SetColor(255, 255, 255);

	music = (new Music("warped-shooting-fx.ogg"))->SetVolume(10)->Play(true);

	fireSound = (new Sound("laser.wav"))->SetVolume(30);

	explodeSound = (new Sound("explode.wav"))->SetVolume(30);

	// Add input mappings
	game->GetInputManager()->AddInputMapping("mainmenu", SDLK_ESCAPE);

	game->GetInputManager()->AddInputMapping("R", SDLK_d)->AddInputMapping("L", SDLK_a)->AddInputMapping("U", SDLK_w)->AddInputMapping("D", SDLK_s)->AddInputMapping("Attack", SDLK_x);

}

void Engine::InGameScreen::Update()
{
	// Back to main menu
	if (game->GetInputManager()->IsKeyReleased("mainmenu")) {
		ScreenManager::GetInstance(game)->SetCurrentScreen("mainmenu");
	}

	int xMod = 0;
	int yMod = 0;

	// Player movement
	if (game->GetInputManager()->IsKeyPressed("R") && playerSprite->GetPosition().x < setting->screenWidth*0.5) {
		xMod += 1;
	}

	if (game->GetInputManager()->IsKeyPressed("L") && playerSprite->GetPosition().x > 0) {
		xMod -= 1;
	}

	if (game->GetInputManager()->IsKeyPressed("U") && playerSprite->GetPosition().y < setting->screenHeight - 96) {
		yMod += 1;
	}

	if (game->GetInputManager()->IsKeyPressed("D") && playerSprite->GetPosition().y >  0) {
		yMod -= 1;
	}

	if (playerHealth > 0) {
		playerSprite->SetPosition(playerSprite->GetPosition().x + (game->GetGameTime() * xMod * 0.15), playerSprite->GetPosition().y + (game->GetGameTime() * yMod * 0.2));
	}

	if (playerHealth <= 0) {
		playerSprite->PlayAnim("explode");
	}

	shootCooldown += game->GetGameTime();

	if (game->GetInputManager()->IsKeyPressed("Attack") && shootCooldown >= 500 && playerHealth > 0) {
		Turtle* bullet = NULL;

		for (Turtle* bullets : playerBullets) {
			if (bullets->IsDie()) {
				bullet = bullets;
			}
		}

		bullet->SetSpawn()->SetPosition(playerSprite->GetPosition().x + 180 + 15, playerSprite->GetPosition().y - 20);

		shootCooldown = 0;

		fireSound->Play(false);
	}

	// Time to spawn objects
	if (spawnDuration >= maxSpawnTime) {
		SpawnObjects();
		spawnDuration = 0;
	}

	// Update all objects
	playerSprite->Update(game->GetGameTime());

	for (Turtle* o : objects) {
		Turtle* bullet = NULL;
		int spawnCount = 0;

		for (Turtle* bullets : enemyBullets) {
			if (spawnCount == 4) {
				break;
			}
			if (bullets->IsDie()) {
				bullet = bullets;
				spawnCount++;
			}
		}

		if (o->GetX() > (game->GetSettings()->screenWidth * 0.9) || o->GetX() < (game->GetSettings()->screenWidth * 0.1)) {
			bullet = NULL;
		}

		if (o->sprite->GetBoundingBox()->CollideWith(playerSprite->GetBoundingBox()) && !o->IsDie() && playerHealth > 0) {
			playerHealth -= 1;
			o->GetHit();
			explodeSound->Play(false);
		}

		o->Update((game->GetGameTime()), bullet);
	}

	for (Turtle* o : enemyBullets) {
		if (o->sprite->GetBoundingBox()->CollideWith(playerSprite->GetBoundingBox()) && !o->IsDie() && playerHealth > 0) {
			playerHealth -= 1;
			o->SetDie();
			explodeSound->Play(false);
		}
		o->Update((game->GetGameTime()), NULL);
	}

	for (Turtle* o : playerBullets) {
		for (Turtle* enemy : objects) {
			if (o->sprite->GetBoundingBox()->CollideWith(enemy->sprite->GetBoundingBox()) && !o->IsDie() && !enemy->IsDie()) {
				enemy->GetHit();
				o->SetDie();
				explodeSound->Play(false);

				score += 20;
			}
		}
		o->Update((game->GetGameTime()), NULL);
	}

	// Count spawn duration
	spawnDuration += (game->GetGameTime());

	Score = (new Text("8-bit Arcade In.ttf", 30, game->GetDefaultTextShader()))
		->SetText(to_string(score))->SetPosition(game->GetSettings()->screenWidth * 0.5f, 100)->SetColor(255, 255, 255);

	HealthText = (new Text("8-bit Arcade In.ttf", 30, game->GetDefaultTextShader()))
		->SetText(to_string(playerHealth) + " Health")->SetPosition(game->GetSettings()->screenWidth * 0.5f - 50, 50)->SetColor(255, 255, 255);

	MoveLayer(backgrounds, 0.005f);
	MoveLayer(middlegrounds, 0.03f);
}

void Engine::InGameScreen::Draw()
{
	backgroundSprite->Draw();
	DrawLayer(backgrounds);
	DrawLayer(middlegrounds);
	Score->Draw();
	HealthText->Draw();

	if (playerHealth <= 0) {
		GameOver->Draw();
	}

	playerSprite->Draw();

	// Render all objects
	for (Turtle* o : objects) {
		o->Draw();
	}

	for (Turtle* o : enemyBullets) {
		o->Draw();
	}

	for (Turtle* o : playerBullets) {
		o->Draw();
	}
}

Engine::Sprite* Engine::InGameScreen::CreateSprite(Texture* texture)
{
	return (new Sprite(texture, game->GetDefaultSpriteShader(), game->GetDefaultQuad()))
		->SetNumXFrames(3)->SetNumYFrames(2)->AddAnimation("hit", 2, 4)
		->AddAnimation("idle", 0, 2)->SetFlipHorizontal(true)
		->AddAnimation("explode", 3, 5)->PlayAnim("idle")->SetScale(1.0f)->SetAnimationDuration(100);
}

void Engine::InGameScreen::SpawnObjects()
{
	//Find Die object to reuse for spawning
	int spawnCount = 0;
	for (Turtle* o : objects) {
		if (spawnCount == numObjectPerSpawn) {
			break;
		}
		if (o->IsDie()) {
			// Set state to spawn
			
			// Random spawn position
			int min = 0;
			int max = (int) (game->GetSettings()->screenWidth - o->GetHeight());
			float x = game->GetSettings()->screenWidth + o->GetWidth();
			float y = (float)(rand() % (max - min + 1) + min);
			o->SetSpawn()->SetPosition(x, y);
			spawnCount++;
		}
	}
}

void Engine::InGameScreen::MoveLayer(vector<Sprite*>& bg, float speed)
{
	for (Sprite* s : bg) {
		if (s->GetPosition().x < -setting->screenWidth + offset) {
			s->SetPosition(setting->screenWidth + offset - 1, 0);
		}
		s->SetPosition(s->GetPosition().x - speed * game->GetGameTime(), s->GetPosition().y);
		s->Update(game->GetGameTime());
	}
}

void Engine::InGameScreen::DrawLayer(vector<Sprite*>& bg)
{
	for (Sprite* s : bg) {
		s->Draw();
	}
}

void Engine::InGameScreen::AddToLayer(vector<Sprite*>& bg, string name)
{
	Texture* texture = new Texture(name);

	Sprite* s = new Sprite(texture, defaultSpriteShader, defaultQuad);
	s->SetSize(setting->screenWidth + offset, setting->screenHeight);
	bg.push_back(s);

	Sprite* s2 = new Sprite(texture, defaultSpriteShader, defaultQuad);
	s2->SetSize(setting->screenWidth + offset, setting->screenHeight)->SetPosition(setting->screenWidth + offset - 1, 0);
	bg.push_back(s2);
}

