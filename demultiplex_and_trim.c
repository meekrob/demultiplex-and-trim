#include <stdio.h>
#include <stdlib.h>
#define max(a,b) (a>b?a:b)
struct barcode 
{
    char name[16];
    char tag[8];
    int taglen;
    struct barcode *next;
    long int count;
    char outfilename[128];
    FILE *outfile;
};
int hamming(char * tag, char * seq)
{
    int d = 0;
    for (int i = 0; i < strlen(tag); i++)
    {
        d += tag[i] == seq[i] ? 0 : 1;
    }
    return d;
}
struct barcode * match_barcode(struct barcode * bclistptr, char * seq, int mismatches_allowed)
{
    while (bclistptr != NULL)
    {
        int d = hamming(bclistptr->tag, seq);
        if (d  <= mismatches_allowed) return bclistptr;
        bclistptr = bclistptr->next;
    }
    return NULL;
}
struct barcode * new_barcode()
{
    struct barcode *nbc = malloc(sizeof(struct barcode));
    nbc->next = NULL;
    nbc->count = 0;
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
        sprintf(newbarcode->outfilename, "%s.trimmed.fastq", newbarcode->name);
        newbarcode->taglen = strlen(newbarcode->tag);
        newbarcode->outfile = fopen(newbarcode->outfilename, "w");
        printf("%d: %s[%s] -> %s\n", i, newbarcode->name, newbarcode->tag, newbarcode->outfilename);
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
        printf("%d: %s[%s] %ld\n", i++, headptr->name, headptr->tag, headptr->count);
        fclose(headptr->outfile);
        struct barcode *tmp = headptr;
        headptr = headptr->next;
        free(tmp);
    }
}

int main(int argc, char **argv)
{
    if (argc == 1) 
    {
        printf("%s sample.fastq barcode.txt\n", argv[0]);
        return 0;
    }
    // argv[1]: fastq file
    FILE* infile = fopen(argv[1],"r");
    // argv[2]: barcode file
    struct barcode * bc_list = read_barcode_file(argv[2]);

    struct barcode * bc_head = bc_list;
    int max_barcode_len = 0;
    while (bc_head != NULL)
    {
        max_barcode_len = max(max_barcode_len, strlen(bc_head->tag));
        bc_head = bc_head->next;
    }
    printf("max_barcode_len = %d\n", max_barcode_len);

    FILE* unmatched_out = fopen("unmatched.fastq", "w");

    char header[128];
    char basecall[128];
    char plussign[128];
    char quality[128];
    int unmatched_count = 0;
    int nseq = 0;
    while (!feof(infile))
    {
        if(fgets(header, 128, infile)==NULL) break;
        if(fgets(basecall, 128, infile)==NULL) break;
        if(fgets(plussign, 128, infile)==NULL) break;
        if(fgets(quality, 128, infile)==NULL) break;
        // otherwise, we have a full fastq
        nseq++;
        //printf("seq %d: %s\n", nseq, basecall);
        struct barcode * bc = match_barcode(bc_list, basecall, 1);
        //printf("%s: ", basecall);
        if (bc)
        {
            //printf("%s\n", bc->name);
            bc->count++;
            fputs(header, bc->outfile);
            fputs(&basecall[bc->taglen], bc->outfile);
            fputs(plussign, bc->outfile);
            fputs(&quality[bc->taglen], bc->outfile);
        }
        else
        {
            //printf("unmatched\n");
            unmatched_count++;
            fputs(header, unmatched_out);
            fputs(basecall, unmatched_out);
            fputs(plussign, unmatched_out);
            fputs(quality, unmatched_out);
        }
    }
    fclose(infile);

    print_and_free_barcodes(bc_list);
    printf("%d were unmatched\n", unmatched_count);
    fclose(unmatched_out);
    return 0;
}
