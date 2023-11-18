#include "Game.h"

Game::Game(const std::string& config) {
	init(config);
}

void Game::init(const std::string& config) {
	// TODO: read in config file here

	m_window.create(sf::VideoMode(1280, 720), "Geometry Wars");
	m_window.setFramerateLimit(60);

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
	// TODO: Finish adding all the properties of the player with the correct values from the config

	std::shared_ptr<Entity> entity = m_entities.addEntity("player");

	entity->cTransform = std::make_shared<CTransform>(10.0f, Vec2(200.0f,200.0f), Vec2(1.0f, 1.0f));
	entity->cShape = std::make_shared<CShape>(30.0f, 8, sf::Color(10, 10, 10), sf::Color::Red, 4.0f);
	entity->cInput = std::make_shared<CInput>();

	m_player = entity;
}

void Game::spawnEnemy() {
	// TODO: make sure the enemy is spawned properly with the m_enemyconfig variables
	//       the enemy must be spawned completely within the bounds of the window

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
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {

}

void Game::sMovement() {
	// TODO: implement all the entity movement in this function
	//       you should read the m_player->cInput component to determine if the player is moving

	// sample movement speed update

	//m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
	//m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
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
	// TODO: implement all proper collisions between entities
	//		 be sure to use the collision radius, not the shape radius
}

void Game::sEnemySpawner() {
	// TODO: code which implements enemy spawning should go here
	// use (m_currentFrame - m_lastEnemySpawnTime) to determine
	// how long it has been since the last enemy spawned
}

void Game::sRender() {
	// TODO: change the below code to draw all of the entities

	m_window.clear();

	m_player->cShape->shape.setPosition(m_player->cTransform->pos.x, m_player->cTransform->pos.y);
	m_player->cTransform->angle += 1.0f;
	m_player->cShape->shape.setRotation(m_player->cTransform->angle);


	m_window.draw(m_player->cShape->shape);
	m_window.display();
}

void Game::sUserInput() {
	// TODO: handle user input here
	//		 note that you should only be setting the player's input component variables here
	//       you should not implement the player's movement logic here
	//       the movement system will read the variables you set in this function

	sf::Event event;
	while (m_window.pollEvent(event)) {

		// this event triggers when window is closed
		if (event.type == sf::Event::Closed) {
			m_running = false;
		}

		if (event.type == sf::Event::KeyPressed) {
			switch (event.key.code) {
			case sf::Keyboard::W:
				std::cout << "W pressed\n";
				// TODO: set player's input component "up" to true

				break;
			default: break;
			}
		}

		if (event.type == sf::Event::KeyReleased) {
			switch (event.key.code) {
			case sf::Keyboard::W:
				std::cout << "W released\n";
				// TODO: set player's input component "up" to false

				break;
			default: break;

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