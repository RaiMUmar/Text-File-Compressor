#include "../include/header.h"


void insertCode(DecodeNode *root,
                       unsigned char *codeData,
                       uint8_t codeLen,
                       char character)
{
    DecodeNode *cur = root;
    for (int b = 0; b < codeLen; b++) {
        int bytePos = b / 8;
        int bitPos  = 7 - (b % 8);
        int bit     = (codeData[bytePos] >> bitPos) & 1;
        if (bit == 0) {
            if (!cur->zero) cur->zero = createNode();
            cur = cur->zero;
        } else {
            if (!cur->one)  cur->one  = createNode();
            cur = cur->one;
        }
    }
    cur->character = character;
}