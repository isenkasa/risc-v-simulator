#include "hex.h"
#include "memory.h"
#include <sstream>
#include <cstdint>
#include <iostream>
#include <fstream>

/**
 * Checks if the given address is in the simulated memory. If it is not
 * it prints a warning message to stdout.                              
 * 								      
 * @param i is the given address in which to check.		      
 * 								      
 * @return true or false.					      
 ***********************************************************************/
bool memory::check_address(uint32_t i) const
{
	bool found = false;
	if (i < size) found = true;

	if(found == false)
	{
		std::cout << "WARNING: Address out of range: " << hex0x32(i) << std::endl;
	}

	return found;		
}

/**
 * @return the rounded up siz value in uint32_t.
 ***********************************************************************/
uint32_t memory::get_size() const
{
	return size;
}

/**
 * Checks to see if the given addr is in the memory by calling	     
 * the check_address() method. If the address is in the valid range,  
 * return the value of the byte from the simulated memory at that     
 * address. Otherwise, addr is not in the valid range then return     
 * zero to the caller.						     
 *								     
 * @param addr is the given address.				     
 *								     
 * @return value of the byte or 0 if invalid.			     
 **********************************************************************/
uint8_t memory::get8(uint32_t addr) const
{
	uint8_t value = 0;
	if(check_address(addr))
	{
		value = mem[addr];
	}
	
	return value;
}

/**
 * Calls the get8() function twice to get two bytes and then combines 
 * them in little-endian order to create a 16-bit return value.       
 *								     
 * @param addr is the given address.				     
 * 								     
 * @return the 16 bit value created by combining the bytes.	     
 **********************************************************************/
uint16_t memory::get16(uint32_t addr) const
{
	uint16_t value = get8(addr) | ((uint16_t) get8(addr+1) << 8);
	return value;
}

/**
 * Calls the get16() function twice to get two bytes and then combines
 * them in little-endian order to create a 32-bit return value.       
 *								     
 * @param addr is the given address.				     
 * 								     
 * @return the 32 bit value created by combining the bytes.	     
 **********************************************************************/
uint32_t memory::get32(uint32_t addr) const
{
	uint32_t value =  get16(addr) | ((uint32_t) get16(addr+2) << 16);
	return value;
}

/**
 * Calls check_address() to verify if the addr argument is valid.     
 * If it is, it sets the byte in memory at that address to the given  
 * val. If not, it discards the data and return to the caller.	     
 *								     
 * @param addr is the given address.				     
 * @param val is the given value.				     
 **********************************************************************/
void memory::set8(uint32_t addr, uint8_t val)
{
	if(check_address(addr)) mem[addr] = val;
}

/**
 * Calls set8() twice to store the given val in little-endian order   
 * into the simulated memory starting at the address given in the     
 * addr argument.						     
 *						 		     
 * @param addr is the given address.				     
 * @param val is the given value.				     
 **********************************************************************/
void memory::set16(uint32_t addr, uint16_t val)
{
	// Set MSB
	uint8_t msb = static_cast<uint8_t>((val & 0xFF00) >> 8);
	set8(addr+1, msb);

	// Set LSB
	uint8_t lsb = static_cast<uint8_t>(val & 0x00FF);
	set8(addr, lsb);
}

/**
 * Calls set16() twice to store the given val in little-endian order  
 * into the simulated memory starting at the address given in the     
 * addr argument.						     
 *						 		     
 * @param addr is the given address.				     
 * @param val is the given value.				     
 *********************************************!*************************/
void memory::set32(uint32_t addr, uint32_t val)
{
	// Set MSB
	uint16_t msb = static_cast<uint16_t>((val & 0xFFFF0000) >> 16);
	set16(addr+2, msb);

	// Set LSB
	uint16_t lsb = static_cast<uint16_t>(val & 0x0000FFFF);
	set16(addr, lsb);
}

/**
 * Dumps the entire contents of the simulated memory in hex with ASCII
 * on the right.
 **********************************************************************/
void memory::dump() const
{
	char ascii[17];
	ascii[16] = '\0';

	for (uint32_t i = 0; i < size; ++i)
	{
		if (i!=0 && i%16==0)
			std::cout << '*' << ascii << '*' << std::endl;

		uint8_t ch = get8(i);
		ascii[i%16] = isprint(ch) ? ch : '.';

		if (i%16 == 0)
			std::cout << hex32(i) << ": ";
		else if (i%8 == 0)
			std::cout << ' ';

		std::cout << hex8(ch) << " ";
	}
	if(size > 0)
		std::cout << '*' << ascii << '*' << std::endl;
}

/**
 * Opens the file named fname in binary mode and read its contents    
 * into the simulated memory. If the file can not be opened, prints   
 * error message to stderr and return false. 			     
 * If the file loads OK, closes file and returns true.		     
 *								     
 * @param fname is the name of the file to be opened.		     
 *								     
 * @return true or false, based on the file.			     
 **********************************************************************/
bool memory::load_file(const std::string &fname)
{
	// Open the file
	std::ifstream infile(fname, std::ios::in|std::ios::binary);

	if (!infile)
	{
		std::cerr << "Can\'t open file \'" << fname << "\' for reading.\n";
		return false;
	}
	else
	{
		uint8_t value;
		// Store the first address
		uint32_t address = 0;

		// Loop until EOF
		while(infile.read((char*)&value, 1))
		{
			if(check_address(address))
			{
				set8(address, value);
				address++;	// Go to next address
			}
			else
			{
				std::cerr << "Program too big.\n";
				infile.close();
				return false;
			}
		}
		// If the file loads, close file
		infile.close();
		return true;
	}
}
