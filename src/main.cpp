#include "../inc/simulator.hpp"

struct ProcDoMontant {
	std::string name = "do_montant";
	std::unordered_map<std::string, int> needs = {{"planche", 1}};
	std::unordered_map<std::string, int> results = {{"montant", 1}};
	int delay = 15;
};

struct ProcDoFond {
	std::string name = "do_fond";
	std::unordered_map<std::string, int> needs = {{"planche", 2}};
	std::unordered_map<std::string, int> results = {{"fond", 1}};
	int delay = 20;
};

struct ProcDoEtagere {
	std::string name = "do_etagere";
	std::unordered_map<std::string, int> needs = {{"planche", 1}};
	std::unordered_map<std::string, int> results = {{"etagere", 1}};
	int delay = 10;
};

struct ProcDoArmoireIkea {
	std::string name = "do_armoire_ikea";
	std::unordered_map<std::string, int> needs = {{"montant", 2}, {"fond", 1}, {"etagere", 3}};
	std::unordered_map<std::string, int> results = {{"armoire", 1}};
	int delay = 30;
};


int	main(void)
{
	Simulator sim = Simulator();
	ProcDoMontant 		do_montant;
	ProcDoFond 			do_fond;
	ProcDoEtagere 		do_etagere;
	ProcDoArmoireIkea 	do_armoire_ikea;
	std::vector<std::string> optimize = {"time", "armoire"};
	std::unordered_map<std::string, int> stocks = {{"planche", 7}};


	return(0);
}