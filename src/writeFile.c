#include "../include/header.h"

/* Write into new file */
int writeFile(const char *fileName, Node *node){
    FILE *readFile  = fopen(fileName, "rb"); 
    FILE *writeFile = fopen("compressedFile.db", "ab");
    if (!readFile || !writeFile) {
        if (readFile) fclose(readFile);
        if (writeFile) fclose(writeFile);
        return -1;
    }

    unsigned char buffer = 0;
    int bitCount = 0;
    int c;
    char foundBits[100];

    while ((c = fgetc(readFile)) != EOF) {
        findNode(node, (char)c, foundBits);
        for (int i = 0; foundBits[i]; i++) {
            buffer = (buffer << 1) | (foundBits[i] - '0');
            if (++bitCount == 8) {
                fwrite(&buffer,1,1,writeFile);
                bitCount = 0;
                buffer = 0;
            }
        }
    }

    // flush leftover bits
    if (bitCount > 0) {
        buffer <<= (8 - bitCount);
        fwrite(&buffer,1,1,writeFile);
    }

    fclose(readFile);
    fclose(writeFile);
    return 1;
}