#include <SFML/Graphics.hpp>
#include "Game.h"

int main() {
	srand(static_cast<unsigned>(time(nullptr)));

	Game g("Config/Config.txt");
	g.run();

	return 0;
}