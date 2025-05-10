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
    uint8_t codeLen;               // Number of bits in the Huffman code.
    unsigned char codeData[13];    // Enough to store up to 100 bits (ceil(100/8)=13).
    uint16_t recordSize;           // Total size of this record (1 + 2 + 1 + numCodeBytes).
    uint16_t nextOffset;           // Offset (from the start of the header) to the next record.
} HeaderInfo;

typedef struct DecodeNode {
    char character;
    struct DecodeNode *zero, *one;
} DecodeNode;

static DecodeNode* createNode(void) {
    DecodeNode *n = calloc(1, sizeof *n);
    if (!n) { perror("calloc"); exit(EXIT_FAILURE); }
    return n;
}

int readFile(const char *fileName, Node **node, int *count);
int writeFile(const char *fileName, Node *node);
int writeHeader(const char *fileName, Node *root);
int huffmanTree(Node **node, int count);
int sortNodes(const void *a, const void *b);
void assignBit(Node *node, char ***codes, char **characters, int *size);
void collectHeaderInfo(Node *node, HeaderInfo **arr, int *count, int *capacity);
void findNode(Node *node, char character, char foundBits[100]);
static void insertCode(DecodeNode *root, unsigned char *codeData, uint8_t codeLen, char character);
static void freeTrie(DecodeNode *root);
int compressFile(const char *inputFile);
int decompressFile(const char *inFile, const char *outFile);


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

int readFile(const char *fileName, Node **node, int *count){
    int found;
    char word[100];

    FILE* fptr = fopen(fileName, "r");
    if (fptr == NULL){
        return -1; // File not found
    }

    while (fscanf(fptr, "%s", word) == 1) {
        for (int i = 0; i < strlen(word); i++){
            found = 0;
            for (int j = 0; j < *count; j++){
                if (word[i] == (*node)[j].character){
                    (*node)[j].frequency++;
                    (*node)[j].nodeValue++;
                    found = 1;
                    break;
                }
            }
            if (!found){
                Node *nodeTemp = realloc(*node, sizeof(Node) * (*count + 1));
                if (nodeTemp == NULL){
                    fclose(fptr);
                    free(nodeTemp);
                    return -1; // Realloc Failure
                }
                *node = nodeTemp;
                (*node)[*count].character = word[i];
                (*node)[*count].frequency = 1;
                (*node)[*count].nodeValue = 1;
                (*node)[*count].left = NULL;
                (*node)[*count].right = NULL;
                (*count)++;
            }
        }
        (*node)[0].frequency++;
        (*node)[0].nodeValue++;
    }
    fclose(fptr);
    return 1;
}

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

int sortNodes(const void *a, const void *b) {
    Node *nodeA = (Node *)a;
    Node *nodeB = (Node *)b;
    return nodeA->nodeValue - nodeB->nodeValue;
}

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
        info->recordSize = 1 + 2 + 1 + numBytes;  // 1 byte for char, 2 for nextOffset, 1 for codeLen, plus code bytes.
        
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

// This function writes the header records to the binary file.
// It should be called before writeFile(). 
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
    
    // Open the file for writing. (Note: if writeFile() later writes compressed data, it should open the file in append mode.)
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


int writeFile(const char *fileName, Node *node){
    char ch, foundBits[100];
    int flag;

    FILE *readFile = fopen(fileName, "r");
    FILE *writeFile = fopen("compressedFile.db", "ab"); // Open in binary write mode.
    if (readFile == NULL || writeFile == NULL){
        printf("File Error!\n");
        return -1;
    }

    unsigned char buffer = 0;
    int bitCount = 0;

    while ((ch = fgetc(readFile)) != EOF) {
        // Find the Huffman bit string for this character.
        findNode(node, ch, foundBits);
        
        // Process each bit in the Huffman code.
        for (int i = 0; i < strlen(foundBits); i++){
            // Convert '0' or '1' to a bit (0 or 1) and shift into buffer.
            buffer = (buffer << 1) | (foundBits[i] - '0');
            bitCount++;
            
            // When we've accumulated 8 bits, write the byte to the file.
            if (bitCount == 8) {
                flag = fwrite(&buffer, sizeof(unsigned char), 1, writeFile);
                if (flag != 1){
                    fclose(readFile);
                    fclose(writeFile);
                    printf("Write Failure\n");
                    return -1;
                }
                // Reset for the next byte.
                bitCount = 0;
                buffer = 0;
            }
        }
    }

    // If there are remaining bits in the buffer, pad them with zeros.
    if (bitCount > 0){
        buffer <<= (8 - bitCount);  // pad with zeros on the right.
        flag = fwrite(&buffer, sizeof(unsigned char), 1, writeFile);
        if (flag != 1){
            fclose(readFile);
            fclose(writeFile);
            printf("Write Failure\n");
            return -1;
        }
    }

    fclose(readFile);
    fclose(writeFile);
    return 1;
}


void findNode(Node *node, char character, char foundBits[100]){
    if (node->character == character){
        strcpy(foundBits, node->bits);
        return;
    }

    if (node->left != NULL) findNode(node->left, character, foundBits);
    if (node->right != NULL) findNode(node->right, character, foundBits);
}


static void insertCode(DecodeNode *root,
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

static void freeTrie(DecodeNode *root) {
    if (!root) return;
    freeTrie(root->zero);
    freeTrie(root->one);
    free(root);
}

// --- Compression pipeline ---
int compressFile(const char *inputFile) {
    int status;
    // 1) Read & count
    Node *nodes = malloc(sizeof(Node));
    if (!nodes) return -1;
    nodes[0].character = ' ';
    nodes[0].frequency = nodes[0].nodeValue = -1;
    nodes[0].left = nodes[0].right = NULL;
    int charCount = 1;
    status = readFile(inputFile, &nodes, &charCount);
    if (status < 0) { free(nodes); return -1; }

    // 2) Build Huffman tree
    status = huffmanTree(&nodes, charCount);
    if (status < 0) { free(nodes); return -1; }

    // 3) Assign codes
    char **codes = NULL;
    char *characters = NULL;
    int size = 0;
    nodes->bits[0] = '\0';
    assignBit(nodes, &codes, &characters, &size);

    // 4) Write header + compressed data
    if (writeHeader("compressedFile.db", nodes) < 0) { }
    if (writeFile(inputFile, nodes) < 0) { }

    // 5) Cleanup
    for (int i = 0; i < size; i++) free(codes[i]);
    free(codes);
    free(characters);
    free(nodes);
    return 0;
}

// --- Decompression pipeline ---
int decompressFile(const char *inFile, const char *outFile) {
    FILE *in  = fopen(inFile,  "rb");
    FILE *out = fopen(outFile, "w");
    if (!in || !out) { perror("File open"); return -1; }

    // 1) Read header
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

    // 2) Build decode trie
    DecodeNode *root = createNode();
    for (size_t i = 0; i < cnt; i++) {
        insertCode(root, hdr[i].codeData, hdr[i].codeLen, hdr[i].character);
    }
    free(hdr);

    // 3) Decode bitstream
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

    // 4) Cleanup
    freeTrie(root);
    fclose(in);
    fclose(out);
    return 0;
}