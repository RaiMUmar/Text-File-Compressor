#include "../include/header.h"

int sortNodes(const void *a, const void *b) {
    Node *nodeA = (Node *)a;
    Node *nodeB = (Node *)b;
    return nodeA->nodeValue - nodeB->nodeValue;
}