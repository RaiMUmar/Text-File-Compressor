#include "../include/header.h"

DecodeNode* createNode(void) {
    DecodeNode *n = calloc(1, sizeof *n);
    if (!n) { perror("calloc"); exit(EXIT_FAILURE); }
    return n;
}