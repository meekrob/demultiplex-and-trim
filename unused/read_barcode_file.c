#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct barcode 
{
    char name[16];
    char tag[8];
    struct barcode *next;
};
int hamming(char * tag, char * seq)
{
    int d = 0;
    for (int i = 0; i < strlen(tag); i++)
    {
        if (tag[i] != seq[i]) d++;
    }
    return d;
}
struct barcode * match_barcode(struct barcode * bclistptr, char * seq, int mismatches_allowed)
{
    while (bclistptr != NULL)
    {
        int d = hamming(bclistptr->tag, seq);
        if (d  < mismatches_allowed) return bclistptr;
        bclistptr = bclistptr->next;
    }
    return NULL;
}
struct barcode * new_barcode()
{
    struct barcode *nbc = malloc(sizeof(struct barcode));
    nbc->next = NULL;
    return nbc;
}
struct barcode * read_barcode_file(char* filename)
{
    FILE *in = fopen(filename, "r");
    int i = 0;
    struct barcode * headptr = NULL;
    struct barcode ** nextptr = &headptr;
    while (!feof(in)) 
    {
        struct barcode *newbarcode = new_barcode();
        if (fscanf(in, "%s\t%s", newbarcode->name, newbarcode->tag) != 2) 
        { 
            free(newbarcode);
            break; 
        }
        *nextptr = newbarcode;
        nextptr = &(newbarcode->next);
        printf("%d: %s[%s]\n", i, newbarcode->name, newbarcode->tag);
        i++;    
    }
    fclose(in);
    return headptr;
}
void print_and_free_barcodes(struct barcode *headptr)
{
    int i = 0;
    while (headptr)
    {
        printf("%d: %s[%s]\n", i++, headptr->name, headptr->tag);
        struct barcode *tmp = headptr;
        headptr = headptr->next;
        free(tmp);
    }
    
}
int main(int argc, char **argv) 
{
    struct barcode * bc = read_barcode_file(argv[1]);
    print_and_free_barcodes(bc);
    return 0;
}
