#include "ConfigReader.h"
#include "Util.h"

ConfigReader::ConfigReader(const std::string& filename) {
	std::ifstream fin(filename);
	std::string line;

	if (!fin.is_open()) {
		std::cerr << "Error opening file!!" << std::endl;
		exit(-1);
	}

	while (std::getline(fin, line)) {
		std::vector<std::string> words = Util::splitString(line);

		std::string key = words.at(0);
		
		if (key == "Window") {
			m_configMap[ConfigReader::Type::Window] = WindowConfig(words);
		}
		else if (key == "Player") {
			m_configMap[ConfigReader::Type::Player] = PlayerConfig(words);
		}
		else if (key == "Bullet") {
			m_configMap[ConfigReader::Type::Bullet] = BulletConfig(words);
		}
		else if (key == "Enemy") {
			m_configMap[ConfigReader::Type::Enemy] = EnemyConfig(words);
		}
		else if (key == "Font") {
			m_configMap[ConfigReader::Type::Font] = FontConfig(words);
		}
	}
}

ConfigVariant ConfigReader::getReader(const ConfigReader::Type& type) {
	return m_configMap[type];
}

WindowConfig::WindowConfig(const std::vector<std::string>& data) {
	try {
		W = stoi(data.at(1));
		L = stoi(data.at(2));
		FL = stoi(data.at(3));
		FS = stoi(data.at(4));
	}
	catch (const std::out_of_range& e) {
		std::cerr << "Invalid data in config file for 'Window'\n" << e.what() << std::endl;
		exit(-1);
	}
}

FontConfig::FontConfig(const std::vector<std::string>& data) {
	try {
		F = data.at(1);
		S = stoi(data.at(2));
		R = stoi(data.at(3));
		G = stoi(data.at(4));
		B = stoi(data.at(5));
	}
	catch (const std::out_of_range& e) {
		std::cerr << "Invalid data in config file for 'Font'\n" << e.what() << std::endl;
		exit(-1);
	}
}

PlayerConfig::PlayerConfig(const std::vector<std::string>& data) {
	try {
		SR = stoi(data.at(1));
		CR = stoi(data.at(2));
		S = stof(data.at(3));
		FR = stoi(data.at(4));
		FG = stoi(data.at(5));
		FB = stoi(data.at(6));
		OR = stoi(data.at(7));
		OG = stoi(data.at(8));
		OB = stoi(data.at(9));
		OT = stoi(data.at(10));
		V = stoi(data.at(11));
	}
	catch (const std::out_of_range& e) {
		std::cerr << "Invalid data in config file for 'Player'\n" << e.what() << std::endl;
		exit(-1);
	}
}

EnemyConfig::EnemyConfig(const std::vector<std::string>& data) {
	try {
		SR = stoi(data.at(1));
		CR = stoi(data.at(2));
		SMIN = stof(data.at(3));
		SMAX = stof(data.at(4));
		OR = stoi(data.at(5));
		OG = stoi(data.at(6));
		OB = stoi(data.at(7));
		OT = stoi(data.at(8));
		VMIN = stoi(data.at(9));
		VMAX = stoi(data.at(10));
		L = stoi(data.at(11));
		SI = stoi(data.at(12));
	}
	catch (const std::out_of_range& e) {
		std::cerr << "Invalid data in config file for 'Enemy'\n" << e.what() << std::endl;
		exit(-1);
	}
}

BulletConfig::BulletConfig(const std::vector<std::string>& data) {
	try {
		SR = stoi(data.at(1));
		CR = stoi(data.at(2));
		S = stof(data.at(3));
		FR = stoi(data.at(4));
		FG = stoi(data.at(5));
		FB = stoi(data.at(6));
		OR = stoi(data.at(7));
		OG = stoi(data.at(8));
		OB = stoi(data.at(9));
		OT = stoi(data.at(10));
		V = stoi(data.at(11));
		L = stoi(data.at(12));
	}
	catch (const std::out_of_range& e) {
		std::cerr << "Invalid data in config file for 'Enemy'\n" << e.what() << std::endl;
		exit(-1);
	}
}
