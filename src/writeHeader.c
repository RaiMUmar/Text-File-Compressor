#include "../include/header.h"

/* This function writes the header records to the binary file. */
int writeHeader(const char *fileName, Node *root) {
    int capacity = 10, count = 0;
    HeaderInfo *headers = malloc(capacity * sizeof(HeaderInfo));
    if (!headers) {
        printf("Initial malloc failure in writeHeader\n");
        return -1;
    }
    
    // Collect header records from the Huffman tree.
    collectHeaderInfo(root, &headers, &count, &capacity);
    
    // Compute the "next node" offset for each header record.
    // The first record starts at offset 0. For each record, nextOffset is the cumulative size of all previous records plus the current one.
    uint16_t offset = 0;
    for (int i = 0; i < count; i++) {
        headers[i].nextOffset = (i == count - 1) ? 0 : offset + headers[i].recordSize;
        offset += headers[i].recordSize;
    }
    
    // Open the file for writing
    FILE *fp = fopen("compressedFile.db", "wb");
    if (!fp) {
        printf("File open failure in writeHeader\n");
        free(headers);
        return -1;
    }
    
    // Write each header record.
    for (int i = 0; i < count; i++) {
        // Write the character (1 byte).
        if (fwrite(&(headers[i].character), sizeof(char), 1, fp) != 1) {
            printf("Write failure for character in header\n");
            fclose(fp);
            free(headers);
            return -1;
        }
        // Write the nextOffset (2 bytes).
        if (fwrite(&(headers[i].nextOffset), sizeof(uint16_t), 1, fp) != 1) {
            printf("Write failure for nextOffset in header\n");
            fclose(fp);
            free(headers);
            return -1;
        }
        // Write the code length (1 byte).
        if (fwrite(&(headers[i].codeLen), sizeof(uint8_t), 1, fp) != 1) {
            printf("Write failure for codeLen in header\n");
            fclose(fp);
            free(headers);
            return -1;
        }
        // Write the packed code bits.
        int numBytes = (headers[i].codeLen + 7) / 8;
        if (fwrite(headers[i].codeData, sizeof(unsigned char), numBytes, fp) != (size_t)numBytes) {
            printf("Write failure for codeData in header\n");
            fclose(fp);
            free(headers);
            return -1;
        }
    }
    
    fclose(fp);
    free(headers);
    return 1;
}