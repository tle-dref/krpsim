#include "simulator.hpp"

int main(int argc, char **argv)
{
	if (argc != 3) {
		std::cout << "Need 2 args, config path file, delay max" << std::endl;
		return (-1);
	}
	Simulator sim(argv[1], atoi(argv[2]));  // Parse le fichier complet
	// sim.displayAll();  // Affiche tout le contenu parsé
	// sim.execAllProcesses(); //essaye tous les processes 
	sim.launchSimulator();
	return 0;
}