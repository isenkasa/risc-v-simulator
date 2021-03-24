//*********************************************************************
// registerfile.cpp							      
// CSCI 463 - Assignment 5					      
//							 	      
// Created by Isen Kasoski (Z1875089)				      
//							 	      
//*********************************************************************
#include "hex.h"
#include "registerfile.h"
#include <sstream>
#include <string>
#include <cstdint>
#include <iostream>
#include <iomanip>

void registerfile::reset()
{
	regs[0] = 0x0;

	for(int32_t i = 1; i < 32; i++)
	{
		regs[i] = 0xf0f0f0f0;
	}
}

/**
* Assigns register r the given val. If r is zero, it does nothing.
*
* @param r is the register
* @param val is the value to be passed
*******************************************************************************/
void registerfile::set(uint32_t r, int32_t val)
{
	if(r == 0){}
	else regs[r] = val;
}

/**
* Returns the value of register r. If r is zero, it returns zero.
*
* @param r is the register
*******************************************************************************/
int32_t registerfile::get(uint32_t r) const
{
	if(r == 0) return 0;
	else return regs[r];
}

/**
* Dumps the registers in a readable format.
*******************************************************************************/
void registerfile::dump() const
{
	for(uint32_t i = 0; i < 32; ++i)
	{
		if(i!=0 && i%8==0)
			std::cout << std::endl;		

		if(i%8 == 0)
		{
			std::string counter = "x";
			counter += std::to_string(i);
			std::cout << std::setw(3) << counter << " ";
		}

		std::cout << hex32(regs[i]) << " ";
	}
	std::cout << std::endl;
}
