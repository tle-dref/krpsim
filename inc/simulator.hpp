#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>


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
	void run();
	void displayTrace() const;
	void displayAll() const;

	int addStock(std::string line);
	int addProcess(std::string line);
	int addOptimize(std::string line);
};

// Utility functions
std::string trim(const std::string& str);
std::vector<std::string> split(const std::string& str, char delimiter);
std::unordered_map<std::string, int> parseNeeds(const std::string& str);
std::unordered_map<std::string, int> parseResults(const std::string& str);
bool is_digits(const std::string& str);
