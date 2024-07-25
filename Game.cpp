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
	std::cout << "Initializing game with config file: " << path << std::endl;

	std::ifstream fin;
	std::string objName;
	int width, height, frameLimit, fullscreen;
	fin.open(path);

	if (!fin.is_open()) {
		std::cerr << "Error: Could not open config file: " << path << std::endl;
		return;
	}

	while (fin >> objName)
	{
		if (objName == "Window") {
			
			fin >> width >> height >> frameLimit >> fullscreen;
			//set up defalut window parameters
			//m_window.setSize(sf::Vector2u(width, height));
			
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

	if (fullscreen) {
		m_window.create(sf::VideoMode(width, height), "ShapeGame", sf::Style::Fullscreen);
	}
	else
		m_window.create(sf::VideoMode(width, height), "ShapeGame");
	m_window.setFramerateLimit(frameLimit);

	ImGui::SFML::Init(m_window);

	spawnPlayer();

	
}

void Game::run()
{
	//todo: add pause functionality in here
	// some systems should function while paused (rendering)
	// some systems shouldn't (movement/input)

	std::cout << "Starting game loop" << std::endl;


	m_running = m_window.isOpen();
	
	
	while (m_running)
	{
		std::cout << "Polling events" << std::endl;
		sf::Event event;
		while (m_window.pollEvent(event)) {
			std::cout << "Processing event" << std::endl;
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed) {
				m_window.close();
				m_running = false;
			}
		}

		if (!m_window.isOpen()) {
			std::cout << "Window is not open, exiting game loop" << std::endl;
			m_running = false;
			break;
		}

		update();
		std::cout << "Entity manager updated" << std::endl;

		ImGui::SFML::Update(m_window, m_deltaClock.restart());
		std::cout << "ImGui update completed" << std::endl;

		sEnemySpawner();
		std::cout << "sEnemySpawner completed" << std::endl;

		sMovement();
		std::cout << "sMovement completed" << std::endl;

		sCollision();
		std::cout << "sCollision completed" << std::endl;

		sUserInput();
		std::cout << "sUserInput completed" << std::endl;

		sGUI();
		std::cout << "sGUI completed" << std::endl;

		sRender();
		std::cout << "sRender completed" << std::endl;

		if (!m_window.isOpen()) {
			std::cout << "Window is not open at the end of the loop, exiting game loop" << std::endl;
			m_running = false;
		}
		//increment the current frame	
		//may need to be moved when spawn is implemented
		m_currentFrame++;
	}

	std::cout << "Exiting game loop" << std::endl;
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
	//entity->cTransform->pos = Vec2(m_window.getSize().x / 2, m_window.getSize().y / 2);

	//entity->cTransform->pos.normalize();

	//give entity a collision component
	entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

	// the entity's shape
	entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT );
	
	// add an input component
	entity->cInput = std::make_shared<CInput>();

	m_player = entity;
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

	enemy->cTransform = std::make_shared<CTransform>(Vec2(ranX, ranY), Vec2(ranSx, ranSy), 12);
	//setting up enemy transform
	enemy->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);

	if (ranX - m_enemyConfig.SR >= 0 && ranX + m_enemyConfig.SR <= m_window.getSize().x &&
		ranY - m_enemyConfig.SR >= 0 && ranY + m_enemyConfig.SR <= m_window.getSize().y)
	{
		


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

		sEnemy->cCollision = std::make_shared<CCollision>(e->cCollision->radius * 0.5f);

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
	bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
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
		if (entity->cInput)
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

	ImGui::Begin("Test Run");
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
	ImGui::End();
	// Below is the information on entities
	
}

void Game::sRender()
{
	m_window.clear();
	//TODO: Draw all entities
	for (auto& e : m_entities.getEntities())
	{
		std::cout << "sRender: Checking entity with ID: " << e->id() << std::endl;

		// Check if the entity is active
		if (e->isActive())
		{
			std::cout << "sRender: Entity is active, rendering entity with ID: " << e->id() << std::endl;

			// Attempt to draw the entity's shape
			if (e->cShape && e->cTransform)
			{
				std::cout << "sRender: Drawing shape for entity with ID: " << e->id() << std::endl;
				e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
				e->cTransform->angle += 1.0f;
				e->cShape->circle.setRotation(e->cTransform->angle);
				m_window.draw(e->cShape->circle);
				std::cout << "sRender: Successfully drew shape for entity with ID: " << e->id() << std::endl;
			}
			else
			{
				std::cout << "sRender: Entity with ID: " << e->id() << " has no shape component" << std::endl;
			}
		}
		else
		{
			std::cout << "sRender: Entity with ID: " << e->id() << " is not active" << std::endl;
		}
	}
	ImGui::SFML::Render(m_window);
	std::cout << "ImGui rendered" << std::endl;

	m_window.display();
}

void Game::sUserInput()
{
	auto playerInput = m_player->cInput;

	if (playerInput)
	{
		playerInput->up = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
		std::cout << "Player input up: " << playerInput->up << std::endl;

		playerInput->left = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
		std::cout << "Player input left: " << playerInput->left << std::endl;

		playerInput->right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
		std::cout << "Player input right: " << playerInput->right << std::endl;

		playerInput->down = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
		std::cout << "Player input down: " << playerInput->down << std::endl;

		playerInput->shoot = sf::Mouse::isButtonPressed(sf::Mouse::Left);
		std::cout << "Player input shoot: " << playerInput->shoot << std::endl;
	} else {
		std::cout << "Player has no input component" << std::endl;
	}

}