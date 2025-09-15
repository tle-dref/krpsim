#include "simulator.hpp"

Simulator::Simulator(const std::string& filename, int delay) {
	(void)delay;
	if (!filename.empty()) {
		parseFile(filename);
	}
}

Simulator::~Simulator() {
	for (const auto& p : this->processes) {
		free(p);
	}
}

void Simulator::run() {
}

void Simulator::displayTrace() const {
}

void Simulator::displayStock() const {
	std::cout << "=== STOCKS ===" << std::endl;
	for (const auto& stock : stocks) {
		std::cout << stock.first << ": " << stock.second << std::endl;
	}
}

void Simulator::displayAll() const {
	std::cout << "=== STOCKS ===" << std::endl;
	for (const auto& stock : stocks) {
		std::cout << stock.first << ": " << stock.second << std::endl;
	}
	
	std::cout << "\n=== PROCESSES ===" << std::endl;
	for (const auto& process : processes) {
		std::cout << "Name: " << process->name << std::endl;
		std::cout << "Delay: " << process->delay << std::endl;
		std::cout << "Needs: ";
		for (const auto& need : process->needs) {
			std::cout << need.first << ":" << need.second << " ";
		}
		std::cout << std::endl;
		std::cout << "Results: ";
		for (const auto& result : process->results) {
			std::cout << result.first << ":" << result.second << " ";
		}
		std::cout << std::endl << std::endl;
	}
	
	std::cout << "\n=== OPTIMIZATIONS ===" << std::endl;
	for (const auto& opt : optimize) {
		std::cout << opt << std::endl;
	}
}

int Simulator::parseOptimize(std::string line) {
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

int Simulator::parseStock(std::string line) {
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

//<name>:(<need>:<qty>;<need>:<qty>):(<result>:<qty>;<result>:<qty>):<nb_cycle>
int Simulator::parseProcess(std::string line)
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

	Process *process = new Process();
	process->name = name;
	process->needs = parseNeeds(needs);
	process->results = parseResults(results);
	process->delay = nb_cycle;
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
			if (parseStock(line) == -1) {
				parsing_step = 1;
			}
		}
		if (parsing_step == 1) {
			if (parseProcess(line) == -1) {
				parsing_step = 2;
			}
		}
		if (parsing_step == 2) {
			if (parseOptimize(line) == -1) {
				parsing_step = 3;
			}
		}
	}
}

void	Simulator::addToStock(State *s,std::unordered_map<std::string, int> itemsToAdd) {
	for (const auto& items : itemsToAdd) {
		auto it = s->stocks.find(items.first);
		if (it == s->stocks.end()) {
			s->stocks.insert(items);
			// std::cout << "Added \033[31m" << items.second << " \033[32m" << items.first << "\033[0m to stocks" << std::endl;
		}
		else {
			it->second += items.second;
			// std::cout << "Added \033[31m" << items.second << " \033[32m" << items.first << "\033[0m to stocks" << std::endl;
		}
	}
}

void	Simulator::subtractFromStock(State *s, std::unordered_map<std::string, int> itemsToSubtract) {
	for (const auto& items : itemsToSubtract) {
		auto it = s->stocks.find(items.first);
		if (it != s->stocks.end()) {
			it->second -= items.second;
			// std::cout << "Remove \033[31m" << items.second << " \033[32m" << items.first << "\033[0m to stocks" << std::endl;
		}
		// else
			// std::cout << "ERROR no items from stock to remove" << items.first << " | " << items.second << std::endl;
	}
}

std::vector<Process *>	Simulator::getExecutableProcesses(const State &state, const std::vector<Process *> &Processes) {
	std::vector<Process *> valid_process;
	
	for (const auto& process : Processes) {
		bool is_valid = true;
		for (const auto& need : process->needs) {
			auto it = state.stocks.find(need.first);
			if (it == state.stocks.end() || it->second < need.second) {
				is_valid = false;
				break ;
			}
		}
		if (is_valid == true) {
			valid_process.push_back(process);
		}
	}
	return (valid_process);
}

State Simulator::applyProcess(const State& current, const Process& process) {
    State next = current;
    
	subtractFromStock(&next, process.needs);

	addToStock(&next, process.results);

    next.time += process.delay;

    next.trace.push_back({next.time, process.name});
    return next;
}

std::vector<State> Simulator::expandState(const State& s, const std::vector<Process *>& processes) {
    std::vector<State> nextStates;

    auto choices = getExecutableProcesses(s, processes);
    for (auto* p : choices) {
        nextStates.push_back(applyProcess(s, *p));
    }

    return nextStates;
}

void Simulator::launchExpand(const State& current, const std::vector<Process *>& processes) {
    std::cout << "\n=== Expansion de l'état au temps " << current.time << " ===\n";

    auto nextStates = this->expandState(current, processes);

    if (nextStates.empty()) {
        std::cout << "Aucun processus exécutable depuis cet état.\n";
        return;
    }

    int i = 0;
    for (const auto& nextState : nextStates) {
        std::cout << "Option " << i++ << " : après process " << nextState.trace.back().process 
                  << " (temps = " << nextState.time << ")\n";

        // afficher stocks pour debug
        std::cout << "Stocks : ";
        for (auto& kv : nextState.stocks) {
            std::cout << kv.first << "=" << kv.second << " ";
        }
        std::cout << "\n";
    }
}

int	Simulator::launchSimulator(void) {
	std::vector<Event> trace;
	State state = {0, this->stocks, trace};

	launchExpand(state, this->processes);
	return (0);
}
