#include "../include/header.h"

/* Compression pipeline */
int compressFile(const char *inputFile) {
    int status;
    // Read & count
    Node *nodes = malloc(sizeof(Node));
    if (!nodes) return -1;
    nodes[0].character = ' ';
    nodes[0].frequency = nodes[0].nodeValue = -1;
    nodes[0].left = nodes[0].right = NULL;
    int charCount = 1;
    status = readFile(inputFile, &nodes, &charCount);
    if (status < 0) { free(nodes); return -1; }

    // Build Huffman tree
    status = huffmanTree(&nodes, charCount);
    if (status < 0) { free(nodes); return -1; }

    // Assign codes
    char **codes = NULL;
    char *characters = NULL;
    int size = 0;
    nodes->bits[0] = '\0';
    assignBit(nodes, &codes, &characters, &size);

    // Write header + compressed data
    if (writeHeader("compressedFile.db", nodes) < 0) { }
    if (writeFile(inputFile, nodes) < 0) { }

    // Cleanup
    for (int i = 0; i < size; i++) free(codes[i]);
    free(codes);
    free(characters);
    free(nodes);
    return 0;
}