#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <strings.h> // for bzero

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

int encode(char* sequence, int len)
{
    int index = 0;
    for (int i = 0; i < len; i++)
    {
        index |= LOOKUP[(int)sequence[i]] << 3*i;
    }
    return index;
}

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
        else if (newbarcode->name[0] == '#') // parsed a header
        {
            free(newbarcode);
            continue; 
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
        printf("To pipe to this command, do something like:\n");
        printf("\tzcat file.fastq.gz | %s /dev/stdin barcode.txt\n", argv[0]);
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
    int score_index_size = 1 << 3*max_barcode_len;
    printf("max_barcode_len = %d. Size of score array: %d\n", max_barcode_len, score_index_size);
    struct barcode ** score_table = malloc(sizeof(struct barcode*)*score_index_size);
    bzero(score_table, sizeof(struct barcode*)*score_index_size);
    

    // "unmatched" barcode obj
    struct barcode unmatched_bc = {
        "unmatched", "", // name, barcode
        0, NULL, 0, // length, next, count
        "unmatched.fastq", NULL // outfilename, outfile*
    };
    unmatched_bc.outfile = fopen(unmatched_bc.outfilename, "w");

    char header[128];
    char basecall[128];
    char plussign[128];
    char quality[128];
    int nseq = 0;
    int lookups = 0;
    while (!feof(infile))
    {
        if(fgets(header, 128, infile)==NULL) break;
        if(fgets(basecall, 128, infile)==NULL) break;
        if(fgets(plussign, 128, infile)==NULL) break;
        if(fgets(quality, 128, infile)==NULL) break;
        // otherwise, we have a full fastq
        nseq++;
        int index = encode(basecall, max_barcode_len);
        if (score_table[index] == NULL)
        {
            struct barcode * bc = match_barcode(bc_list, basecall, 1);
            score_table[index] = (bc!=NULL) ? bc : &unmatched_bc;
            lookups++;
        }
        struct barcode * match = score_table[index];
        match->count++;
        fputs(header, match->outfile);
        fputs(&basecall[match->taglen], match->outfile);
        fputs(plussign, match->outfile);
        fputs(&quality[match->taglen], match->outfile);

        /*
        struct barcode * bc = match_barcode(bc_list, basecall, 1);

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
            unmatched_bc.count++;
            fputs(header, unmatched_bc.outfile);
            fputs(basecall, unmatched_bc.outfile);
            fputs(plussign, unmatched_bc.outfile);
            fputs(quality, unmatched_bc.outfile);
        }
*/
    }
    fclose(infile);

    print_and_free_barcodes(bc_list);
    printf("%ld were unmatched\n", unmatched_bc.count);
    printf("performed %d lookups for all %d seqs\n", lookups, nseq);
    fclose(unmatched_bc.outfile);
    free(score_table);
    return 0;

}
