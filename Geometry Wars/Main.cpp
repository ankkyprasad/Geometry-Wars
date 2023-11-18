#include <SFML/Graphics.hpp>
#include "Game.h"

int main() {
	Game g("Config/Config.txt");
	g.run();

	return 0;
}