#include "../include/header.h"

// Recursive function which goes through all nodes and assigns bit values as string.
void assignBit(Node *node, char ***codes, char **characters, int *size) {
    if (node->left != NULL) {
        // Propagate parent's code and add "0" for left branch.
        strcpy((node->left)->bits, node->bits);
        strcat((node->left)->bits, "0");
        if ((node->left)->character != '`') {
            (*size)++;
            // Resize and store the character.
            char *tempChar = realloc(*characters, (*size) * sizeof(char));
            if (tempChar == NULL) { printf("Realloc failure\n"); return; }
            *characters = tempChar;
            (*characters)[*size - 1] = (node->left)->character;

            // Resize the codes array and duplicate the bit string.
            char **tempCodes = realloc(*codes, (*size) * sizeof(char*));
            if (tempCodes == NULL) { printf("Realloc failure\n"); return; }
            *codes = tempCodes;
            (*codes)[*size - 1] = strdup((node->left)->bits);
        }
        assignBit(node->left, codes, characters, size);
    }
    if (node->right != NULL) {
        // Propagate parent's code and add "1" for right branch.
        strcpy((node->right)->bits, node->bits);
        strcat((node->right)->bits, "1");
        if ((node->right)->character != '`') {
            (*size)++;
            char *tempChar = realloc(*characters, (*size) * sizeof(char));
            if (tempChar == NULL) { printf("Realloc failure\n"); return; }
            *characters = tempChar;
            (*characters)[*size - 1] = (node->right)->character;

            char **tempCodes = realloc(*codes, (*size) * sizeof(char*));
            if (tempCodes == NULL) { printf("Realloc failure\n"); return; }
            *codes = tempCodes;
            (*codes)[*size - 1] = strdup((node->right)->bits);
        }
        assignBit(node->right, codes, characters, size);
    }
}