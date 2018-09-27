#ifndef LINKED_LINE_H
#define LINKED_LINE_H 1
#include <string.h>
struct linked_line
{
    char * head;
    int len;
    struct linked_line * next;
    int has_nl;
};
struct linked_line* new_linked_line(char *head);
struct linked_line*  parse_text(char* txt, size_t txtlen, struct linked_line **_nextptr);
int print_list(struct linked_line * head);
void free_list(struct linked_line * head);
#endif
