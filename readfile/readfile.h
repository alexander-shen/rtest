#include <stdint.h>
#include <stdbool.h>

/*

The pseudorandom generators (both software and hardware) usually produce sequence of random bits as a sequence of blocks of some size (16-bit, 32-bits etc), and they are written into a file, and then read again, and reinterpreted as a sequence of bits. So the (imaginary) order of bits in bytes and bytes in integers matters.

We assume that the sequence of bit is written horizontally, from left to right, and is split into 8-bit blocs, called bytes. Bytes are identified with 8-bit unsigned integers where the most significant bit is on the left. File is a sequence of bytes, and is written from left to right. The memory is also a sequence of bytes indexed 0,1,... from left to right. 

In Intel64 processors, integers in the memory are written as LSB..MSB, so one cannot read 32 bits from the bit sequence and interpret it as a 32-bit unsigned integer (bytes go in the reversed order). 

Of course, for a truly random sequence the order of bits does not matter, but tests may be sensitive to it.

This difference in the ordering create problems: it would be natural to have a procedure

uint32_t get_next(int i)

that returns i<=32 next bits from the sequence, and returns them as a unsigned integer (with 32 bits), from most significant bit to the least significant bit. But then, if we get 32 bits that were placed in the sequence as an integer, we will get them in the byte-reversed order.

The same problem happens with sequences of 16-bit integers: obtaining them as 16-bit blocks, we will get reversed bit order. 
*/

/* All the procedures report the successful read via ok parameter */


/* start reading file with given name */
void start_read(char* filename, bool *ok);

/* get next i<=32 bits from the stream as an integer with leading zeros added */
uint32_t getnext (int i, bool *ok);

/* get next 32 bits as unsigned integer. Cannot be used after other procedures were called
because alignment is broken */
uint32_t get_uint32(bool *ok);

/* get next 16 bits as unsigned 16-bit integer, with 16 leading zeros added at the left */
uint32_t get_uint16(bool *ok);



