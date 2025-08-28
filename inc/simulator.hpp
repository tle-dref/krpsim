#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>


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
	int maxDelay;

public:
	// Simulator(const std::string& filename, int delay);
	Simulator(void);
	~Simulator(void);

	// void parseFile(const std::string& filename);
	// void run();
	// void displayTrace() const;
	std::unordered_map<std::string, int>	getStocks(void);
	std::vector<Process>					getProcesses(void);
	std::vector<Event>						getTrace(void);
	int										getMaxDelay(void);

};
