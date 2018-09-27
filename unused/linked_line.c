#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_line.h"
#define DEBUG 0


struct linked_line* new_linked_line(char *head)
{
    struct linked_line* ptr = malloc(sizeof(struct linked_line));
    ptr->head = head;
    ptr->next = NULL;
    ptr->has_nl = 0;
    return ptr;
}

struct linked_line* parse_text(char* txt, size_t txtlen, struct linked_line **_nextptr)
{
    /*
       * _nextptr is initially the reference to headptr in the calling function.
       * But it is changed to point to the __next__ of each individual struct.
       * Therefore, the headptr of the calling function is set to the address of the first
       * struct, but is not modified afterward.
       * ----------
       * Return a reference to the last struct, tailptr
    */
#if DEBUG
    fprintf(stderr, "ENTER parse_text()\n");
    fprintf(stderr, "[%p] _nextptr\n", _nextptr);
    fprintf(stderr, "[%p] *_nextptr\n", *_nextptr);
#endif

    for (int i=0; i< txtlen; i++)
    {
        if (*_nextptr == NULL)
        {
            *_nextptr =  new_linked_line(&txt[i]);
        }
        if (txt[i] == '\n')
        {
            (*_nextptr)->len = (void*)&txt[i] - (void*)(*_nextptr)->head;
            (*_nextptr)->has_nl = 1;
            _nextptr = &(*_nextptr)->next;
        }        
    }
    if (*_nextptr != NULL)
    {
        void * endptr = (void*)&txt[txtlen-1];
        void * headptr = (void*)(*_nextptr)->head;
        (*_nextptr)->len = endptr - headptr;
    }
#if DEBUG
    fprintf(stderr, "LEAVE parse_text()\n");
    fprintf(stderr, "[%p] _nextptr\n", _nextptr);
    fprintf(stderr, "[%p] *_nextptr\n", *_nextptr);
#endif

    return *_nextptr;
}
int print_list(struct linked_line * head)
{
    if (head == NULL) return 1;
    fprintf(stderr, "print_list()\n");
    fprintf(stderr, "head [%p]\n", head);
    struct linked_line * curr = head;
    do 
    {
        printf("linked_line @%p\n", curr);
        printf("...`");
        if (curr->has_nl)
        {
            fwrite(curr->head, sizeof(char), curr->len, stdout);
            printf("'\n");
        }
        else
        {
            fwrite(curr->head, sizeof(char), curr->len+1, stdout);
            printf("'NO EOL\n");
        }
        printf("...head -> %p\n", curr->head);
        printf("...len  -> %d\n", curr->len);
        printf("...next -> %p\n", curr->next);
        /*if (curr->has_nl) // valgrind says conditional jump or move dependant on unitialized value
        {
            printf("HAS newline\n");
        }
        else
        {
            printf("MISSING newline\n");
        }*/
    } while ((curr = curr->next) != NULL);
    return 0;
}
void free_list(struct linked_line * head)
{
    if (head == NULL) return;
    struct linked_line * curr = head;
    do
    {
        struct linked_line * next = curr->next;
        free(curr);
        curr = next;
    } while(curr != NULL);
}
