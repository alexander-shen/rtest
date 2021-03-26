#include <stdint.h>
#include <stdio.h>
#include <assert.h>

int main (void)
{
    /* test 1*/
    uint32_t word = 0x0A0B0C0D; // An unsigned 32-bit integer.
    char *pointer = (char *) &word; // A pointer to the first octet of the word.
    
    printf("32 bits order:\n");
    for (int i = 0; i < 4; i++)
    {
        printf("byte[%d] = 0x%02x\n", i, (unsigned int) *(pointer + i));
        assert((unsigned int) *(pointer + i) == 13-i);
    }
    
    /* test 2 */

    uint64_t longword = 0x0102030405060708; // an unsigned 64 bit integer;
    char *pointer64 = (char *) &longword; // A pointer to the first octet of the word.
    printf("64 bits order:\n");
    for (int i = 0; i < 8; i++)
    {
        printf("byte[%d] = 0x%02x\n", i, (unsigned int) *(pointer64 + i));
        assert((unsigned int) *(pointer64 + i) == 8-i);
    }

    /* test 3: fread and fwrite */
    
    FILE *f;
    f = fopen ("tmp","wb");
    char bt;
     /* bt= (char) 'a'; fwrite (&bt,1,1,f);
    bt= (char) 'b'; fwrite (&bt,1,1,f);
    bt= (char) 'c'; fwrite (&bt,1,1,f);
    bt= (char) 'd'; fwrite (&bt,1,1,f);
    fclose(f);
     writes a b c d in the file sequentially */
    f = fopen ("tmp","wb");
    bt= (char) 0x01; fwrite (&bt,1,1,f);
    bt= (char) 0x02; fwrite (&bt,1,1,f);
    bt= (char) 0x03; fwrite (&bt,1,1,f);
    bt= (char) 0x04; fwrite (&bt,1,1,f);
    fclose(f);
    /* writes bytes 01 02 03 04 in the file sequentially */
    f = fopen ("tmp", "rb");
    int buf;
    fread (&buf, 4, 1, f); /* reading one object of size 4 */
    printf("reading bytes written by different chunks:\n");
    for (int i = 0; i < 4; i++)
    {
        printf("byte[%d] = 0x%02x\n", i, (unsigned int) *((char *)(&buf) + i));
        assert ((unsigned int) *((char *)(&buf) + i) == i+1);
    }
    fclose(f);
    /* putting the bytes from memory to file one by one and moving them back by groups of 4 using fread with int does not change the order of bytes in the memory */
    printf("Endianness test ok\n");
}
