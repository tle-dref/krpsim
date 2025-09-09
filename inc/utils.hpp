#pragma once
# include "simulator.hpp"

// Utility functions
std::string trim(const std::string& str);
std::vector<std::string> split(const std::string& str, char delimiter);
std::unordered_map<std::string, int> parseNeeds(const std::string& str);
std::unordered_map<std::string, int> parseResults(const std::string& str);
bool is_digits(const std::string& str);
bool RequierementsForProcesses(const std::unordered_map<std::string, int> &needs, const std::unordered_map<std::string, int> &stocks);