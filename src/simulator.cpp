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
#include <stdexcept>
#include <limits>

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
		ParseResult result = parseFileWithErrorCheck(filename);
		if (!result.isSuccess()) {
			std::cerr << "Parse error at line " << result.lineNumber << ": " << result.message << std::endl;
			std::exit(1);
		}
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

ParseResult Simulator::addOptimize(const std::string& line, int lineNumber) {
	std::string trimmedLine = trim(line);
	
	if (trimmedLine.substr(0, 10) != "optimize:(" || trimmedLine.back() != ')') {
		return ParseResult(ParseError::INVALID_FORMAT, "Optimize line must have format 'optimize:(target)'", lineNumber);
	}
	
	size_t start = trimmedLine.find('(') + 1;
	size_t end = trimmedLine.find(')', start);
	if (end == std::string::npos || start >= end) {
		return ParseResult(ParseError::INVALID_FORMAT, "Invalid parentheses in optimize line", lineNumber);
	}
	
	std::string target = trim(trimmedLine.substr(start, end - start));
	
	if (target.empty()) {
		return ParseResult(ParseError::EMPTY_NAME, "Optimization target cannot be empty", lineNumber);
	}
	
	ParseResult nameValidation = validateStockName(target);
	if (!nameValidation.isSuccess()) {
		nameValidation.lineNumber = lineNumber;
		return nameValidation;
	}
	
	// Check for duplicate optimization targets
	for (const auto& opt : optimize) {
		if (opt == target) {
			return ParseResult(ParseError::DUPLICATE_NAME, "Duplicate optimization target: " + target, lineNumber);
		}
	}
	
	optimize.push_back(target);
	return ParseResult(ParseError::SUCCESS);
}

bool is_digits(const std::string& str) {
	return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

void Simulator::reportError(const ParseResult& result, const std::string& line) const {
	if (!result.isSuccess()) {
		std::cerr << "Parse error at line " << result.lineNumber << ": " << result.message << std::endl;
		std::cerr << "Line content: " << line << std::endl;
	}
}

ParseResult Simulator::validateStockName(const std::string& name) const {
	if (name.empty()) {
		return ParseResult(ParseError::EMPTY_NAME, "Stock name cannot be empty");
	}
	
	for (char c : name) {
		if (!std::isalnum(c) && c != '_') {
			return ParseResult(ParseError::INVALID_FORMAT, "Stock name contains invalid characters: " + name);
		}
	}
	
	return ParseResult(ParseError::SUCCESS);
}

ParseResult Simulator::validateNumber(const std::string& str, int& result) const {
	if (str.empty()) {
		return ParseResult(ParseError::MISSING_VALUE, "Number value is empty");
	}
	
	if (!is_digits(str)) {
		return ParseResult(ParseError::INVALID_NUMBER, "Invalid number format: " + str);
	}
	
	try {
		long long temp = std::stoll(str);
		if (temp < 0) {
			return ParseResult(ParseError::NEGATIVE_VALUE, "Negative values not allowed: " + str);
		}
		if (temp > std::numeric_limits<int>::max()) {
			return ParseResult(ParseError::OVERFLOW, "Number too large: " + str);
		}
		result = static_cast<int>(temp);
		return ParseResult(ParseError::SUCCESS);
	} catch (const std::exception& e) {
		return ParseResult(ParseError::OVERFLOW, "Number conversion failed: " + str);
	}
}

ParseResult Simulator::addStock(const std::string& line, int lineNumber) {
	std::string trimmedLine = trim(line);
	
	if (trimmedLine.find('(') != std::string::npos) {
		return ParseResult(ParseError::INVALID_FORMAT, "Line contains parentheses - likely a process");
	}
	
	std::vector<std::string> tokens = split(trimmedLine, ':');
	
	if (tokens.size() < 2) {
		return ParseResult(ParseError::INVALID_FORMAT, "Stock line must have format 'name:quantity'", lineNumber);
	}
	
	if (tokens.size() > 2) {
		return ParseResult(ParseError::INVALID_FORMAT, "Stock line has too many ':' separators", lineNumber);
	}
	
	std::string name = trim(tokens[0]);
	std::string valueStr = trim(tokens[1]);
	
	if (valueStr.empty()) {
		return ParseResult(ParseError::MISSING_VALUE, "Stock quantity is missing", lineNumber);
	}
	
	ParseResult nameValidation = validateStockName(name);
	if (!nameValidation.isSuccess()) {
		nameValidation.lineNumber = lineNumber;
		return nameValidation;
	}
	
	if (stocks.find(name) != stocks.end()) {
		return ParseResult(ParseError::DUPLICATE_NAME, "Duplicate stock name: " + name, lineNumber);
	}
	
	int quantity;
	ParseResult numberValidation = validateNumber(valueStr, quantity);
	if (!numberValidation.isSuccess()) {
		numberValidation.lineNumber = lineNumber;
		return numberValidation;
	}
	
	stocks[name] = quantity;
	return ParseResult(ParseError::SUCCESS);
}

ParseResult Simulator::parseNeeds(const std::string& str, std::unordered_map<std::string, int>& needs) const {
	needs.clear();
	std::vector<std::string> tokens = split(str, ';');
	
	for (const std::string& token : tokens) {
		std::string trimmedToken = trim(token);
		if (trimmedToken.empty()) continue;
		
		std::vector<std::string> pair = split(trimmedToken, ':');
		if (pair.size() != 2) {
			return ParseResult(ParseError::INVALID_FORMAT, "Need format must be 'name:quantity', got: " + trimmedToken);
		}
		
		std::string name = trim(pair[0]);
		std::string quantityStr = trim(pair[1]);
		
		ParseResult nameValidation = validateStockName(name);
		if (!nameValidation.isSuccess()) {
			return nameValidation;
		}
		
		int quantity;
		ParseResult numberValidation = validateNumber(quantityStr, quantity);
		if (!numberValidation.isSuccess()) {
			return numberValidation;
		}
		
		if (quantity == 0) {
			return ParseResult(ParseError::INVALID_NUMBER, "Need quantity cannot be zero for: " + name);
		}
		
		if (needs.find(name) != needs.end()) {
			return ParseResult(ParseError::DUPLICATE_NAME, "Duplicate need in process: " + name);
		}
		
		needs[name] = quantity;
	}
	
	return ParseResult(ParseError::SUCCESS);
}

ParseResult Simulator::parseResults(const std::string& str, std::unordered_map<std::string, int>& results) const {
	results.clear();
	std::vector<std::string> tokens = split(str, ';');
	
	for (const std::string& token : tokens) {
		std::string trimmedToken = trim(token);
		if (trimmedToken.empty()) continue;
		
		std::vector<std::string> pair = split(trimmedToken, ':');
		if (pair.size() != 2) {
			return ParseResult(ParseError::INVALID_FORMAT, "Result format must be 'name:quantity', got: " + trimmedToken);
		}
		
		std::string name = trim(pair[0]);
		std::string quantityStr = trim(pair[1]);
		
		ParseResult nameValidation = validateStockName(name);
		if (!nameValidation.isSuccess()) {
			return nameValidation;
		}
		
		int quantity;
		ParseResult numberValidation = validateNumber(quantityStr, quantity);
		if (!numberValidation.isSuccess()) {
			return numberValidation;
		}
		
		if (quantity == 0) {
			return ParseResult(ParseError::INVALID_NUMBER, "Result quantity cannot be zero for: " + name);
		}
		
		if (results.find(name) != results.end()) {
			return ParseResult(ParseError::DUPLICATE_NAME, "Duplicate result in process: " + name);
		}
		
		results[name] = quantity;
	}
	
	return ParseResult(ParseError::SUCCESS);
}

//<name>:(<need>:<qty>;<need>:<qty>):(<result>:<qty>;<result>:<qty>):<nb_cycle>
ParseResult Simulator::addProcess(const std::string& line, int lineNumber)
{
	if (line.find("optimize:") == 0) {
		return ParseResult(ParseError::INVALID_FORMAT, "Process line cannot start with 'optimize:'", lineNumber);
	}
	
	std::string trimmedLine = trim(line);
	if (trimmedLine.empty()) {
		return ParseResult(ParseError::INVALID_FORMAT, "Process line cannot be empty", lineNumber);
	}
	
	size_t firstColon = trimmedLine.find(':');
	if (firstColon == std::string::npos) {
		return ParseResult(ParseError::INVALID_FORMAT, "Process line must contain ':' separator", lineNumber);
	}
	
	std::string name = trim(trimmedLine.substr(0, firstColon));
	std::string remainder = trimmedLine.substr(firstColon + 1);
	
	ParseResult nameValidation = validateStockName(name);
	if (!nameValidation.isSuccess()) {
		nameValidation.lineNumber = lineNumber;
		return nameValidation;
	}
	
	for (const auto& process : processes) {
		if (process.name == name) {
			return ParseResult(ParseError::DUPLICATE_NAME, "Duplicate process name: " + name, lineNumber);
		}
	}
	
	size_t needsStart = remainder.find('(');
	size_t needsEnd = remainder.find(')', needsStart);
	if (needsStart == std::string::npos || needsEnd == std::string::npos) {
		return ParseResult(ParseError::INVALID_FORMAT, "Process needs must be enclosed in parentheses", lineNumber);
	}
	
	std::string needsStr = remainder.substr(needsStart + 1, needsEnd - needsStart - 1);
	remainder = remainder.substr(needsEnd + 1);
	
	size_t resultsStart = remainder.find('(');
	size_t resultsEnd = remainder.find(')', resultsStart);
	if (resultsStart == std::string::npos || resultsEnd == std::string::npos) {
		return ParseResult(ParseError::INVALID_FORMAT, "Process results must be enclosed in parentheses", lineNumber);
	}
	
	std::string resultsStr = remainder.substr(resultsStart + 1, resultsEnd - resultsStart - 1);
	remainder = remainder.substr(resultsEnd + 1);
	
	if (remainder.empty() || remainder[0] != ':') {
		return ParseResult(ParseError::INVALID_FORMAT, "Process must end with ':cycle_count'", lineNumber);
	}
	
	std::string cycleStr = trim(remainder.substr(1));
	int cycles;
	ParseResult cycleValidation = validateNumber(cycleStr, cycles);
	if (!cycleValidation.isSuccess()) {
		cycleValidation.lineNumber = lineNumber;
		return cycleValidation;
	}
	
	std::unordered_map<std::string, int> needs, results;
	
	ParseResult needsResult = parseNeeds(needsStr, needs);
	if (!needsResult.isSuccess()) {
		needsResult.lineNumber = lineNumber;
		return needsResult;
	}
	
	ParseResult resultsResult = parseResults(resultsStr, results);
	if (!resultsResult.isSuccess()) {
		resultsResult.lineNumber = lineNumber;
		return resultsResult;
	}
	
	if (needs.empty()) {
		return ParseResult(ParseError::INVALID_FORMAT, "Process must have at least one need", lineNumber);
	}
	
	if (results.empty()) {
		return ParseResult(ParseError::INVALID_FORMAT, "Process must have at least one result", lineNumber);
	}
	
	Process process;
	process.name = name;
	process.needs = needs;
	process.results = results;
	process.delay = cycles;
	processes.push_back(process);

	return ParseResult(ParseError::SUCCESS);
}

ParseResult Simulator::parseFileWithErrorCheck(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		return ParseResult(ParseError::FILE_ERROR, "Cannot open file: " + filename, 0);
	}

	int parsing_step = 0; // 0=stocks, 1=processes, 2=optimize
	std::string line;
	int lineNumber = 0;
	
	while (std::getline(file, line)) {
		lineNumber++;
		
		// Skip empty lines and comments
		std::string trimmedLine = trim(line);
		if (trimmedLine.empty() || trimmedLine[0] == '#') {
			continue;
		}

		ParseResult result;
		
		// Try parsing based on current step
		if (parsing_step == 0) {
			// Try to parse as stock
			result = addStock(line, lineNumber);
			if (!result.isSuccess()) {
				if (result.error == ParseError::INVALID_FORMAT) {
					parsing_step = 1;
				} else {
					file.close();
					return result;
				}
			} else {
				continue;
			}
		}
		
		if (parsing_step == 1) {
			// Try to parse as process
			result = addProcess(line, lineNumber);
			if (!result.isSuccess()) {
				if (result.error == ParseError::INVALID_FORMAT && line.find("optimize:") == 0) {
					parsing_step = 2;
				} else {
					file.close();
					return result;
				}
			} else {
				continue;
			}
		}
		
		if (parsing_step == 2) {
			result = addOptimize(line, lineNumber);
			if (!result.isSuccess()) {
				file.close();
				return result;
			}
		}
	}
	
	file.close();
	return ParseResult(ParseError::SUCCESS, "File parsed successfully");
}

void Simulator::parseFile(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error: Cannot open file: " << filename << std::endl;
		return;
	}

	int parsing_step = 0;
	std::string line;
	int lineNumber = 0;
	bool hasErrors = false;
	
	while (std::getline(file, line)) {
		lineNumber++;
		
		std::string trimmedLine = trim(line);
		if (trimmedLine.empty() || trimmedLine[0] == '#') {
			continue;
		}

		ParseResult result;
		bool lineProcessed = false;
		
		if (parsing_step == 0) {
			result = addStock(line, lineNumber);
			if (result.isSuccess()) {
				lineProcessed = true;
			} else if (result.error == ParseError::INVALID_FORMAT) {
				parsing_step = 1;
			} else {
				reportError(result, line);
				hasErrors = true;
				lineProcessed = true;
			}
		}
		
		if (parsing_step == 1 && !lineProcessed) {
			if (trimmedLine.find("optimize:") == 0) {
				parsing_step = 2;
			} else {
				// Try to parse as process
				result = addProcess(line, lineNumber);
				if (result.isSuccess()) {
					lineProcessed = true;
				} else {
					reportError(result, line);
					hasErrors = true;
					lineProcessed = true;
				}
			}
		}
		
		if (parsing_step == 2 && !lineProcessed) {
			result = addOptimize(line, lineNumber);
			if (result.isSuccess()) {
				lineProcessed = true;
			} else {
				reportError(result, line);
				hasErrors = true;
				lineProcessed = true;
			}
		}
		
		if (!lineProcessed) {
			std::cerr << "Parse error at line " << lineNumber << ": Unrecognized line format" << std::endl;
			std::cerr << "Line content: " << line << std::endl;
			hasErrors = true;
		}
	}
	
	file.close();
	
	if (!hasErrors) {
		for (const std::string& target : optimize) {
			bool targetExists = false;
			
			if (stocks.find(target) != stocks.end()) {
				targetExists = true;
			} else {
				for (const auto& process : processes) {
					if (process.results.find(target) != process.results.end()) {
						targetExists = true;
						break;
					}
				}
			}
			
			if (!targetExists) {
				std::cerr << "Warning: Optimization target '" << target 
						  << "' is not available in stocks or process results" << std::endl;
			}
		}
	}
	
	if (hasErrors) {
		std::cerr << "Parsing completed with errors. Please fix the issues above." << std::endl;
	} else {
		std::cout << "File parsed successfully!" << std::endl;
		std::cout << "Stocks: " << stocks.size() << ", Processes: " << processes.size() 
				  << ", Optimizations: " << optimize.size() << std::endl;
	}
}