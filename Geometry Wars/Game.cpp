#include <cmath>

#include "Game.h"
#include "Util.h"


Game::Game(const std::string& config) {
	m_configReader.readFromFile(config);
	init();
}

void Game::init() {
	ConfigVariant cVariant = m_configReader.getData(ConfigReader::Type::Window);
	WindowConfig wConfig = std::get<WindowConfig>(cVariant);

	m_window.create(sf::VideoMode(wConfig.W, wConfig.H), "Geometry Wars", wConfig.FS ? sf::Style::Fullscreen : sf::Style::Default);
	m_window.setFramerateLimit(wConfig.FL);

	spawnPlayer();
}

void Game::run() {
	while (m_running) {
		if (!m_paused) {
			m_entities.update();
			sEnemySpawner();
			sLifespan();
			sMovement();
			sCollision();

			m_currentFrame++;
		}

		sUserInput();
		sRender();
	}
}

void Game::setPaused(bool paused) {
	m_paused = paused;
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
	ConfigVariant eVariant = m_configReader.getData(ConfigReader::Type::Enemy);
	EnemyConfig eConfig = std::get<EnemyConfig>(eVariant);

	size_t enemiesCount = e->cShape->shape.getPointCount();
	float speed = Util::generateRandomValFromRange((float) eConfig.SMIN, (float) eConfig.SMAX);
	float outInterval = 360.0f / enemiesCount;

	sf::Color fillColor = e->cShape->shape.getFillColor();
	sf::Color outlineColor = e->cShape->shape.getOutlineColor();

	for (int i = 1; i <= enemiesCount; i++) {
		std::shared_ptr<Entity> entity = m_entities.addEntity("small_enemies");

		float angle = outInterval * i;
		float angleRadians = angle * (3.14f / 180.0f);

		Vec2 velocity(speed * cos(angleRadians), speed * sin(angleRadians));
		velocity.normalize();

		entity->cShape = std::make_shared<CShape>(eConfig.SR / 2, enemiesCount, fillColor, outlineColor, eConfig.OT);
		entity->cLifespan = std::make_shared<CLifespan>(eConfig.L, eConfig.L);
		entity->cCollision = std::make_shared<CCollision>(eConfig.CR / 2);
		entity->cTransform = std::make_shared<CTransform>(int(outInterval * i) % 360, e->cTransform->pos, velocity);
	}
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target) {
	ConfigVariant bVariant = m_configReader.getData(ConfigReader::Type::Bullet);
	BulletConfig bConfig = std::get<BulletConfig>(bVariant);

	std::shared_ptr<Entity> e = m_entities.addEntity("bullet");

	sf::Color fillColor(bConfig.FR, bConfig.FG, bConfig.FB);
	sf::Color outlineColor(bConfig.OR, bConfig.OG, bConfig.OB);

	Vec2 playerPos = entity->cTransform->pos;

	// calculate bullet velocity
	Vec2 distanceV(target.x - playerPos.x, target.y - playerPos.y);
	distanceV.normalize();
	Vec2 bulletVelocity(bConfig.S * distanceV.x, bConfig.S * distanceV.y);

	e->cShape = std::make_shared<CShape>(bConfig.SR, bConfig.V, fillColor, outlineColor, bConfig.OT);
	e->cCollision = std::make_shared<CCollision>(bConfig.CR);
	e->cTransform = std::make_shared<CTransform>(0.0f, playerPos, bulletVelocity);
	e->cLifespan = std::make_shared<CLifespan>(bConfig.L, bConfig.L);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> e) {
	ConfigVariant bVariant = m_configReader.getData(ConfigReader::Type::Bullet);
	BulletConfig bConfig = std::get<BulletConfig>(bVariant);

	const sf::Color fillColor = sf::Color::Red;
	const sf::Color outlineColor = sf::Color::Red;
	const float speed = 10;
	const float outInterval = 20;

	for (int i = 1; i <= 18; i++) {
		std::shared_ptr<Entity> entity = m_entities.addEntity("bullet");
		float angle = outInterval * i * (3.14f / 180.0f);
		Vec2 velocity(speed * cos(angle), speed * sin(angle));

		entity->cShape = std::make_shared<CShape>(bConfig.SR, bConfig.V, fillColor, outlineColor, bConfig.OT);
		entity->cCollision = std::make_shared<CCollision>(bConfig.CR);
		entity->cTransform = std::make_shared<CTransform>(0.0f, e->cTransform->pos, velocity);
		entity->cLifespan = std::make_shared<CLifespan>(bConfig.L, bConfig.L);
	}
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

	// bullet movement
	for (std::shared_ptr<Entity>& e : m_entities.getEntities("bullet")) {
		e->cTransform->pos.x += e->cTransform->velocity.x;
		e->cTransform->pos.y += e->cTransform->velocity.y;
	}

	// small enemy movement
	for (std::shared_ptr<Entity>& e : m_entities.getEntities("small_enemies")) {
		e->cTransform->pos.x += e->cTransform->velocity.x;
		e->cTransform->pos.y += e->cTransform->velocity.y;
	}
}

void Game::sLifespan() {
	for (std::shared_ptr<Entity>& e : m_entities.getEntities()) {
		if (!e->cLifespan) continue;

		if (e->cLifespan->remaining > 0 && e->isActive()) {
			e->cLifespan->remaining--;

			sf::Uint8 alpha = ((float) e->cLifespan->remaining / e->cLifespan->total) * 180;

			sf::Color color = e->cShape->shape.getFillColor();
			color.a = alpha;

			sf::Color outlineColor = e->cShape->shape.getOutlineColor();
			outlineColor.a = alpha;

			e->cShape->shape.setFillColor(color);
			e->cShape->shape.setOutlineColor(color);
		}
		else {
			e->destroy();
		}
	}

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

	// bullet collision with walls
	for (std::shared_ptr<Entity>& bullet : m_entities.getEntities("bullet")) {
		if (bullet->cCollision->radius + bullet->cTransform->pos.x > m_window.getSize().x) {
			bullet->destroy();
		} 
		else if (bullet->cCollision->radius + bullet->cTransform->pos.y > m_window.getSize().y) {
			bullet->destroy();
		} 
		else if (bullet->cTransform->pos.x <= 0 - bullet->cCollision->radius) {
			bullet->destroy();
		}
		else if (bullet->cTransform->pos.y <= 0 - bullet->cCollision->radius) {
			bullet->destroy();
		}
	}

	// bullet collision with enemies
	for (std::shared_ptr<Entity>& bullet : m_entities.getEntities("bullet")) {
		for (std::shared_ptr<Entity>& enemy : m_entities.getEntities("enemy")) {
			Vec2 diffVec(bullet->cTransform->pos.x - enemy->cTransform->pos.x, bullet->cTransform->pos.y - enemy->cTransform->pos.y);
			float dist = diffVec.dist();

			if (dist <= bullet->cCollision->radius + enemy->cCollision->radius) {
				enemy->destroy();
				bullet->destroy();
				spawnSmallEnemies(enemy);
			}
		}
	}

	// player collision with enemies
	for (std::shared_ptr<Entity>& enemy : m_entities.getEntities("enemy")) {
		Vec2 diffVec(enemy->cTransform->pos.x - m_player->cTransform->pos.x, enemy->cTransform->pos.y - m_player->cTransform->pos.y);
		float dist = diffVec.dist();

		if (dist <= enemy->cCollision->radius + m_player->cCollision->radius) {
			m_player->destroy();
			enemy->destroy();

			spawnPlayer();
		}
	}

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
			case sf::Keyboard::P:
				setPaused(!m_paused);
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

		if (m_paused) {
			continue;
		}


		if (event.type == sf::Event::MouseButtonPressed) {
			if (event.mouseButton.button == sf::Mouse::Left) {
				spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
			}

			if (event.mouseButton.button == sf::Mouse::Right) {
				spawnSpecialWeapon(m_player);
			}
		}
	}
}
