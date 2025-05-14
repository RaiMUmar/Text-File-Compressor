#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct Node {
    char character;
    int frequency, nodeValue;
    struct Node *left, *right;
    char bits[100];
} Node;

typedef struct {
    char character;
    uint8_t codeLen;               
    unsigned char codeData[13];    
    uint16_t recordSize;           
    uint16_t nextOffset;           
} HeaderInfo;

typedef struct DecodeNode {
    char character;
    struct DecodeNode *zero, *one;
} DecodeNode;

DecodeNode* createNode(void);
int readFile(const char *fileName, Node **node, int *count);
int writeFile(const char *fileName, Node *node);
int writeHeader(const char *fileName, Node *root);
int huffmanTree(Node **node, int count);
int sortNodes(const void *a, const void *b);
void assignBit(Node *node, char ***codes, char **characters, int *size);
void collectHeaderInfo(Node *node, HeaderInfo **arr, int *count, int *capacity);
void findNode(Node *node, char character, char foundBits[100]);
void insertCode(DecodeNode *root, unsigned char *codeData, uint8_t codeLen, char character);
void freeTrie(DecodeNode *root);
int compressFile(const char *inputFile);
int decompressFile(const char *inFile, const char *outFile);

#endif // HEADER_H