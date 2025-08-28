#include "simulator.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>

std::string trim(const std::string& str) {
	size_t start = str.find_first_not_of(" \t\n\r");
	if (start == std::string::npos) return "";
	size_t end = str.find_last_not_of(" \t\n\r");
	return str.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;
	while (std::getline(ss, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

Simulator::Simulator(const std::string& filename, int delay) {
	(void)delay;
	parseFile(filename);
}

Simulator::~Simulator() {
}

void Simulator::run() {
}

void Simulator::displayTrace() const {
}

int addOptimize(std::string line) {
	(void)line;
	return 0;
}

bool is_digits(const std::string& str) {
	return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

int Simulator::addStock(std::string line) {
		line = trim(line);
		char **tokens = split(line, ':');
		if (!tokens || tokens[0] == nullptr || tokens[1] == nullptr) {
			std::cerr << "Invalid line format: " << line << std::endl;
			return -1;
		}
		else if (is_digits(tokens[1])) {
			std::string key = trim(tokens[0]);
			std::string value = trim(tokens[1]);
			this->stocks[key] = std::stoi(value);
			return 0;
		}
		return -1;
}

//<name>:(<need>:<qty>;<need>:<qty>):(<result>:<qty>;<result>:<qty>):<nb_cycle>
int addProcess(std::string line)
{
	std::string name, needs, results;
	int nb_cycle;
	char skip;
	std::istringstream stream(line);
	std::getline(stream, name, ':');
	stream >> skip;
	std::getline(stream, needs, ')');
	stream >> skip >> skip;
	std::getline(stream, results, ')');
	stream >> skip >> nb_cycle;

	name = trim(name);
	needs = trim(needs);
	results = trim(results);


	// std::cout << "Nom: " << name << std::endl;
    // std::cout << "Besoins: " << needs << std::endl;
    // std::cout << "Résultats: " << results << std::endl;
    // std::cout << "Cycles: " << nb_cycle << std::endl;
	return 0;
}

void Simulator::parseFile(const std::string& filename) {
	std::ifstream file(filename);
	if (!file) {
		std::cerr << "Error opening file: " << filename << std::endl;
		return;
	}

	int parsing_step = 0;
	std::string line;
	while (std::getline(file, line)) {
		if (line.empty() || line[0] == '#') {
			continue;
		}

		if (parsing_step == 0) {
			if (addStock(line) == -1) {
				parsing_step = 1;
			}
		} else if (parsing_step == 1) {
			if (addProcess(line) == -1) {
				parsing_step = 2;
			}
		} else if (parsing_step == 2) {
			if (addOptimize(line) == -1) {
				parsing_step = 3;
			}
		}
	}
}