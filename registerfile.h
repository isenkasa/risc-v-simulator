#include<cstdint>
#include<string>

/*
* The documentation of most of the functions is included in the .cpp file.
*/

class registerfile
{
public:
	/**
	* The constructor of the class. It calls the reset() method.
	************************************************************/
	registerfile()
	{ reset(); }

	void reset();
	void set(uint32_t r, int32_t val);
	int32_t get(uint32_t r) const;
	void dump() const;
private:
	int32_t regs[32];
};
