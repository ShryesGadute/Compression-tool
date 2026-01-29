//bit manipulation macros

const char MASK1 = 0b10000000;
const char MASK2 = 0b11000000;
const char MASK3 = 0b00001111;

//A quad is a group oof four bits like a byte is of eight bits.
//A bat is a pair of two bits.

//sets bit at index i in bit array addr
#define set_bit(addr, i) (addr)[(i)/8] = (addr)[(i)/8] | (MASK1 >> ((i)%8))

//resets bit at index i in bit array addr
#define reset_bit(addr, i) (addr)[(i)/8] = (addr)[(i)/8] & ~(MASK1 >> ((i)%8))

//gets the value of bit at index i in bit array addr
#define get_bit(addr, i) ((addr)[(i)/8] & (MASK1 >> ((i)%8))) ? 1:0

//sets quad at index i in quad array addr to value val. Here we pass val is supposed to be a quad, but we pass a byte whose lowest four lsb bit are val, eg. if we want to set a particular quad to 0111, we pass 0b00000110 as val. it is assumed that the addr has 0000 set where val is supposed to go
#define set_quad(addr, i, val) (addr)[(i)/2] = ((i)%2==0 ? (val << 4) : ((addr)[(i)/2] | val))

//gets the quad at index i in quad array addr, and store it in the first four lsb of a byte
#define get_quad(addr, i) ((i)%2 == 0) ? ((addr)[(i)/2] & (~MASK3)) : ((addr)[(i)/2] & MASK3)

//similar to set quad, but instead of setting 4 bits to a particular value, it sets 2 bits to a particular value
#define set_bat(addr, i, val) (addr)[(i)/4] = ((addr)[(i)/4] | (val << (2*(3-((i)%4)))))

//gets the value of bat at a particular index i in bat array addr
#define get_bat(addr, i) ((addr)[(i)/4] & (MASK2 >> 2*((i)%4))) >> (2*(3-((i)%4)))




