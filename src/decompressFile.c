#include "../include/header.h"

/* Decompression pipeline */
int decompressFile(const char *inFile, const char *outFile) {
    FILE *in  = fopen(inFile,  "rb");
    FILE *out = fopen(outFile, "w");
    if (!in || !out) { perror("File open"); return -1; }

    // Read header
    size_t cap = 8, cnt = 0;
    HeaderInfo *hdr = malloc(cap * sizeof *hdr);
    if (!hdr) { fclose(in); fclose(out); return -1; }
    while (1) {
        if (cnt == cap) {
            cap *= 2;
            hdr = realloc(hdr, cap * sizeof *hdr);
            if (!hdr) { fclose(in); fclose(out); return -1; }
        }
        if (fread(&hdr[cnt].character,1,1,in)!=1) break;
        fread(&hdr[cnt].nextOffset, sizeof(uint16_t),1,in);
        fread(&hdr[cnt].codeLen,1,1,in);
        int nB = (hdr[cnt].codeLen + 7)/8;
        fread(hdr[cnt].codeData,1,nB,in);
        cnt++;
        if (hdr[cnt-1].nextOffset == 0) break;
    }

    // Build decode trie
    DecodeNode *root = createNode();
    for (size_t i = 0; i < cnt; i++) {
        insertCode(root, hdr[i].codeData, hdr[i].codeLen, hdr[i].character);
    }
    free(hdr);

    // Decode bitstream
    unsigned char buffer;
    DecodeNode *cur = root;
    while (fread(&buffer,1,1,in)==1) {
        for (int i = 7; i >= 0; i--) {
            int bit = (buffer >> i) & 1;
            cur = bit ? cur->one : cur->zero;
            if (!cur) { fprintf(stderr,"Decoding error\n"); return -1; }
            if (cur->character) {
                fputc(cur->character, out);
                cur = root;
            }
        }
    }

    // Cleanup
    freeTrie(root);
    fclose(in);
    fclose(out);
    return 0;
}