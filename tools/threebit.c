#include <stdio.h>
#include <string.h>

#define _X 0x0
#define _A 0x1 // 001
#define _C 0x2 // 010
#define _G 0x3 // 011
#define _T 0x4 // 100
#define _N 0x5 // 101

const char LOOKUP[] = {
_X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X,
_X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X,
_X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X,
_X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X,
_X, _A, _X, _C, _X, _X, _X, _G, _X, _X, _X, _X, _X, _X, _N, _X,
_X, _X, _X, _X, _T, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X,
_X, _A, _X, _C, _X, _X, _X, _G, _X, _X, _X, _X, _X, _X, _N, _X,
_X, _X, _X, _X, _T, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X, _X
};

int encode(char* sequence)
{
    int index = 0;
    for (int i = 0; i < strlen(sequence); i++)
    {
        index |= LOOKUP[(int)sequence[i]] << 3*i;
    }
    return index;
}

// binary 111
#define MASK 0x7

char decode(int sequence)
{
    if ((sequence & MASK) == _G) return 'G';
    if ((sequence & MASK) == _A) return 'A';
    if ((sequence & MASK) == _C) return 'C';
    if ((sequence & MASK) == _T) return 'T';
    if ((sequence & MASK) == _N) return 'N';
    return 'X';
}

int main(int argc, char ** argv)
{
    char *seq = argv[1]; //"CACGCNGT";
    int arrsize = 1;
    for (int i = 0; i < strlen(seq); i++)
    {
        arrsize *= 5;
    }
    printf("arrsize = %d\n", arrsize);
    int sequence = encode(seq);

    printf("sizeof(int) = %lu * 8 = %lu, div by 3 = %lu\n", sizeof(int), sizeof(int)*8,sizeof(int)*8/3);
    printf("encoding %s => %d\n", seq, sequence);
    printf("decoding %s:\n", seq);
    for (int i = 0; i < strlen(seq); i++)
    {
        putchar(decode(sequence));
        sequence >>= 3;
            
    }
    putchar('\n');
            
    
    return 0;
}

