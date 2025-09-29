#include "simulator.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <config_file> [delay]" << std::endl;
		return 1;
	}
	
	std::string filename = argv[1];
	int delay = 100;
	
	if (argc >= 3) {
		try {
			delay = std::stoi(argv[2]);
		} catch (const std::exception& e) {
			std::cerr << "Invalid delay value: " << argv[2] << std::endl;
			return 1;
		}
	}
	
	Simulator sim(filename, delay);
	sim.displayAll();
	return 0;
}