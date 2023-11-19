#pragma once

#include <variant>
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>

struct WindowConfig {
	int W, L, FL, FS;

	WindowConfig(const std::vector<std::string>&);
};

struct FontConfig {
	std::string F;
	int S, R, G, B;

	FontConfig(const std::vector<std::string>&);
};

struct PlayerConfig { 
	int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; 
	float S; 

	PlayerConfig(const std::vector<std::string>&);
};

struct EnemyConfig { 
	int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; 
	float SMIN, SMAX; 

	EnemyConfig(const std::vector<std::string>&);
};

struct BulletConfig { 
	int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; 
	float S; 

	BulletConfig(const std::vector<std::string>&);
};

using ConfigVariant = std::variant<PlayerConfig, EnemyConfig, BulletConfig, WindowConfig, FontConfig>;

class ConfigReader {
	friend class Game;

	enum Type { Window, Font, Player, Enemy, Bullet };

	ConfigReader(const std::string&);

	std::unordered_map<ConfigReader::Type, ConfigVariant> m_configMap;

public:
	ConfigVariant getReader(const ConfigReader::Type& type);
};
