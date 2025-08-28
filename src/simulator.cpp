#include "../inc/simulatior.hpp"

// Simulator::Simulator(const std::string& filename, int delay) {}

Simulator::Simulator(void) {}

Simulator::~Simulator(void) {}


// void Simulator::parseFile(const std::string& filename) {}

// void Simulator::run() {}

// void Simulator::displayTrace() const {}

std::unordered_map<std::string, int>    Simulator::getStocks(void)
{
    return(this->stocks);
}

std::vector<Process>    Simulator::getProcesses(void)
{
    return(this->processes);
}

std::vector<Event>  Simulator::getTrace(void)
{
    return(this->trace);
}