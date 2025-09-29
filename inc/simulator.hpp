#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

enum class ParseError {
	SUCCESS = 0,
	INVALID_FORMAT,
	MISSING_VALUE,
	INVALID_NUMBER,
	NEGATIVE_VALUE,
	OVERFLOW,
	DUPLICATE_NAME,
	UNKNOWN_REFERENCE,
	EMPTY_NAME,
	FILE_ERROR
};

struct ParseResult {
	ParseError error;
	std::string message;
	int lineNumber;
	
	ParseResult(ParseError err = ParseError::SUCCESS, const std::string& msg = "", int line = -1)
		: error(err), message(msg), lineNumber(line) {}
	
	bool isSuccess() const { return error == ParseError::SUCCESS; }
};

struct Process {
	std::string name;
	std::unordered_map<std::string, int> needs;
	std::unordered_map<std::string, int> results;

	int delay;
	int startTime = -1;
	int finishTime = -1;
};

struct Event {
	int time;
	std::string process;
};

class Simulator {
	private:
	std::unordered_map<std::string, int> stocks;
	std::vector<Process> processes;
	std::vector<Event> trace;
	std::vector<std::string> optimize;
	// int maxDelay;

public:
	Simulator(const std::string& filename, int delay);
	~Simulator();

	void parseFile(const std::string& filename);
	ParseResult parseFileWithErrorCheck(const std::string& filename);
	void run();
	void displayTrace() const;
	void displayAll() const;

	ParseResult addStock(const std::string& line, int lineNumber);
	ParseResult addProcess(const std::string& line, int lineNumber);
	ParseResult addOptimize(const std::string& line, int lineNumber);
	
	// Helper validation functions
	ParseResult validateStockName(const std::string& name) const;
	ParseResult validateNumber(const std::string& str, int& result) const;
	ParseResult parseNeeds(const std::string& str, std::unordered_map<std::string, int>& needs) const;
	ParseResult parseResults(const std::string& str, std::unordered_map<std::string, int>& results) const;
	void reportError(const ParseResult& result, const std::string& line) const;
};

// Utility functions
std::string trim(const std::string& str);
std::vector<std::string> split(const std::string& str, char delimiter);
bool is_digits(const std::string& str);
