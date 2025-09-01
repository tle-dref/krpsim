#include "simulator.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <vector>

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
	if (!filename.empty()) {
		parseFile(filename);
	}
}

Simulator::~Simulator() {
}

void Simulator::run() {
}

void Simulator::displayTrace() const {
}

void Simulator::displayAll() const {
	std::cout << "=== STOCKS ===" << std::endl;
	for (const auto& stock : stocks) {
		std::cout << stock.first << ": " << stock.second << std::endl;
	}
	
	std::cout << "\n=== PROCESSES ===" << std::endl;
	for (const auto& process : processes) {
		std::cout << "Name: " << process.name << std::endl;
		std::cout << "Delay: " << process.delay << std::endl;
		std::cout << "Needs: ";
		for (const auto& need : process.needs) {
			std::cout << need.first << ":" << need.second << " ";
		}
		std::cout << std::endl;
		std::cout << "Results: ";
		for (const auto& result : process.results) {
			std::cout << result.first << ":" << result.second << " ";
		}
		std::cout << std::endl << std::endl;
	}
	
	std::cout << "\n=== OPTIMIZATIONS ===" << std::endl;
	for (const auto& opt : optimize) {
		std::cout << opt << std::endl;
	}
}

int Simulator::addOptimize(std::string line) {
	line = trim(line);
	if (line.find("optimize:(") == 0 && line.back() == ')') {
		size_t start = line.find('(') + 1;
		size_t end = line.find(')', start);
		if (end != std::string::npos) {
			std::string target = line.substr(start, end - start);
			target = trim(target);
			this->optimize.push_back(target);
			return 0;
		}
	}
	return -1;
}

bool is_digits(const std::string& str) {
	return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

int Simulator::addStock(std::string line) {
		line = trim(line);
		std::vector<std::string> tokens = split(line, ':');
		if (tokens.size() < 2) {
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

std::unordered_map<std::string, int> parseNeeds(const std::string& str) {
	std::unordered_map<std::string, int> needs;
	std::vector<std::string> tokens = split(str, ';');
	for (const std::string& token : tokens) {
		std::vector<std::string> pair = split(token, ':');
		if (pair.size() == 2 && is_digits(pair[1])) {
			needs[trim(pair[0])] = std::stoi(trim(pair[1]));
		}
	}
	return needs;
}

std::unordered_map<std::string, int> parseResults(const std::string& str) {
	std::unordered_map<std::string, int> results;
	std::vector<std::string> tokens = split(str, ';');
	for (const std::string& token : tokens) {
		std::vector<std::string> pair = split(token, ':');
		if (pair.size() == 2 && is_digits(pair[1])) {
			results[trim(pair[0])] = std::stoi(trim(pair[1]));
		}
	}
	return results;
}

//<name>:(<need>:<qty>;<need>:<qty>):(<result>:<qty>;<result>:<qty>):<nb_cycle>
int Simulator::addProcess(std::string line)
{
	if (line.find("optimize:") == 0) {
		return -1;
	}
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

	Process process;
	process.name = name;
	process.needs = parseNeeds(needs);
	process.results = parseResults(results);
	process.delay = nb_cycle;
	this->processes.push_back(process);

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
	while (std::getline(file, line) && parsing_step != 3) {
		if (line.empty() || line[0] == '#') {
			continue;
		}

		if (parsing_step == 0) {
			if (addStock(line) == -1) {
				parsing_step = 1;
			}
		}
		if (parsing_step == 1) {
			if (addProcess(line) == -1) {
				parsing_step = 2;
			}
		}
		if (parsing_step == 2) {
			if (addOptimize(line) == -1) {
				parsing_step = 3;
			}
		}
	}
}