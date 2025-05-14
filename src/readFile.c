#include "../include/header.h"

/* Read File and save characters along with their frequency */
int readFile(const char *fileName, Node **node, int *count){
    FILE *fptr = fopen(fileName, "rb");    
    if (!fptr) return -1;

    int c;
    while ((c = fgetc(fptr)) != EOF) {
        char ch = (char)c;
        int found = 0;
        for (int j = 0; j < *count; j++) {
            if ((*node)[j].character == ch) {
                (*node)[j].frequency++;
                (*node)[j].nodeValue++;
                found = 1;
                break;
            }
        }
        if (!found) {
            Node *tmp = realloc(*node, sizeof(Node) * (*count + 1));
            if (!tmp) { fclose(fptr); return -1; }
            *node = tmp;
            (*node)[*count].character = ch;
            (*node)[*count].frequency = 1;
            (*node)[*count].nodeValue = 1;
            (*node)[*count].left = (*node)[*count].right = NULL;
            (*count)++;
        }
    }
    fclose(fptr);
    return 1;
}