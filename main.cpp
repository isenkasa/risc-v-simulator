#include "rv32i.h"
#include <stdlib.h>
#include <iostream>
#include <ctype.h>
#include <unistd.h>

/**
 * Prints the usage for the program and terminates the program
 *********************************************************************/
void usage()
{
	std::cerr << "Usage: rv32i [-m hex-mem-size] [-l execution-limit] [-dirz] infile" << std::endl;
	std::cerr << "     -m specify memory size (default = 0x10000)" << std::endl;
	std::cerr << "     -l specify execution limit (default = infinite)" << std::endl;
	std::cerr << "     -d show disassembly before program simulation" << std::endl;
	std::cerr << "     -i show instruction printing during execution" << std::endl;
	std::cerr << "     -r show a dump of the hart status before each exec" << std::endl;
	std::cerr << "     -z show a dump of the hart and memory after simulation" << std::endl;
	exit(1);
}

/**
 * Read a file of RV32I instructions and execute them.
 ********************************************************************/
int main(int argc, char **argv)
{	
	uint32_t memory_limit = 0x1000; // default memory size = 64k
	uint32_t exec_limit = 0;

	bool i_is_on = false;		// show instruction printing during execution.
	bool r_is_on = false;		// show a dump of the hart status before each instruction.
	bool z_is_on = false;		// show a dump of the hart status and memory after the simulation has halted.
	bool d_is_on = false;		// show a disassembly before program simulation begins.

	int opt;

	while ((opt = getopt(argc, argv, "irzdl:m:")) != -1)
	{
		switch (opt)
		{
		case 'i':
			i_is_on = true;
			break;
		case 'r':
			r_is_on = true;
			break;
		case 'z':
			z_is_on = true;
			break;
		case 'd':
			d_is_on = true;
			break;
		case 'l':
			exec_limit = std::stoul(optarg, nullptr, 10);
			break;
		case 'm':
			memory_limit = std::stoul(optarg, nullptr, 16);
			break;
		default: /* '?' */
			usage();
		}
	}	

	if (optind >= argc)
		usage();

	memory mem(memory_limit);

	if (!mem.load_file(argv[optind]))
		usage();

	rv32i sim(&mem);

	if(r_is_on)
	{
		sim.set_show_registers(true);
	}

	if(d_is_on)
	{
		sim.disasm();
		sim.reset();
	}

	if(i_is_on)
		sim.set_show_instructions(true);

	if(exec_limit != 0)
		sim.run(exec_limit);
	else
		sim.run(0);

	if(z_is_on)
	{
		sim.dump();
		mem.dump();
	}

	return 0;
}
