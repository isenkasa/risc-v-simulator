//**********************************************************************
// hex.h								      
// CSCI 463 - Assignment 4					      
//							 	      
// Created by Isen Kasoski (Z1875089)				      
//							 	      
//***********************************************************************
#ifndef hex_H
#define hex_H

#include <cstdint>
#include <string>

/*
* The documentation of each function is included in the .cpp file.
*/

std::string hex8(uint8_t i);
std::string hex32(uint32_t i);
std::string hex0x32(uint32_t i);

#endif
