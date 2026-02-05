#ifndef BITMAN_H
#define BITMAN_H
//bit manipulation macros

static const unsigned char ONE = 0b10000000;
static const unsigned char TWO = 0b11000000;
static const unsigned char QUAD = 0b00001111;

#define MASK(n) (ONE >> (n))
#define MASK2(n) (TWO >> (2*(n)))


static inline unsigned char read_bit(unsigned char* addr, unsigned int i){
	return (addr[i/8] & (MASK(0) >> (i%8))) ? 1:0;  
}

static inline void write_bit(unsigned char* addr, unsigned int i, unsigned char val){
	addr[i/8] = (val ? (addr[i/8] | MASK(i%8)) : (addr[i/8] & ~MASK(i%8))); 
}

static inline unsigned char read_bat(unsigned char* addr, unsigned int i){
	return ((addr[i/4] & MASK2(i%4)) >> (2*(3-(i%4))));
}

static inline void write_bat(unsigned char* addr, unsigned int i, unsigned char val){
	addr[i/4] = addr[i/4] | (val << (2*(3-(i%4))));
}

static inline unsigned char read_quad(unsigned char* addr, unsigned int i){
	return ((i%2==0) ? (addr[i/2] >> 4) : (addr[i/2] & QUAD));
}

static inline void write_quad(unsigned char* addr, unsigned int i, unsigned char val){
	addr[i/2] = (i%2==0 ? (val << 4) : (addr[i/2] | val));	
}

static inline unsigned char read_byte(unsigned char* addr, unsigned int i){
	return addr[i];
}

static inline void write_byte(unsigned char* addr, unsigned int i, unsigned char val){
	addr[i] = val;
}
#endif
