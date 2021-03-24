//*********************************************************************
// hex.cpp						              
// CSCI 463 - Assignment 4					      
//							 	      
// Created by Isen Kasoski (Z1875089)				      
//							 	      
//*********************************************************************
#include "hex.h"

#include<sstream>
#include<iomanip>
#include<cstdint>
#include<iostream>

/**
 * This function will return a std::string with exactly 2 hex digits   
 * representing the 8 bits of the i argument.			      
 *								      
 * @param i is a uint8_t that contains the 8 bits to be converted.     
 *								      
 * @return The std::string with exactly 2 hex digits representing the i
 * argument							      
 ***********************************************************************/
std::string hex8(uint8_t i)
{
	std::ostringstream os;
	os << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(i);
	return os.str();
}

/**
 * This function will return a std::string with exactly 8 hex digits   
 * representing the 32 bits of the i argument.			      
 *								      
 * @param i is a uint32_t that contains the 32 bits to be converted.   
 *								      
 * @return The std::string with exactly 8 hex digits representing the i
 * argument							      
 ***********************************************************************/
std::string hex32(uint32_t i)
{
	std::ostringstream os;
	os << std::hex << std::setfill('0') << std::setw(8) << static_cast<uint32_t>(i);
	return os.str();
}

/**
 * This function will return a std::string beginning with 0x,          
 * followed by the 8 hex digits representing the 32 bits of the i arg. 
 * 								      
 * @param i is a uint32_t that contains the 32 bits to be converted.   
 *								      
 * @return The std::string with formatting to represent the i argument 
 ***********************************************************************/
std::string hex0x32(uint32_t i)
{
	return std::string("0x")+hex32(i);
}
