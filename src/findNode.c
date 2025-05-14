#include "../include/header.h"

void findNode(Node *node, char character, char foundBits[100]){
    if (node->character == character){
        strcpy(foundBits, node->bits);
        return;
    }

    if (node->left != NULL) findNode(node->left, character, foundBits);
    if (node->right != NULL) findNode(node->right, character, foundBits);
}