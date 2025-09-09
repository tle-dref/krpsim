#include "simulator.hpp"

int main()
{
	Simulator sim("configs/pomme", 100);  // Parse le fichier complet
	// sim.displayAll();  // Affiche tout le contenu parsé
	sim.execAllProcesses();
	return 0;
}