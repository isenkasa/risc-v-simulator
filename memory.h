#include<cstdint>
#include<string>

/*
* The documentation of most of the functions is included in the .cpp file.
*/

class memory
{
public:
	/**
	 * This is the constructor of the class. It stores the value of siz in 
	 * the size member variable. Then, it allocates siz bytes for the mem  
	 * array and it initializes every byte to 0xa5.		  	      
	 *								      
	 * @param siz holds the size to initialize size.			      
	 *								      
	 * @note - You may implement the following rounding logic to make the  
	 * job of formatting and aligning the last line of output in the       
	 * dump() method easier: siz = (siz+15)&0xfffffff0;                    
	 ***********************************************************************/
	memory(uint32_t siz)
	{
		siz = (siz+15)&0xfffffff0;	// round the length up, mod-16
		size = siz;

		mem = new uint8_t[size];

		for(uint32_t i = 0; i < size; i++)
		{
			mem[i] = 0xa5;
		}
	}

	/**
	 * This is the destructor of the class. It frees the memory that was   
	 * allocated in the constructor to represent the simulated memory.     
	 ***********************************************************************/
	~memory()
	{
		delete[] mem;
	}

	bool check_address(uint32_t i) const;
	
	uint32_t get_size() const;

	uint8_t get8(uint32_t addr) const;
	uint16_t get16(uint32_t addr) const;
	uint32_t get32(uint32_t addr) const;

	void set8(uint32_t addr, uint8_t val);
	void set16(uint32_t addr, uint16_t val);
	void set32(uint32_t addr, uint32_t val);

	void dump() const;

	bool load_file(const std::string &fname);
private:
	uint8_t *mem;	// The actual memory buffer
	uint32_t size;
};
