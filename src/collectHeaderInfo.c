#include "../include/header.h"

// Recursively traverse the Huffman tree to collect header info for every leaf.
void collectHeaderInfo(Node *node, HeaderInfo **arr, int *count, int *capacity) {
    if (node == NULL) return;
    
    // Only add leaves (nodes that hold an actual character)
    if (node->character != '`') {
        if (*count >= *capacity) {
            *capacity *= 2;
            HeaderInfo *temp = realloc(*arr, (*capacity) * sizeof(HeaderInfo));
            if (temp == NULL) {
                printf("Realloc failure in collectHeaderInfo\n");
                return; // In a real program, you might want to handle this more gracefully.
            }
            *arr = temp;
        }
        HeaderInfo *info = &((*arr)[*count]);
        info->character = node->character;
        info->codeLen = (uint8_t)strlen(node->bits);
        
        // Calculate how many bytes we need to pack the code bits.
        int numBytes = (info->codeLen + 7) / 8;
        info->recordSize = 1 + 2 + 1 + numBytes;  
        
        // Pack the code (a string of '0' and '1') into bytes.
        memset(info->codeData, 0, sizeof(info->codeData));
        int bitIndex = 0, byteIndex = 0;
        unsigned char currentByte = 0;
        for (int i = 0; i < info->codeLen; i++) {
            currentByte = (currentByte << 1) | (node->bits[i] - '0');
            bitIndex++;
            if (bitIndex == 8) {
                info->codeData[byteIndex++] = currentByte;
                bitIndex = 0;
                currentByte = 0;
            }
        }
        if (bitIndex > 0) { // Pad remaining bits (if any) with zeros.
            currentByte = currentByte << (8 - bitIndex);
            info->codeData[byteIndex++] = currentByte;
        }
        (*count)++;
    }
    
    // Traverse left and right subtrees.
    if (node->left) collectHeaderInfo(node->left, arr, count, capacity);
    if (node->right) collectHeaderInfo(node->right, arr, count, capacity);
}