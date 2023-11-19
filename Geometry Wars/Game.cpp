#include "Game.h"
#include "Util.h"

Game::Game(const std::string& config) {
	m_configReader.readFromFile(config);
	init();
}

void Game::init() {
	// TODO: read in config file here
	ConfigVariant cVariant = m_configReader.getData(ConfigReader::Type::Window);
	WindowConfig wConfig = std::get<WindowConfig>(cVariant);

	m_window.create(sf::VideoMode(wConfig.W, wConfig.H), "Geometry Wars", wConfig.FS ? sf::Style::Fullscreen : sf::Style::Default);
	m_window.setFramerateLimit(wConfig.FL);

	spawnPlayer();
}

void Game::run() {
	// TODO: add pause functionality in here
	//		 some systems should function while paused (rendering)
	//		 some systems shouldn't (movement / input)

	while (m_running) {
		m_entities.update();

		sEnemySpawner();
		sMovement();
		sCollision();
		sUserInput();
		sRender();

		// increase the current frame
		// may need to be moved when pause implemented
		m_currentFrame++;
	}
}

void Game::setPaused(bool paused) {
	// TODO
}

void Game::spawnPlayer() {
	ConfigVariant cVariant = m_configReader.getData(ConfigReader::Type::Player);
	PlayerConfig pConfig = std::get<PlayerConfig>(cVariant);

	std::shared_ptr<Entity> entity = m_entities.addEntity("player");

	sf::Color fillColor(pConfig.FR, pConfig.FG, pConfig.FB);
	sf::Color outlineColor(pConfig.OR, pConfig.OG, pConfig.OB);

	sf::Vector2u wSize = m_window.getSize();
	Vec2 pPos(wSize.x / 2.0f, wSize.y / 2.0f);

	entity->cTransform = std::make_shared<CTransform>(0.0f, pPos, Vec2(2.0f, 2.0f));
	entity->cShape = std::make_shared<CShape>(pConfig.SR, pConfig.V, fillColor, outlineColor, pConfig.OT);
	entity->cCollision = std::make_shared<CCollision>(pConfig.CR);
	entity->cInput = std::make_shared<CInput>();

	m_player = entity;
}

void Game::spawnEnemy() {
	ConfigVariant cVariant = m_configReader.getData(ConfigReader::Type::Enemy);
	EnemyConfig eConfig = std::get<EnemyConfig>(cVariant);

	std::shared_ptr<Entity> e = m_entities.addEntity("enemy");

	sf::Color fillColor(Util::generateRandomValFromRange(0, 255), Util::generateRandomValFromRange(0, 255), Util::generateRandomValFromRange(0, 255));
	sf::Color outlineColor(eConfig.OR, eConfig.OG, eConfig.OB);
	int vertices = Util::generateRandomValFromRange(eConfig.VMIN, eConfig.VMAX);

	sf::Vector2u wSize = m_window.getSize();
	float mx = Util::generateRandomValFromRange((float) eConfig.SR, (float) wSize.x - eConfig.SR);
	float my = Util::generateRandomValFromRange((float) eConfig.SR, (float) wSize.y - eConfig.SR);

	float speed = Util::generateRandomValFromRange((float) eConfig.SMIN, (float) eConfig.SMAX);

	e->cCollision = std::make_shared<CCollision>(eConfig.CR);
	e->cShape = std::make_shared<CShape>(eConfig.SR, vertices, fillColor, outlineColor, eConfig.OT);
	e->cTransform = std::make_shared<CTransform>(0, Vec2(mx, my), Vec2(speed, speed));

	// record when the most recent enemy was spawned
	m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e) {
	// TODO: spawn small enemies at the location of the input enemy e

	// when we create the smaller enemy, we have to read the values of the original enemy
	// - spawn a number of small enemies equal to the vertices of the original enemy
	// - set each small enemy to the same color as the original, half the size
	// - small enemies are worth the double points of the original enemy
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target) {
	// spawn enemy from the given entity to the target location

	// TODO: implement the spawning of a bullet which travels towards the target
	//		 - bullet speed is given as a scalar speed
	//		 - you must set the velocity by using the formula in the notes

	std::shared_ptr<Entity> e = m_entities.addEntity("bullet");
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {

}

void Game::sMovement() {
	// player movement

	ConfigVariant cVariant = m_configReader.getData(ConfigReader::Type::Player);
	PlayerConfig pConfig = std::get<PlayerConfig>(cVariant);

	if (m_player->cInput->up) {
		m_player->cTransform->pos.y -= pConfig.S;
	} else if (m_player->cInput->down) {
		m_player->cTransform->pos.y += pConfig.S;
	}

	if (m_player->cInput->left) {
		m_player->cTransform->pos.x -= pConfig.S;
	}
	else if (m_player->cInput->right) {
		m_player->cTransform->pos.x += pConfig.S;
	}

	// enemy movement
	for (std::shared_ptr<Entity>& e : m_entities.getEntities("enemy")) {
		e->cTransform->pos.x += e->cTransform->velocity.x;
		e->cTransform->pos.y += e->cTransform->velocity.y;
	}
}

void Game::sLifespan() {
	// TODO: implement all the lifespan functionality

	/**
	* for all the entities
	*		- if the entity has no lifespan component, skip it
	*		- if entity has > 0 remaining lifespan, substract 1
	*		- if it has lifespan and is alive
	*			scale its alpha channel properly
	*		- if it has lifespan and its time is up
	*			destory the entity
	*/
}

void Game::sCollision() {
	sf::Vector2u wSize = m_window.getSize();

	// player collision with walls
	if (m_player->cTransform->pos.x + m_player->cCollision->radius >= wSize.x) {
		m_player->cTransform->pos.x = wSize.x - m_player->cCollision->radius;
	}

	if (m_player->cTransform->pos.x - m_player->cCollision->radius <= 0) {
		m_player->cTransform->pos.x = m_player->cCollision->radius;
	}

	if (m_player->cTransform->pos.y + m_player->cCollision->radius >= wSize.y) {
		m_player->cTransform->pos.y = wSize.y - m_player->cCollision->radius;
	}

	if (m_player->cTransform->pos.y - m_player->cCollision->radius <= 0) {
		m_player->cTransform->pos.y = m_player->cCollision->radius;
	}
	

	// enemies collision with walls
	for (std::shared_ptr<Entity>& e : m_entities.getEntities("enemy")) {
		Vec2 pos = e->cTransform->pos;

		if (pos.x - e->cCollision->radius <= 0 || pos.x + e->cCollision->radius >= wSize.x) {
			e->cTransform->velocity.x *= -1.0f;
		}

		if (pos.y - e->cCollision->radius <= 0 || pos.y + e->cCollision->radius >= wSize.y) {
			e->cTransform->velocity.y *= -1.0f;
		}
	}

	// bullet collision with enemies

	// player collision with enemies
}

void Game::sEnemySpawner() {
	ConfigVariant cVariant = m_configReader.getData(ConfigReader::Type::Enemy);
	EnemyConfig eConfig = std::get<EnemyConfig>(cVariant);

	if (((m_currentFrame - m_lastEnemySpawnTime) % eConfig.SI) == 0) {
		spawnEnemy();
	}
}

void Game::sRender() {
	m_window.clear();

	for (const std::shared_ptr<Entity>& e : m_entities.getEntities()) {
		if (e->tag() == "player") {
			continue;
		}

		e->cShape->shape.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
		e->cTransform->angle += 1.0f;
		e->cShape->shape.setRotation(e->cTransform->angle);

		m_window.draw(e->cShape->shape);
	}

	m_player->cShape->shape.setPosition(m_player->cTransform->pos.x, m_player->cTransform->pos.y);
	m_player->cTransform->angle += 3.0f;
	m_player->cShape->shape.setRotation(m_player->cTransform->angle);

	m_window.draw(m_player->cShape->shape);
	m_window.display();
}

void Game::sUserInput() {
	sf::Event event;
	while (m_window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			m_running = false;
		}

		if (event.type == sf::Event::KeyPressed) {
			switch (event.key.code) {
			case sf::Keyboard::W:
				m_player->cInput->up = true;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = true;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = true;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = true;
				break;

			case sf::Keyboard::Escape:
				m_running = false;
			}
		}

		if (event.type == sf::Event::KeyReleased) {
			switch (event.key.code) {
			case sf::Keyboard::W:
				m_player->cInput->up = false;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = false;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = false;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = false;
				break;
			}

		}

		if (event.type == sf::Event::MouseButtonPressed) {
			if (event.mouseButton.button == sf::Mouse::Left) {
				std::cout << "left button at (" << event.mouseButton.x << ", " << event.mouseButton.y << ")\n";
				// call spawnBullet here
			}

			if (event.mouseButton.button == sf::Mouse::Right) {
				std::cout << "right button at (" << event.mouseButton.x << ", " << event.mouseButton.y << ")\n";
				// call spawnSpecialWeapon here
			}
		}
	}
}