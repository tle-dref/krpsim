#include "simulator.hpp"

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

bool is_digits(const std::string& str) {
	return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
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

bool RequierementsForProcesses(const std::unordered_map<std::string, int>& needs, const std::unordered_map<std::string, int>& stocks) {
    for (const auto& need : needs) {
        auto it = stocks.find(need.first);
        if (it == stocks.end() || it->second < need.second) {
            // std::cout << "Fail to find \033[32m" << need.first << "\033[0m in Stocks or bad \033[31mQuantity\033[0m" << std::endl;
            return false;
        }
        // else
        // {
        //     std::cout << "Success: " << need.first << " | need: " << need.second << " | stock: " << it->second << std::endl;
        // }
    }
    return true;
}