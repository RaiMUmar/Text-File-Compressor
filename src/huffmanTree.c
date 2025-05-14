#include "../include/header.h"

/* Creates a huffman tree using nodes with their corresponding characters */
int huffmanTree(Node **node, int count){
    if (*node == NULL || count < 2) {
        return -1; // File too small
    }

    while (count > 1) {
        qsort(*node, count, sizeof(Node), sortNodes);

        Node *newNode = malloc(sizeof(Node));
        if (newNode == NULL) {
            for (int i = 0; i < count; i++) {
                free((*node)[i].left);
                free((*node)[i].right);
            }
            free(*node);
            return -1; // Memory allocation failure
        }
        newNode->nodeValue = (*node)[0].nodeValue + (*node)[1].nodeValue;
        newNode->left = malloc(sizeof(Node));
        newNode->right = malloc(sizeof(Node));
        newNode->character = '`';

        if (!newNode->left || !newNode->right) {
            free(newNode->left);
            free(newNode->right);
            free(newNode);
            return -1; // Memory allocation failure
        }

        *(newNode->left) = (*node)[1];
        *(newNode->right) = (*node)[0];

        for (int i = 2; i < count; i++) {
            (*node)[i - 2] = (*node)[i];
        }

        Node *temp = realloc(*node, sizeof(Node) * (count - 1));
        if (temp == NULL) {
            free(newNode->left);
            free(newNode->right);
            free(newNode);
            return -1; // Memory allocation failure
        }
        *node = temp;
        (*node)[count - 2] = *newNode;

        free(newNode); // Free temporary node struct
        count--;
    }

    return 1; // Successfully created the Huffman tree
}