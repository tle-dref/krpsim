#pragma once
# include <iostream>
# include <string>
# include <vector>
# include <unordered_map>
# include <sstream>
# include <fstream>
# include <algorithm>
# include <cctype>
# include <cstdlib>
# include <cstring>
# include "utils.hpp"

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

struct State {
	int time;
	std::unordered_map<std::string, int> stocks;
	std::vector<Event> trace;
};

class Simulator {
	private:
	std::unordered_map<std::string, int> stocks;
	std::vector<Process *> processes;
	std::vector<std::string> optimize;

	public:
		Simulator(const std::string& filename, int delay);
		~Simulator();

		void parseFile(const std::string& filename);
		void run();
		void displayTrace() const;
		void displayStock() const;
		void displayAll() const;

		int parseStock(std::string line);
		int parseProcess(std::string line);
		int parseOptimize(std::string line);

		void	addToStock(State *s, std::unordered_map<std::string, int> itemsToAdd);
		void	subtractFromStock(State *s, std::unordered_map<std::string, int> itemsToSubtract);
		std::vector<Process *>	getExecutableProcesses(const State &state, const std::vector<Process *> &Processes);
		std::vector<State> expandState(const State& s, const std::vector<Process *>& processes);
		State 	applyProcess(const State& current, const Process& p);
		int		launchSimulator(void);
		void	launchExpand(const State& current, const std::vector<Process *>& processes);
};

