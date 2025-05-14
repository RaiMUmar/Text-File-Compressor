#include "../include/header.h"

/* Main Function */
int main(void) {
    char choice;
    char inName[256], outName[256];

    printf("Enter 'c' to compress or 'd' to decompress: ");
    if (scanf(" %c", &choice) != 1) return 1;

    if (choice == 'c') {
        printf("Input file to compress: ");
        scanf(" %255s", inName);
        if (compressFile(inName) == 0)
            printf("→ wrote compressedFile.db\n");
        else
            fprintf(stderr, "Compression failed\n");
    }
    else if (choice == 'd') {
        printf("Compressed file to decompress: ");
        scanf(" %255s", inName);
        printf("Output file name: ");
        scanf(" %255s", outName);
        if (decompressFile(inName, outName) == 0)
            printf("→ wrote %s\n", outName);
        else
            fprintf(stderr, "Decompression failed\n");
    }
    else {
        fprintf(stderr, "Invalid option. Use 'c' or 'd'.\n");
        return 1;
    }

    return 0;
}