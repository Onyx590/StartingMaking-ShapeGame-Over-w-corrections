#include "Game.h"

#include <iostream>


Game::Game(const std::string& config)
{
	init(config);
}






void Game::init(const std::string& path)
{
	// todo: read in config file here
	// use the premade player config, enemyConfig, BullitConfig variables
	std::ifstream fin;
	std::string objName;
	fin.open(path);
	while (fin >> objName)
	{
		if (objName == "Window") {
			int width, height, frameLimit, fullscreen;
			fin >> width >> height >> frameLimit >> fullscreen;
			//set up defalut window parameters
			
			if (fullscreen) {
				m_window.create(sf::VideoMode(width, height), "Assignment 2", sf::Style::Fullscreen);
			}
			else
				m_window.create(sf::VideoMode(width, height), "Assignment 2");
			m_window.setFramerateLimit(frameLimit);
		}
		else if (objName == "Player") {
			fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
			
		}
		else if (objName == "Enemy") {
			fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
			
		}
		else if (objName == "Bullet") {
			fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
		}
	}

	

	ImGui::SFML::Init(m_window);

	spawnPlayer();

	run();
}

void Game::run()
{
	//todo: add pause functionality in here
	// some systems should function while paused (rendering)
	// some systems shouldn't (movement/input)

	m_running = m_window.isOpen();

	while (m_running)
	{
		sf::Event event;
		while (m_window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed)
				m_window.close();
		}

		//required update call to imgui
		ImGui::SFML::Update(m_window, m_deltaClock.restart());

		sGUI();

		//update the entity manager
		update();
		sEnemySpawner();
		sMovement();
		sCollision();
		sUserInput();
		
		sRender();

		//increment the current frame	
		//may need to be moved when spawn is implemented
	}
}

void Game::setPaused(bool paused)
{
	
	//todo
	m_paused = paused;
}

void Game::update()
{
	m_entities.update();
}

//respawn player in the middle of the screen
void Game::spawnPlayer()
{
	// todo: finish adding all properties of the player with the correct values from 

	//we create every entity by calling EntityManager.addEntity(tag)
	// This returns a std::shared_ptr<Entity>, so we use 'auto' to save typing
	auto entity = m_entities.addEntity("Player");

	//give entity a transform
	entity->cTransform = std::make_shared<CTransform>();

	// the entity's shape
	entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT );
	
	// add an input component
	entity->cInput = std::make_shared<CInput>();
}

void Game::spawnEnemy()
{
	// TODO: make sure the enemy is spawned properly with the m_enemyConfig variables
	// the enemy must be spawned completely within the bounds of the window
	auto enemy = m_entities.addEntity("Enemy");

	std::srand(static_cast<unsigned>(std::time(nullptr)));

	float ranX = static_cast<float>(std::rand() % static_cast<int>(m_window.getSize().x));
	float ranY = static_cast<float>(std::rand() % static_cast<int>(m_window.getSize().y));

	int ranV = static_cast<int>(std::rand() % (m_enemyConfig.VMAX - m_enemyConfig.VMIN + 1) + m_enemyConfig.VMIN);

	float ranSx = static_cast<float>(std::rand() % static_cast<int>(m_enemyConfig.SMAX - m_enemyConfig.SMIN + 1) + m_enemyConfig.SMIN);
	float ranSy = static_cast<float>(std::rand() % static_cast<int>(m_enemyConfig.SMAX - m_enemyConfig.SMIN + 1) + m_enemyConfig.SMIN);

	int ranCR = static_cast<int>(std::rand() % 255);
	int ranCG = static_cast<int>(std::rand() % 255);
	int ranCB = static_cast<int>(std::rand() % 255);

	if (ranX - m_enemyConfig.SR >= 0 && ranX + m_enemyConfig.SR <= m_window.getSize().x &&
		ranY - m_enemyConfig.SR >= 0 && ranY + m_enemyConfig.SR <= m_window.getSize().y)
	{
		enemy->cTransform = std::make_shared<CTransform>(Vec2(ranX, ranY), Vec2(ranSx, ranSy), 12);


		enemy->cShape = std::make_shared<CShape>(m_enemyConfig.SR, ranV, sf::Color(ranCR, ranCG, ranCB), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);

		//enemy->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);

		enemy->cScore = std::make_shared<CScore>(ranV * 100);

		auto m_lastEnemySpawnTime = m_currentFrame;

	}

	

	//record when the most recent enemy was spawned
	//m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
	//TODO: spawn small enemies at the location of the input enemy e

	//when we create the smaller enemy, we have to read the values of the original enemy
	//- spawn a number of small enemies equal to the vertices of the original enemy
	// - set each small enemy to the same color as teh original, half the size
	//- small enemies are worth double points of the original enemy

	int vertices = e->cShape->circle.getPointCount();

	for (int i = 1; i < (vertices + 1); i++)
	{
		auto sEnemy = m_entities.addEntity("Enemy");

		sEnemy->cTransform = std::make_shared<CTransform>(e->cTransform->pos, e->cTransform->velocity, 360 / i);

		float smallSize = e->cShape->circle.getRadius() * 0.5f;

		sEnemy->cShape = std::make_shared<CShape>(smallSize, vertices, e->cShape->circle.getFillColor(), e->cShape->circle.getOutlineColor(), e->cShape->circle.getOutlineThickness());

		int smallScore = e->cScore->score * 2;
		sEnemy->cScore = std::make_shared<CScore>(smallScore);

		sEnemy->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
	}
}

// spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
	//TODO: implement the spawning of a bullet which travels toward target
	//  - bullet speed is given as a scalar speed
	// - you must set the velocity by using formula in notes
	auto bullet = m_entities.addEntity("Bullet");
	Vec2 velocity = target - entity->cTransform->pos;
	float angle = atan2f(velocity.y, velocity.x);
	

	bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);
	bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, velocity, angle);
	bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
	

}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
	//todo: implement your own special weapon
}

void Game::sMovement()
{
	//todo: implement all entity movement in this function
	// you should read the m_player->cInput component to determine if the player is moving

	for (auto& entity : m_entities.getEntities())
	{
		if (entity->tag() == m_player.tag())
		{
			if (entity->cInput->up)
			{
				entity->cTransform->pos.y -= entity->cTransform->velocity.y * m_playerConfig.S;
			}

			if (entity->cInput->left)
			{
				entity->cTransform->pos.x -= entity->cTransform->velocity.x * m_playerConfig.S;
			}

			if (entity->cInput->right)
			{
				entity->cTransform->pos.x += entity->cTransform->velocity.x * m_playerConfig.S;
			}

			if (entity->cInput->down)
			{
				entity->cTransform->pos.y += entity->cTransform->velocity.y * m_playerConfig.S;
			}
		}

		if (entity->tag() == "Enemy")
		{
			entity->cTransform->pos.x += entity->cTransform->velocity.x * m_enemyConfig.SMIN;
			if (entity->cTransform->pos.x - entity->cCollision->radius < 0)
				{
					entity->cTransform->pos.x += entity->cTransform->velocity.x * -(m_enemyConfig.SMIN);
				}
			else if (entity->cTransform->pos.x + entity->cCollision->radius > m_window.getSize().x)
			{
				entity->cTransform->pos.x += entity->cTransform->velocity.x * -(m_enemyConfig.SMIN);
			}

			entity->cTransform->pos.y += entity->cTransform->velocity.y * m_enemyConfig.SMIN;
			if (entity->cTransform->pos.y - entity->cCollision->radius < 0)
				{
					entity->cTransform->pos.y += entity->cTransform->velocity.y * -(m_enemyConfig.SMIN);
				}
			else if (entity->cTransform->pos.y + entity->cCollision->radius > m_window.getSize().y)
			{
				entity->cTransform->pos.y += entity->cTransform->velocity.y * -(m_enemyConfig.SMIN);
			}
		}

		if (entity->tag() == "Bullet")
		{
			//implement path of bullet
			entity->cTransform->pos.x += entity->cTransform->velocity.x * m_bulletConfig.S;
			entity->cTransform->pos.y += entity->cTransform->velocity.y * m_bulletConfig.S;

			if (entity->cTransform->pos.x < 0 || entity->cTransform->pos.x > m_window.getSize().x || entity->cTransform->pos.y < 0 || entity->cTransform->pos.y > m_window.getSize().y)
			{
				entity->destroy();
			}
		}
	}

	//sample movement speed update
	//m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
	//m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
}

void Game::sLifespan()
{
	
	//todo: implement all lifespann functionality

	//for all entities
	//   if an entity has no lifespan component, skip it
	//  if an entity has > 0 remaining lifespan, subtract 1
	//  if it has lifespan and is alive
		// scale its alpha channel properly
	//	if it has lifespan and its  time is up
	//		destroy the entity

	for (auto& e : m_entities.getEntities())
	{
		if (e->cLifespan)
		{
			if (e->cLifespan->remaining > 0)
			{
				if (m_currentFrame % e->cLifespan->total == 0)
				{
					e->cLifespan->remaining--;

					float alphaScale = static_cast<float>(e->cLifespan->remaining) / static_cast<float>(e->cLifespan->total);
					sf::Color shapeColor = e->cShape->circle.getFillColor();
					shapeColor.a = static_cast<float>(alphaScale * 255);
					e->cShape->circle.setFillColor(shapeColor);
				}
			}
			else
			{
				e->destroy();
			}
		}
		
	}
}

void Game::sCollision()
{
	//TODO: implement all proper collisions between entities
	// use collision radius not shape radius

	for (auto& e : m_entities.getEntities())
	{
		if (e->cCollision)
		{
			for (auto& otherE : m_entities.getEntities())
			{
				if (e != otherE)
				{
					float dist = e->cTransform->pos.dist(otherE->cTransform->pos);
					if (dist < (e->cCollision->radius + otherE->cCollision->radius) * (e->cCollision->radius + otherE->cCollision->radius))
					{
						if (e->tag() == "Player" && otherE->tag() == "Enemy")
						{
							e->destroy();
							otherE->destroy();
							update();
							spawnPlayer();
						}
						else if (e->tag() == "Bullet" && otherE->tag() == "Enemy")
						{
							e->destroy();
							otherE->destroy();
							spawnSmallEnemies(otherE);
							update();
						}
						else
						{
							continue;
						}
					}
				}
			}
		}
	}
}

void Game::sEnemySpawner()
{
	//todo: code which implements enemy spawning should go here
	if (m_currentFrame % m_enemyConfig.SI == 0)
	{
		spawnEnemy(); // Spawn a new enemy
		
	}
}

void Game::sGUI()
{
	bool collisioncondi;

	ImGui::Begin("");
	ImGui::Text("Test run of Menu GUI");

	if (ImGui::TreeNode("System Dropdown"))
	{
		ImGui::Checkbox("Pause", &m_paused);
		ImGui::Checkbox("Collision System", &collisioncondi);

	}
	if (ImGui::TreeNode("Display Entities"))
	{
		ImGui::BeginChild("Scrolling");
		for (auto& entity : m_entities.getEntities())
		{
			std::string entityInfo = "ID: " + std::to_string(entity->id()) +
				", Tag: " + entity->tag() +
				", PosX: " + std::to_string(entity->cTransform->pos.x) +
				", PosY: " + std::to_string(entity->cTransform->pos.y);

			ImGui::Text("%s", entityInfo.c_str());
		}
		ImGui::EndChild();
	}
	ImGui::SliderInt("Enemy Spawn Interval", &m_enemyConfig.SI, 1, 10);
	static int clicked = 0;
	if (ImGui::Button("SpawnEnemy")) 
	{
		sEnemySpawner();
	}

	// Below is the information on entities
	
}

void Game::sRender()
{
	//TODO: Draw all entities
	for (auto& e : m_entities.getEntities())
	{
		if (e->isActive())
		{
			m_window.draw(e->cShape->circle);
		}
	}
}

void Game::sUserInput()
{
	auto playerInput = m_player.cInput;

	playerInput->up = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
	playerInput->left = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
	playerInput->right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
	playerInput->down = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);

	playerInput->shoot = sf::Mouse::isButtonPressed(sf::Mouse::Left);


}