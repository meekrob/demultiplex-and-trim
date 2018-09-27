#include "linked_line.h"
#include <stdio.h>
#include <stdlib.h>

int bad_line(struct linked_line *line)
{
    if (line && line->has_nl) return 0;
    return 1;
}
void print_line(struct linked_line *line)
{
    fwrite(line->head, sizeof(char), line->len, stdout);
    putchar('\n');
}

struct linked_line *parse_fours(struct linked_line *headptr)
{
    // return NULL if the linked list has a multiple of four (no overhang),
    // or return the pointer to the first line of an incomplete group so
    // it may be used to shift the partial data.
    while (headptr != NULL)
    {
        struct linked_line * first = headptr;
        if (bad_line(first)) return headptr;
        struct linked_line * second = first->next;
        if (bad_line(second)) return headptr;
        struct linked_line * third = second->next;
        if (bad_line(third)) return headptr;
        struct linked_line * fourth = third->next;
        if (bad_line(fourth)) return headptr;
        headptr = fourth->next;

        // got 4!
        printf("first line:"); print_line(first);
        printf("second line:"); print_line(second);
        printf("third line:"); print_line(third);
        printf("fourth line:"); print_line(fourth);
    }

    return NULL;

}

int main(int argc, char **argv) 
{
    FILE* infile = fopen(argv[1],"r");
    //int chunksize = 17*4; // must be at least large enough to hold any group of four lines
    int chunksize = 1024*1024;
    char *buf = malloc(sizeof(char)*chunksize);
    char * dst = buf; // pointer to the read destination. May be adjusted due to overhang
    int next_read_size = chunksize;
    int overhang = 0;
    while (!feof(infile)) 
    {
        // read through the input file chunk-wise
        int numread = fread(dst, sizeof(char), next_read_size, infile);
        struct linked_line *headptr = NULL;
        struct linked_line * tailptr = parse_text((char*)buf, numread + overhang, &headptr);
        struct linked_line * incomplete_list = parse_fours(headptr);

        // shift the incomplete data to the top of the buffer
        if (incomplete_list != NULL)
        {
            overhang = &buf[chunksize-1] - incomplete_list->head + 1;
            memmove(buf, incomplete_list->head, overhang); 
            dst = &(buf[overhang]);
        }
        free_list(headptr);
        next_read_size = chunksize - overhang;
    }
    free(buf);
    fclose(infile);
    return 0;
}
