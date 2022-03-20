#include "hex.h"
#include "rv32i.h"
#include <sstream>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <cassert>
#include <iomanip>

/**
* This method is used to disassemble the instructions in the simulated
* memory. To perform this task, it sets pc to zero, and then, for each
* 32-bit word in the memory:
* 	- print the 32-bit hex address in the pc register
*	- fetch the 32-bit instruction from memory at the address
*	  in the pc register
*	- print the instruction as a 32-bit hex value
*	- pass the fetched instruction to decode() to decode and render
*	  the instruction into a printable std::string
*	- print the decoded instruction string returned from decode()
*	- increment pc by 4				      
***********************************************************************/
void rv32i::disasm(void)
{
	pc = 0;
	
	for(uint32_t i = 0; i < mem->get_size(); i = i+4)
	{
		pc = i;

		std::cout << hex32(pc) << ": " << hex32(mem->get32(pc)) 
		<< "  " << decode(mem->get32(pc)) << std::endl;
	}
}

/**
* This method resets the rv32i object and the registerfile.
***********************************************************************/
void rv32i::reset()
{
	pc = 0;
	insn_counter = 0;
	halt = false;
	regs.reset();
}

/**
* The purpose of this function is to return a std::string containing
* the disassembled in-struction text. It is implemented by using a
* switch statement. It uses the value of the opcode extracted with
* get_opcode(insn) as the switch expression.
*
* @param insn the instruction to be decoded
*
* @return the string formatted instruction
**********************************************************************/
std::string rv32i::decode(uint32_t insn) const
{
	uint32_t opcode = get_opcode(insn);
	uint32_t funct3 = get_funct3(insn);
	uint32_t funct7 = get_funct7(insn);
	int32_t imm_i = get_imm_i(insn);

	switch(opcode)
	{
		default:				return render_illegal_insn();
		case opcode_lui:			return render_lui(insn);
		case opcode_auipc:			return render_auipc(insn);
		case opcode_rtype:
			switch(funct3)
			{
			default:			return render_illegal_insn();
			case funct3_add:	
			// funct3 = 000
				switch(funct7)
				{
				default:		return render_illegal_insn();
				case funct7_add:	return render_rtype(insn, "add");
				case funct7_sub:	return render_rtype(insn, "sub");
				}
				assert(0 && "unhandled funct7");
			// funct3 = 001
			case funct3_sll:		return render_rtype(insn, "sll");
			// funct3 = 010
			case funct3_slt:		return render_rtype(insn, "slt");
			// funct3 = 011
			case funct3_sltu:		return render_rtype(insn, "sltu");
			// funct3 = 100
			case funct3_xor:		return render_rtype(insn, "xor");
			// funct3 = 101
			case funct3_srl:
				switch(funct7)
				{
				default:		return render_illegal_insn();
				case funct7_add:	return render_rtype(insn, "srl");
				case funct7_sub:	return render_rtype(insn, "sra");
				}
				assert(0 && "unhandled funct7");
			// funct3 = 110
			case funct3_or:			return render_rtype(insn, "or");
			// funct3 = 111
			case funct3_and:		return render_rtype(insn, "and");
			}
			assert(0 && "unhandled funct3");
		// (I-Type) Load Immediate Instructions
		case opcode_load_imm:
			switch(funct3)
			{
			default:			return render_illegal_insn();
			// funct3 = 000
			case funct3_lb:			return render_itype_load(insn, "lb");
			// funct3 = 001
			case funct3_lh:			return render_itype_load(insn, "lh");
			// funct3 = 010
			case funct3_lw:			return render_itype_load(insn, "lw");
			// funct3 = 100
			case funct3_lbu:		return render_itype_load(insn, "lbu");
			// funct3 = 101
			case funct3_lhu:		return render_itype_load(insn, "lhu");
			}
			assert(0 && "unhandled funct3");
		// I-Type Instructions
		case opcode_itype:
			switch(funct3)
			{
			default:			return render_illegal_insn();
			// 000
			case funct3_addi:		return render_itype_alu(insn, "addi", imm_i);
			// 010
			case funct3_slti:		return render_itype_alu(insn, "slti", imm_i);
			// 011
			case funct3_sltiu:		return render_itype_alu(insn, "sltiu", imm_i);
			// 100
			case funct3_xori:		return render_itype_alu(insn, "xori", imm_i);
			// 110
			case funct3_ori:		return render_itype_alu(insn, "ori", imm_i);
			// 111
			case funct3_andi:		return render_itype_alu(insn, "andi", imm_i);
			// (I-Type) Shift Instructions
			// 001
			case funct3_slli:		return render_itype_shift(insn, "slli");
			// 101
			case funct3_srli:
				switch(funct7)
				{
				default:		return render_illegal_insn();
				case funct7_add:	return render_itype_shift(insn, "srli");
				case funct7_sub:	return render_itype_shift(insn, "srai");
				}
				assert(0 && "unhandled funct7");
			}
			assert(0 && "unhandled funct3");
		case opcode_stype:
			switch(funct3)
			{
			default:			return render_illegal_insn();
			// 000
			case funct3_sb:			return render_stype(insn, "sb");		
			// 001
			case funct3_sh:			return render_stype(insn, "sh");
			// 010
			case funct3_sw:			return render_stype(insn, "sw");
			}
			assert(0 && "unhandled funct3");
		case opcode_btype:
			switch(funct3)
			{
			default:			return render_illegal_insn();
			// 000
			case funct3_beq:		return render_btype(insn, "beq");		
			// 001
			case funct3_bne:		return render_btype(insn, "bne");
			// 100
			case funct3_blt:		return render_btype(insn, "blt");
			// 101
			case funct3_bge:		return render_btype(insn, "bge");
			// 110
			case funct3_bltu:		return render_btype(insn, "bltu");
			// 111
			case funct3_bgeu:		return render_btype(insn, "bgeu");
			}
			assert(0 && "unhandled funct3");
		case opcode_jal:			return render_jal(insn);
		case opcode_jalr:			return render_itype_load(insn, "jalr");
		case opcode_fence:			return render_fence(insn);
		case opcode_ecall_ebreak:		return render_ecall_ebreak(insn);
	}
	assert(0 && "unhandled opcode");
}

/**
* Dumps the state of the hart. It dumps the GP-registers and the PC
* register.
**********************************************************************/
void rv32i::dump() const
{
	regs.dump();
	std::cout << " pc " << hex32(pc) << std::endl;
}

/**
* Mutator for show_instructions
*
* @param b is the value to set up show_instructions
**********************************************************************/
void rv32i::set_show_instructions(bool b)
{
	show_instructions = b;
}

/**
* Mutator for show_registers
*
* @param b is the value to set up show_registers
**********************************************************************/
void rv32i::set_show_registers(bool b)
{
	show_registers = b;
}

/**
* Accessor for show_registers
*
* @return the boolean value of halt
**********************************************************************/
bool rv32i::is_halted() const
{
	return halt;
}

/**
* Executes the given instruction by making use of the get_xxx()
* methods to extract the needed instruction fields to decode the
* instruction and invoke the associated exec_xxx() helper function
* using a switch statement.
*
* @param insn is the instruction coming in
* @param ostream is the ostream object
*
* @note this function must be capable of handling any 32-bit value.
*********************************************************************/
void rv32i::dcex(uint32_t insn, std::ostream* pos)
{
	uint32_t opcode = get_opcode(insn);
	uint32_t funct3 = get_funct3(insn);
	uint32_t funct7 = get_funct7(insn);

	switch(opcode)
	{
		default:				exec_illegal_insn(insn, pos); return;
		case opcode_lui:			exec_lui(insn, pos); return;
		case opcode_auipc:			exec_auipc(insn, pos); return;
		case opcode_jal:			exec_jal(insn, pos); return;
		case opcode_jalr:			exec_jalr(insn, pos); return;
		case opcode_ecall_ebreak:		exec_ebreak(insn, pos); return;
		case opcode_btype:
			switch(funct3)
			{
			default:			exec_illegal_insn(insn, pos); return;
			// 000
			case funct3_beq:		exec_beq(insn, pos); return;		
			// 001
			case funct3_bne:		exec_bne(insn, pos); return;
			// 100
			case funct3_blt:		exec_blt(insn, pos); return;
			// 101
			case funct3_bge:		exec_bge(insn, pos); return;
			// 110
			case funct3_bltu:		exec_bltu(insn, pos); return;
			// 111
			case funct3_bgeu:		exec_bgeu(insn, pos); return;
			}
			assert(0 && "unhandled funct3");	
		// I-Type Instructions
		case opcode_itype:
			switch(funct3)
			{
			default:			exec_illegal_insn(insn, pos); return;
			// 000
			case funct3_addi:		exec_addi(insn, pos); return;
			// 010
			case funct3_slti:		exec_slti(insn, pos); return;
			// 011
			case funct3_sltiu:		exec_sltiu(insn, pos); return;
			// 100
			case funct3_xori:		exec_xori(insn, pos); return;
			// 110 
			case funct3_ori:		exec_ori(insn, pos); return;
			// 111
			case funct3_andi:		exec_andi(insn, pos); return;
			// (I-Type) Shift Instructions
			// 001 
			case funct3_slli:		exec_slli(insn, pos); return;
			// 101
			case funct3_srli:
				switch(funct7)
				{
				default:		exec_illegal_insn(insn, pos); return;
				case funct7_add:	exec_srli(insn, pos); return;
				case funct7_sub:	exec_srai(insn, pos); return;
				}
				assert(0 && "unhandled funct7");
			}
			assert(0 && "unhandled funct3");
		case opcode_load_imm:
			switch(funct3)
			{
			default:			exec_illegal_insn(insn, pos); return;
			// funct3 = 000 
			case funct3_lb:			exec_lb(insn, pos); return;
			// funct3 = 001
			case funct3_lh:			exec_lh(insn, pos); return;
			// funct3 = 010 
			case funct3_lw:			exec_lw(insn, pos); return;
			// funct3 = 100
			case funct3_lbu:		exec_lbu(insn, pos); return;
			// funct3 = 101
			case funct3_lhu:		exec_lhu(insn, pos); return;
			}
			assert(0 && "unhandled funct3");
		case opcode_stype:
			switch(funct3)
			{
			default:			exec_illegal_insn(insn, pos); return;
			// 000
			case funct3_sb:			exec_sb(insn, pos); return;	
			// 001
			case funct3_sh:			exec_sh(insn, pos); return;
			// 010
			case funct3_sw:			exec_sw(insn, pos); return;
			}
			assert(0 && "unhandled funct3");
		case opcode_rtype:
			switch(funct3)
			{
			default:			exec_illegal_insn(insn, pos); return;
			case funct3_add:	
			// funct3 = 000
				switch(funct7)
				{
				default:		exec_illegal_insn(insn, pos); return;
				case funct7_add:	exec_add(insn, pos); return;
				case funct7_sub:	exec_sub(insn, pos); return;
				}
				assert(0 && "unhandled funct7");
			case funct3_sll:		exec_sll(insn, pos); return;
			// funct3 = 010 
			case funct3_slt:		exec_slt(insn, pos); return;
			// funct3 = 011 
			case funct3_sltu:		exec_sltu(insn, pos); return;
			// funct3 = 100
			case funct3_xor:		exec_xor(insn, pos); return;
			// funct3 = 101 
			case funct3_srl:
				switch(funct7)
				{
				default:		exec_illegal_insn(insn, pos); return;
				case funct7_add:	exec_srl(insn, pos); return;
				case funct7_sub:	exec_sra(insn, pos); return;
				}
				assert(0 && "unhandled funct7");
			// funct3 = 110
			case funct3_or:			exec_or(insn, pos); return;
			// funct3 = 111
			case funct3_and:		exec_and(insn, pos); return;
			}
		case opcode_fence:			exec_fence(insn, pos); return;
	}
	assert(0 && "unhandled opcode");
}

/**
* Simulates the execution of one single instruction.
**********************************************************************/
void rv32i::tick()
{
	if(is_halted())
	{
		//do nothing
	}
	else
	{
		insn_counter++;

		if(show_registers)
				dump();

		if(show_instructions)
		{
			std::cout << hex32(pc) << ": " << hex32(mem->get32(pc)) << "  ";
			dcex(mem->get32(pc), &std::cout);
		}
		else
		{
			dcex(mem->get32(pc),nullptr);
		}
	}
}

/**
* Simulates the running of a program by an RV32I machine.
*
* @param limit is the max amount of instructions to execute
********************************************************************/
void rv32i::run(uint64_t limit)
{
	regs.set(2,mem->get_size());

	while(!halt)
	{
		// If limit is set
		if (limit != 0)
		{
			// When the count is limit, stop execution
			if(insn_counter == limit)
				halt = true;
		}

		tick();
	}
	std::cout << "Execution terminated by EBREAK instruction" << std::endl;
	std::cout << insn_counter << " instructions executed" << std::endl;
}

/**
* Simulates the execution of the fence instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_fence(uint32_t insn, std::ostream* pos)
{
	if (pos)
	{
		std::string s = render_fence(insn);
		s.resize(instruction_width, ' ');

		*pos << s << "// fence" << std::endl;
	}
	
	pc += 4;
}

/**
* Simulates the execution of the and instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_and(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	int32_t val = regs.get(rs1) & regs.get(rs2);

	if (pos)
	{
		std::string s = render_rtype(insn, "and");
		s.resize(instruction_width, ' ');

		*pos << s << "// " << "x" << rd << " = " << hex0x32(regs.get(rs1)) << " & " <<
		hex0x32(regs.get(rs2)) << " = " << hex0x32(val) << std::endl;
	}

	regs.set(rd, val);
	pc += 4;
}

/**
* Simulates the execution of the or instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_or(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	int32_t val = regs.get(rs1) | regs.get(rs2);

	if (pos)
	{
		std::string s = render_rtype(insn, "or");
		s.resize(instruction_width, ' ');

		*pos << s << "// " << "x" << rd << " = " << hex0x32(regs.get(rs1)) << " | " <<
		hex0x32(regs.get(rs2)) << " = " << hex0x32(val) << std::endl;
	}

	regs.set(rd, val);
	pc += 4;
}

/**
* Simulates the execution of the sra instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_sra(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	int32_t rs1 = regs.get(get_rs1(insn));
	int32_t rs2 = regs.get(get_rs2(insn)) & 0x0000001f;
	int32_t val = rs1 >> regs.get(get_rs2(insn));

	if (pos)
	{
		std::string s = render_rtype(insn, "sra");
		s.resize(instruction_width, ' ');

		*pos << s << "// " << "x" << rd << " = " << hex0x32(rs1) << " >> " <<
		rs2 << " = " << hex0x32(val) << std::endl;
	}

	regs.set(rd, val);
	pc += 4;
}

/**
* Simulates the execution of the srl instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_srl(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = regs.get(get_rs1(insn));
	int32_t rs2 = regs.get(get_rs2(insn)) & 0x0000001f;
	int32_t val = rs1 >> regs.get(get_rs2(insn));

	if (pos)
	{
		std::string s = render_rtype(insn, "srl");
		s.resize(instruction_width, ' ');

		*pos << s << "// " << "x" << rd << " = " << hex0x32(rs1) << " >> " <<
		rs2 << " = " << hex0x32(val) << std::endl;
	}

	regs.set(rd, val);
	pc += 4;
}

/*
* Simulates the execution of the xor instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_xor(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	int32_t val = regs.get(rs1) ^ regs.get(rs2);

	if (pos)
	{
		std::string s = render_rtype(insn, "xor");
		s.resize(instruction_width, ' ');

		*pos << s << "// " << "x" << rd << " = " << hex0x32(regs.get(rs1)) << " ^ " <<
		hex0x32(regs.get(rs2)) << " = " << hex0x32(val) << std::endl;
	}

	regs.set(rd, val);
	pc += 4;
}

/*
* Simulates the execution of the sltu instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_sltu(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = regs.get(get_rs1(insn));
	uint32_t rs2 = regs.get(get_rs2(insn));
	int32_t val = (rs1 < rs2) ? 1 : 0;

	if (pos)
	{
		std::string s = render_rtype(insn, "sltu");
		s.resize(instruction_width, ' ');

		*pos << s << "// " << "x" << rd << " = (" << hex0x32(rs1) << " <U " <<
		hex0x32(rs2) << ") ? 1 : 0 = " << hex0x32(val) << std::endl;
	}

	regs.set(rd, val);
	pc += 4;
}

/*
* Simulates the execution of the slt instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_slt(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	int32_t val = (regs.get(rs1) < regs.get(rs2)) ? 1 : 0;

	if (pos)
	{
		std::string s = render_rtype(insn, "slt");
		s.resize(instruction_width, ' ');

		*pos << s << "// " << "x" << rd << " = (" << hex0x32(regs.get(rs1)) << " < " <<
		hex0x32(regs.get(rs2)) << ") ? 1 : 0 = " << hex0x32(val) << std::endl;
	}

	regs.set(rd, val);
	pc += 4;
}

/*
* Simulates the execution of the sll instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_sll(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	uint32_t shift = (regs.get(rs2) & 0x0000001f);
	int32_t val = regs.get(rs1) << shift;

	if (pos)
	{
		std::string s = render_rtype(insn, "sll");
		s.resize(instruction_width, ' ');

		*pos << s << "// x" << rd << " = " << hex0x32(regs.get(rs1)) << " << " << shift <<
		" = " << hex0x32(val) << std::endl;
	}

	regs.set(rd, val);
	pc += 4;
}

/**
* Simulates the execution of the sub instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_sub(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);

	int32_t val = regs.get(rs1) - regs.get(rs2);

	if (pos)
	{
		std::string s = render_rtype(insn, "sub");
		s.resize(instruction_width, ' ');

		*pos << s << "// " << "x" << rd << " = " << hex0x32(regs.get(rs1)) << " - " <<
		hex0x32(regs.get(rs2)) << " = " << hex0x32(val) << std::endl;
	}

	regs.set(rd, val);
	pc += 4;
}
/**
* Simulates the execution of the add instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_add(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);

	int32_t val = regs.get(rs1) + regs.get(rs2);

	if (pos)
	{
		std::string s = render_rtype(insn, "add");
		s.resize(instruction_width, ' ');

		*pos << s << "// " << "x" << rd << " = " << hex0x32(regs.get(rs1)) << " + " <<
		hex0x32(regs.get(rs2)) << " = " << hex0x32(val) << std::endl;
	}

	regs.set(rd, val);
	pc += 4;
}

/**
* Simulates the execution of the srai instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_srai(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t shamt = (insn & 0x01f00000) >> 20;
	int32_t val = regs.get(rs1) >> shamt;

	if (pos)
	{
		std::string s = render_itype_shift(insn, "srai");
		s.resize(instruction_width, ' ');

		*pos << s << "// " << "x" << rd << " = " << hex0x32(regs.get(rs1)) << " >> " <<
		shamt << " = " << hex0x32(val) << std::endl;
	}

	regs.set(rd, val);
	pc += 4;
}

/**
* Simulates the execution of the srli instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_srli(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = regs.get(get_rs1(insn));
	int32_t shamt = (insn & 0x01f00000) >> 20;
	int32_t val = rs1 >> shamt;

	if (pos)
	{
		std::string s = render_itype_shift(insn, "srli");
		s.resize(instruction_width, ' ');

		*pos << s << "// " << "x" << rd << " = " << hex0x32(rs1) << " >> " <<
		shamt << " = " << hex0x32(val) << std::endl;
	}

	regs.set(rd, val);
	pc += 4;
}

/**
* Simulates the execution of the slli instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_slli(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t shamt = (insn & 0x01f00000) >> 20;
	int32_t val = regs.get(rs1) << shamt;

	if (pos)
	{
		std::string s = render_itype_shift(insn, "slli");
		s.resize(instruction_width, ' ');

		*pos << s << "// " << "x" << rd << " = " << hex0x32(regs.get(rs1)) << " << " <<
		shamt << " = " << hex0x32(val) << std::endl;
	}

	regs.set(rd, val);
	pc += 4;
}

/**
* Simulates the execution of the andi instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_andi(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);
	int32_t val = regs.get(rs1) & imm_i;

	if (pos)
	{
		std::string s = render_itype_alu(insn, "andi", imm_i);
		s.resize(instruction_width, ' ');

		*pos << s << "// " << "x" << rd << " = " << hex0x32(regs.get(rs1)) << " & " <<
		hex0x32(imm_i) << " = " << hex0x32(val) << std::endl;
	}

	regs.set(rd, val);
	pc += 4;
}

/**
* Simulates the execution of the ori instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_ori(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);
	int32_t val = regs.get(rs1) | imm_i;

	if (pos)
	{
		std::string s = render_itype_alu(insn, "ori", imm_i);
		s.resize(instruction_width, ' ');

		*pos << s << "// " << "x" << rd << " = " << hex0x32(regs.get(rs1)) << " | " <<
		hex0x32(imm_i) << " = " << hex0x32(val) << std::endl;
	}

	regs.set(rd, val);
	pc += 4;
}

/**
* Simulates the execution of the xori instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_xori(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);
	int32_t val = regs.get(rs1) ^ imm_i;

	if (pos)
	{
		std::string s = render_itype_alu(insn, "xori", imm_i);
		s.resize(instruction_width, ' ');

		*pos << s << "// " << "x" << rd << " = " << hex0x32(regs.get(rs1)) << " ^ " <<
		hex0x32(imm_i) << " = " << hex0x32(val) << std::endl;
	}

	regs.set(rd, val);
	pc += 4;
}

/**
* Simulates the execution of the sltiu instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_sltiu(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = regs.get(get_rs1(insn));
	uint32_t imm_i = get_imm_i(insn);
	int32_t val = (rs1 < imm_i) ? 1 : 0;

	if (pos)
	{
		std::string s = render_itype_alu(insn, "sltiu", imm_i);
		s.resize(instruction_width, ' ');

		*pos << s << "// " << "x" << rd << " = (" << hex0x32(rs1) << " <U " <<
		imm_i << ") ? 1 : 0 = " << hex0x32(val) << std::endl;
	}

	regs.set(rd, val);
	pc += 4;
}

/**
* Simulates the execution of the slti instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_slti(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);
	int32_t val = (regs.get(rs1) < imm_i) ? 1 : 0;

	if (pos)
	{
		std::string s = render_itype_alu(insn, "slti", imm_i);
		s.resize(instruction_width, ' ');

		*pos << s << "// " << "x" << rd << " = (" << hex0x32(regs.get(rs1)) << " < " <<
		imm_i << ") ? 1 : 0 = " << hex0x32(val) << std::endl;
	}

	regs.set(rd, val);
	pc += 4;
}

/**
* Simulates the execution of the sw instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_sw(uint32_t insn, std::ostream* pos)
{
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	int32_t imm_s = get_imm_s(insn);
	int32_t address = regs.get(rs1) + imm_s;
	int32_t value = regs.get(rs2);

	if (pos)
	{
		std::string s = render_stype(insn, "sw");
		s.resize(instruction_width, ' ');

		*pos << s << "// m32(" << hex0x32(regs.get(rs1)) << " + " <<
		hex0x32(imm_s) << ") = " << hex0x32(value) << std::endl;
	}

	mem->set32(address, value);
	pc += 4;
}

/**
* Simulates the execution of the sh instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_sh(uint32_t insn, std::ostream* pos)
{
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	int32_t imm_s = get_imm_s(insn);
	int32_t address = regs.get(rs1) + imm_s;
	int32_t value = regs.get(rs2) & 0x0000ffff;

	if (pos)
	{
		std::string s = render_stype(insn, "sh");
		s.resize(instruction_width, ' ');

		*pos << s << "// m16(" << hex0x32(regs.get(rs1)) << " + " <<
		hex0x32(imm_s) << ") = " << hex0x32(value) << std::endl;
	}

	mem->set16(address, value);
	pc += 4;
}

/**
* Simulates the execution of the sb instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_sb(uint32_t insn, std::ostream* pos)
{
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	int32_t imm_s = get_imm_s(insn);
	int32_t address = regs.get(rs1) + imm_s;
	int32_t value = regs.get(rs2) & 0x000000ff;

	if (pos)
	{
		std::string s = render_stype(insn, "sb");
		s.resize(instruction_width, ' ');

		*pos << s << "// m8(" << hex0x32(regs.get(rs1)) << " + " <<
		hex0x32(imm_s) << ") = " << hex0x32(value) << std::endl;
	}

	mem->set8(address, value);
	pc += 4;
}

/**
* Simulates the execution of the lw instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_lw(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = regs.get(get_rs1(insn));
	int32_t imm_i = get_imm_i(insn);
	int32_t val = rs1 + imm_i;
	int32_t from_address = mem->get32(val);

	if (pos)
	{
		std::string s = render_itype_load(insn, "lw");
		s.resize(instruction_width, ' ');

		*pos << s << "// x" << rd << " = sx(m32(" << hex0x32(rs1) << " + " <<
		hex0x32(imm_i) << ")) = " << hex0x32(from_address) << std::endl;
	}

	regs.set(rd, from_address);
	pc += 4;
}

/**
* Simulates the execution of the lh instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_lh(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = regs.get(get_rs1(insn));
	int32_t imm_i = get_imm_i(insn);
	int32_t val = rs1 + imm_i;
	int32_t from_address = mem->get16(val);

	if (from_address & 0x00008000)      // sign extend
		from_address |= 0xffff0000;

	if (pos)
	{
		std::string s = render_itype_load(insn, "lh");
		s.resize(instruction_width, ' ');

		*pos << s << "// x" << rd << " = sx(m16(" << hex0x32(rs1) << " + " <<
		hex0x32(imm_i) << ")) = " << hex0x32(from_address) << std::endl;
	}

	regs.set(rd, from_address);
	pc += 4;
}

/**
* Simulates the execution of the lb instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_lb(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = regs.get(get_rs1(insn));
	int32_t imm_i = get_imm_i(insn);
	int32_t val = rs1 + imm_i;
	int32_t from_address = mem->get8(val);

	if (from_address & 0x00000080)      // sign extend
		from_address |= 0xffffff00;

	if (pos)
	{
		std::string s = render_itype_load(insn, "lb");
		s.resize(instruction_width, ' ');

		*pos << s << "// x" << rd << " = sx(m8(" << hex0x32(rs1) << " + " <<
		hex0x32(imm_i) << ")) = " << hex0x32(from_address) << std::endl;
	}

	regs.set(rd, from_address);
	pc += 4;
}

/**
* Simulates the execution of the lhu instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_lhu(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = regs.get(get_rs1(insn));
	int32_t imm_i = get_imm_i(insn);
	int32_t val = rs1 + imm_i;
	int32_t from_address = mem->get16(val);

	if (pos)
	{
		std::string s = render_itype_load(insn, "lhu");
		s.resize(instruction_width, ' ');

		*pos << s << "// x" << rd << " = zx(m16(" << hex0x32(rs1) << " + " <<
		hex0x32(imm_i) << ")) = " << hex0x32(from_address) << std::endl;
	}

	regs.set(rd, from_address);
	pc += 4;
}

/**
* Simulates the execution of the lbu instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_lbu(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = regs.get(get_rs1(insn));
	int32_t imm_i = get_imm_i(insn);
	int32_t val = rs1 + imm_i;
	int32_t from_address = mem->get8(val);

	if (pos)
	{
		std::string s = render_itype_load(insn, "lbu");
		s.resize(instruction_width, ' ');

		*pos << s << "// x" << rd << " = zx(m8(" << hex0x32(rs1) << " + " <<
		hex0x32(imm_i) << ")) = " << hex0x32(from_address) << std::endl;
	}

	regs.set(rd, from_address);
	pc += 4;
}

/**
* Simulates the execution of the addi instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_addi(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);
	int32_t val = regs.get(rs1) + imm_i;

	if (pos)
	{
		std::string s = render_itype_alu(insn, "addi", imm_i);
		s.resize(instruction_width, ' ');

		*pos << s << "// " << "x" << rd << " = " << hex0x32(regs.get(rs1)) << " + " <<
		hex0x32(imm_i) << " = " << hex0x32(val) << std::endl;
	}

	regs.set(rd, val);
	pc += 4;
}

/**
* Sets the halt flag to terminate the loop, and if the ostream* param
* is not nullptr, it calls render_illegal_insn() to render the proper
* error message.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
**********************************************************************/
void rv32i::exec_illegal_insn(uint32_t insn, std::ostream* pos)
{
	(void)insn;

	if(pos)
	{
		std::string s = render_illegal_insn();
		s.resize(instruction_width, ' ');
		*pos << s << std::endl;
	}
	halt = true;
}

/**
* Simulates the execution of the ebreak instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_ebreak(uint32_t insn, std::ostream* pos)
{
	if(pos)
	{
		std::string s = render_ecall_ebreak(insn);
		s.resize(instruction_width, ' ');
		*pos << s << "// HALT" << std::endl;
	}
	halt = true;
}

/**
* Simulates the execution of the lui instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_lui(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	int32_t imm_u = get_imm_u(insn);

	if (pos)
	{
		std::string s = render_lui(insn);
		s.resize(instruction_width,' ');

		*pos << s << "// " << "x" << rd << " = " << hex0x32(imm_u) << std::endl;
	}
	
	regs.set(rd, imm_u);
	pc += 4;
}
	
/**
* Simulates the execution of the auipc instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_auipc(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	int32_t imm_u = get_imm_u(insn);

	if (pos)
	{
		std::string s = render_auipc(insn);
		s.resize(instruction_width,' ');

		*pos << s << "// " << "x" << rd << " = " << hex0x32(pc) << " + "
		<< hex0x32(imm_u) << " = " << hex0x32(imm_u+pc) << std::endl;
	}

	regs.set(rd, imm_u+pc);
	pc += 4;
}

/**
* Simulates the execution of the jal instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_jal(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	int32_t imm_j = get_imm_j(insn);

	if (pos)
	{
		std::string s = render_jal(insn);
		s.resize(instruction_width,' ');

		*pos << s << "// " << "x" << rd << " = " << hex0x32(pc+4) << ", "
		<< " pc = " << hex0x32(pc) << " + " << hex0x32(imm_j) << " = " << hex0x32(pc+imm_j) << std::endl;
	}

	regs.set(rd, pc+4);
	pc = pc + imm_j;
}

/**
* Simulates the execution of the jalr instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
*********************************************************************/
void rv32i::exec_jalr(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	int32_t imm_i = get_imm_i(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t val = (regs.get(rs1) + imm_i) & 0xfffffffe;

	if (pos)
	{
		std::string s = render_itype_load(insn, "jalr");
		s.resize(instruction_width,' ');

		*pos << s << "// " << "x" << rd << " = " << hex0x32(pc+4) << ", "
		<< " pc = (" << hex0x32(imm_i) << " + " << hex0x32(regs.get(rs1)) << ") & " << hex0x32(0xfffffffe) 
		<< " = " << hex0x32(val)<< std::endl;
	}

	regs.set(rd, pc+4);
	pc = val;
}

/**
* Simulates the execution of the bne instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
**********************************************************************/
void rv32i::exec_bne(uint32_t insn, std::ostream* pos)
{
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	int32_t imm_b = get_imm_b(insn);
	int32_t val = ((regs.get(rs1) != regs.get(rs2)) ? imm_b : 4);

	if (pos)
	{
		std::string s = render_btype(insn, "bne");
		s.resize(instruction_width,' ');

		*pos << s << "// " << "pc += (" << hex0x32(regs.get(rs1)) << " != " << hex0x32(regs.get(rs2)) << " ? " <<
		hex0x32(imm_b) << " : 4) = " << hex0x32(pc+val) << std::endl; 
	}

	pc = pc + val;
}

/**
* Simulates the execution of the blt instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
**********************************************************************/
void rv32i::exec_blt(uint32_t insn, std::ostream* pos)
{
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	int32_t imm_b = get_imm_b(insn);
	int32_t val = ((regs.get(rs1) < regs.get(rs2)) ? imm_b : 4);

	if (pos)
	{
		std::string s = render_btype(insn, "blt");
		s.resize(instruction_width,' ');

		*pos << s << "// " << "pc += (" << hex0x32(regs.get(rs1)) << " < " << hex0x32(regs.get(rs2)) << " ? " <<
		hex0x32(imm_b) << " : 4) = " << hex0x32(pc+val) << std::endl; 
	}

	pc = pc + val;
}

/**
* Simulates the execution of the bge instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
**********************************************************************/
void rv32i::exec_bge(uint32_t insn, std::ostream* pos)
{
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	int32_t imm_b = get_imm_b(insn);
	int32_t val = ((regs.get(rs1) >= regs.get(rs2)) ? imm_b : 4);

	if (pos)
	{
		std::string s = render_btype(insn, "bge");
		s.resize(instruction_width,' ');

		*pos << s << "// " << "pc += (" << hex0x32(regs.get(rs1)) << " >= " << hex0x32(regs.get(rs2)) << " ? " <<
		hex0x32(imm_b) << " : 4) = " << hex0x32(pc+val) << std::endl; 
	}

	pc = pc + val;
}

/**
* Simulates the execution of the bltu instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
**********************************************************************/
void rv32i::exec_bltu(uint32_t insn, std::ostream* pos)
{
	uint32_t rs1 = regs.get(get_rs1(insn));
	uint32_t rs2 = regs.get(get_rs2(insn));
	int32_t imm_b = get_imm_b(insn);
	int32_t val = ((rs1 < rs2) ? imm_b : 4);

	if (pos)
	{
		std::string s = render_btype(insn, "bltu");
		s.resize(instruction_width,' ');

		*pos << s << "// " << "pc += (" << hex0x32(rs1) << " <U " << hex0x32(rs2) << " ? " <<
		hex0x32(imm_b) << " : 4) = " << hex0x32(pc+val) << std::endl; 
	}

	pc = pc + val;
}

/**
* Simulates the execution of the bgeu instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
**********************************************************************/
void rv32i::exec_bgeu(uint32_t insn, std::ostream* pos)
{
	uint32_t rs1 = regs.get(get_rs1(insn));
	uint32_t rs2 = regs.get(get_rs2(insn));
	int32_t imm_b = get_imm_b(insn);
	int32_t val = ((rs1 >= rs2) ? imm_b : 4);

	if (pos)
	{
		std::string s = render_btype(insn, "bgeu");
		s.resize(instruction_width,' ');

		*pos << s << "// " << "pc += (" << hex0x32(rs1) << " >=U " << std::dec << hex0x32(rs2) << " ? " <<
		hex0x32(imm_b) << " : 4) = " << hex0x32(val+pc) << std::endl; 
	}

	pc = pc + val;
}

/**
* Simulates the execution of the beq instruction.
*
* @param insn is the instruction to be executed
*
* @param pos is the ostream object
**********************************************************************/
void rv32i::exec_beq(uint32_t insn, std::ostream* pos)
{
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	int32_t imm_b = get_imm_b(insn);
	int32_t val = ((regs.get(rs1) == regs.get(rs2)) ? imm_b : 4);

	if (pos)
	{
		std::string s = render_btype(insn, "beq");
		s.resize(instruction_width,' ');

		*pos << s << "// " << "pc += (" << hex0x32(regs.get(rs1)) << " == " << hex0x32(regs.get(rs2)) << " ? " <<
		hex0x32(imm_b) << " : 4) = " << hex0x32(pc+val) << std::endl; 
	}

	pc = pc + val;
}

/**
* Extracts and returns the opcode from the given instruction.
*
* @param insn is the instruction.
*
* @return the opcode field of the instruction.
**********************************************************************/
uint32_t rv32i::get_opcode(uint32_t insn) {return (insn & 0x0000007f);}

/**
* Extracts and returns the rd field from the given instruction.
*
* @param insn is the instruction.
*
* @return the rd field of the instruction.
**********************************************************************/
uint32_t rv32i::get_rd(uint32_t insn) {return ((insn & 0x00000f80) >> 7);}

/**
* Extracts and returns the funct3 field from the given instruction.
*
* @param insn is the instruction.
*
* @return the funct3 field of the instruction.
**********************************************************************/
uint32_t rv32i::get_funct3(uint32_t insn) {return ((insn & 0x00007000) >> 12);}

/**
* Extracts and returns the rs1 field from the given instruction.
*
* @param insn is the instruction.
*
* @return the rs1 field of the instruction.
**********************************************************************/
uint32_t rv32i::get_rs1(uint32_t insn) {return  ((insn & 0x000f8000) >> 15);}

/**
* Extracts and returns the rs2 field from the given instruction.
*
* @param insn is the instruction.
*
* @return the rs2 field of the instruction.
**********************************************************************/
uint32_t rv32i::get_rs2(uint32_t insn) {return ((insn & 0x01f00000) >> 20);}

/**
* Extracts and returns the funct7 field from the given instruction.
*
* @param insn is the instruction.
*
* @return the funct7 field of the instruction.
**********************************************************************/
uint32_t rv32i::get_funct7(uint32_t insn) {return ((insn & 0xfe000000) >> 25);}

/**
* Extracts and returns the imm_i field from the given instruction.
*
* @param insn is the instruction.
*
* @return the imm_i field of the instruction.
**********************************************************************/
int32_t  rv32i::get_imm_i(uint32_t insn) 
{
	int32_t imm_i = (insn & 0xfff00000) >> 20; // extract & shift bits

	if (insn & 0x80000000)			   // sign-extend
		imm_i |= 0xfffff000;

	return imm_i;
}

/**
* Extracts and returns the imm_u field from the given instruction.
*
* @param insn is the instruction.
*
* @return the imm_u field of the instruction.
**********************************************************************/
int32_t  rv32i::get_imm_u(uint32_t insn) {return (insn & 0xfffff000);}

/**
* Extracts and returns the imm_b field from the given instruction.
*
* @param insn is the instruction.
*
* @return the imm_b field of the instruction.
**********************************************************************/
int32_t  rv32i::get_imm_b(uint32_t insn) 
{
	int32_t imm_b = (insn & 0x00000f00) >> (8-1); // extract & shift bits 8-11
	imm_b |= (insn & 0x00000080) << (11 - 7);     // extract & shift bit 7
	imm_b |= (insn & 0x7e000000) >> (25-5);	      // extract & shift bits 25-30
	
	if(insn & 0x80000000)
		imm_b |= 0xfffff000;		      // sign-extend	

	return imm_b;
}

/**
* Extracts and returns the imm_s field from the given instruction.
*
* @param insn is the instruction.
*
* @return the imm_s field of the instruction.
**********************************************************************/
int32_t rv32i::get_imm_s(uint32_t insn)
{
	int32_t imm_s = (insn & 0xfe000000) >> (25-5); // extract & shift bits 5-11
	imm_s |= (insn & 0x00000f80) >> (7-0);	       // extract & shift bits 0-4

	if (insn & 0x80000000)			       // sign-extend
		imm_s |= 0xfffff000;

	return imm_s;
}

/**
* Extracts and returns the imm_j field from the given instruction.
*
* @param insn is the instruction.
*
* @return the imm_j field of the instruction.
**********************************************************************/
int32_t  rv32i::get_imm_j(uint32_t insn)
{
	int32_t imm_j = (insn & 0x000ff000);	  // extract bits 12-19
	imm_j |= (insn & 0x7fe00000) >> (21 - 1); // extract & shift bits 21-31
	imm_j |= (insn & 0x00100000) >> (20 - 11);// extract & shift bit 20

	if (insn & 0x80000000)			  // sign-extend
		imm_j |= 0xfff00000;

	return imm_j;
}

/**
* Returns the error code of an unimplemented instruction.
*
* @return Error message.
**********************************************************************/
std::string rv32i::render_illegal_insn() const
{
	return "ERROR: UNIMPLEMENTED INSTRUCTION";
}

/**
* Renders the lui instruction for output.
*
* @param insn is the instruction
*
* @return the rendered lui instruction as a string
*********************************************************************/
std::string rv32i::render_lui(uint32_t insn) const
{
	uint32_t rd = get_rd(insn);
	int32_t imm_u = get_imm_u(insn);

	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ') << std::left 
	<< "lui" << "x" << std::dec << rd << ",0x" << std::hex 
	<< ((imm_u >> 12)&0x0fffff);

	return os.str();
}

/**
* Renders the auipc instruction for output.
*
* @param insn is the instruction
*
* @return the rendered auipc instruction as a string
*********************************************************************/
std::string rv32i::render_auipc(uint32_t insn) const
{
	uint32_t rd = get_rd(insn);
	int32_t imm_u = get_imm_u(insn);

	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ') << std::left 
	<< "auipc" << "x" << std::dec << rd << ",0x" << std::hex 
	<< ((imm_u >> 12)&0x0fffff);

	return os.str();
}

/**
* Renders the jal instruction for output.
*
* @param insn is the instruction
*
* @return the rendered jal instruction as a string
*********************************************************************/
std::string rv32i::render_jal(uint32_t insn) const
{
	uint32_t rd = get_rd(insn);
	int32_t imm_j = get_imm_j(insn);
	uint32_t pcrel_21 = imm_j + pc;

	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ') << std::left 
	<< "jal" << "x" << std::dec << rd << ",0x" << std::hex 
	<< pcrel_21;

	return os.str();
}

/**
* Renders the btype instructions for output.
*
* @param insn is the instruction
*
* @return the rendered btype instruction as a string
*********************************************************************/
std::string rv32i::render_btype(uint32_t insn, const char *mnemonic) const
{
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	int32_t imm_b = get_imm_b(insn);
	uint32_t pcrel_13 = imm_b + pc;

	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ') << std::left 
	<< mnemonic << std::dec << "x" << rs1 << ",x" 
	<< rs2 << ",0x" << std::hex << pcrel_13;	

	return os.str();
}

/**
* Renders the itype_load instructions for output.
*
* @param insn is the instruction
*
* @return the rendered itype_load instruction as a string
*********************************************************************/
std::string rv32i::render_itype_load(uint32_t insn, const char *mnemonic) const
{
	uint32_t rd  = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);

	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ') << std::left 
	<< mnemonic << "x" << std::dec << rd << "," << imm_i 
	<< "(x" << rs1 << ")" ;

	return os.str();
}

/**
* Renders the stype instructions for output.
*
* @param insn is the instruction
*
* @return the rendered stype instruction as a string
*********************************************************************/
std::string rv32i::render_stype(uint32_t insn, const char *mnemonic) const
{
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	int32_t imm_s = get_imm_s(insn);
		
	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ') << std::left 
	<< mnemonic << "x" << std::dec << rs2 << "," << imm_s 
	<< "(x" << rs1 << ")" ;

	return os.str();
}

/**
* Renders the itype_alu instructions for output.
*
* @param insn is the instruction
*
* @return the rendered itype_alu instruction as a string
*********************************************************************/
std::string rv32i::render_itype_alu(uint32_t insn, const char *mnemonic, int32_t imm_i) const
{
	uint32_t rd  = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);

	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ') << std::left 
	<< mnemonic << "x" << std::dec << rd << ",x" << rs1 << "," << imm_i;
	
	return os.str();	
}

/**
* Renders the itype_shift instructions for output.
*
* @param insn is the instruction
*
* @return the rendered itype_shift instruction as a string
*********************************************************************/
std::string rv32i::render_itype_shift(uint32_t insn, const char *mnemonic) const
{
	uint32_t rd  = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);

	int32_t shamt = (insn & 0x01f00000) >> 20; // extract & shift bits

	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ') << std::left 
	<< mnemonic << "x" << std::dec << rd << ",x" << rs1 << "," << shamt;
	
	return os.str();	
}

/**
* Renders the rtype instructions for output.
*
* @param insn is the instruction
*
* @return the rendered rtype instruction as a string
*********************************************************************/
std::string rv32i::render_rtype(uint32_t insn, const char *mnemonic) const
{
	uint32_t rd  = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);

	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ') << std::left 
	<< mnemonic << "x" << std::dec << rd << ",x" << rs1 << ",x" << rs2;

	return os.str();
}

/**
* Renders the fence instruction for output.
*
* @param insn is the instruction
*
* @return the rendered fence instruction as a string
*********************************************************************/
std::string rv32i::render_fence(uint32_t insn) const
{
	uint32_t pred  = (insn & 0x0f000000);
	uint32_t succ = (insn & 0x00f00000);

	std::string p;
	std::string s;

	if(pred == 0x8000000) p = "i";
	else if(pred == 0x4000000) p = "o";
	else if(pred == 0x3000000) p = "rw";
	else if(pred == 0x2000000) p = "r";
	else if(pred == 0x1000000) p = "w";
	else if(pred == 0xf000000) p = "iorw";

	if(succ == 0x800000) s = "i";
	else if(succ == 0x400000) s = "o";
	else if(succ == 0x300000) s = "rw";
	else if(succ == 0x200000) s = "r";
	else if(succ == 0x100000) s = "w";
	else if(succ == 0xf00000) s = "iorw";

	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ') << std::left 
	<< "fence" << p << "," << s;

	return os.str();
}

/**
* Renders the ecall instruction for output.
*
* @param insn is the instruction
*
* @return the rendered ecall instruction as a string
*********************************************************************/
std::string rv32i::render_ecall_ebreak(uint32_t insn) const
{
	if((insn & 0x00100000) == 0x00100000) return "ebreak";
		else return "ecall";
}
