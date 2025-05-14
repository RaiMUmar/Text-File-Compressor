#include "../include/header.h"

void freeTrie(DecodeNode *root) {
    if (!root) return;
    freeTrie(root->zero);
    freeTrie(root->one);
    free(root);
}