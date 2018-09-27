#include <stdio.h>

int main(int argc, char **argv)
{
    for (int q=0; q < 94; q++)
    {
        char ascii = q + 33;
        printf("%d; %c\n", q, ascii);
    }
    return 0;
}
