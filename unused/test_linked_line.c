#include "linked_line.h"
#define DEBUG 0

#if 0
const char SAMPLE_TEXT[] = "A word\nAn other word.\nFinal word.";
int main(int argc, char **argv) 
{
    size_t textsize = strlen(SAMPLE_TEXT);
    struct linked_line *headptr = NULL;
    parse_text((char*)SAMPLE_TEXT, textsize, &headptr);
    print_list(headptr);
    free_list(headptr);
    return 0;
}
#endif
#include <stdio.h>
int main(int argc, char **argv) 
{
    FILE* infile = fopen(argv[1],"r");
    int chunksize = 16;
    char buf[chunksize];
    char * dst = buf; // pointer to the read destination. May be adjusted due to overhang
    int next_read_size = chunksize;
    int overhang = 0;
    while (!feof(infile)) {
        int numread = fread(dst, sizeof(char), next_read_size, infile);
        printf("numread = %d\n", numread);

        printf("<<<<<<<<<<<<\n");
        fwrite(buf, sizeof(char), numread, stdout);
        printf(">>>>>>>>>>>>\n");

        struct linked_line *headptr = NULL;
        struct linked_line * tailptr = parse_text((char*)buf, numread + overhang, &headptr);

        print_list(headptr);
        if (tailptr && (!tailptr->has_nl))
        {
            overhang = &buf[chunksize-1] - tailptr->head + 1;
            printf("overhang size is %d bytes\n", overhang);
            // shift the overhang part to the top of the buffer
            memmove(buf, tailptr->head, overhang);
            dst = &(buf[overhang]);
        }
        free_list(headptr);
        next_read_size = chunksize - overhang;
    }


    fclose(infile);
    return 0;
}
