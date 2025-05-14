# Text File Compressor & Decompressor

A C-based command-line utility that compresses and decompresses text files using Huffman Coding. It supports binary compression of character data, preserves bit-efficient representations, and rebuilds the original file using a custom header format.

## 🔧 Features

- Compresses plain text files into a binary `.db` file using Huffman encoding
- Decompresses the binary back into a readable text file
- Builds a Huffman tree based on character frequencies
- Generates a binary header with compact bit-packed Huffman codes
- Includes custom bit manipulation and trie-based decoding
- Handles large files with dynamic memory management

## 📦 How It Works

### Compression

1. **Read Input**: Counts frequency of each character in the text file
2. **Build Huffman Tree**: Constructs an optimal binary tree for encoding
3. **Assign Codes**: Creates a map of characters to binary Huffman codes
4. **Write Header**: Stores metadata (character, bit-length, packed bits) for decoding
5. **Write Data**: Encodes the file content using the Huffman codes and saves as `compressedFile.db`

### Decompression

1. **Read Header**: Parses binary header from `compressedFile.db`
2. **Reconstruct Trie**: Rebuilds decoding tree from stored codes
3. **Decode Stream**: Reads the encoded binary data and writes decoded characters to a new output file

## 🖥️ How to Compile

Compile with:
```bash
make
```

## ▶️ Usage

```bash
./bin/output
```

Then follow the prompts:
```
Enter 'c' to compress or 'd' to decompress:
```

### Compression Example:
```
Input file to compress: ./samples/names.csv
→ wrote compressedFile.db
```

### Decompression Example:
```
Compressed file to decompress: compressedFile.db
Output file name: output.csv
→ wrote output.csv
```

## 📁 Output Files

- `compressedFile.db` – Binary file containing header and compressed content
- `output.csv` – Decompressed text matching the original input

## ⚠️ Notes

- Designed for text files only
- May not compress binary files properly
- Uses static memory and manual memory management (no external libraries)

## 📄 License

This project is open source and licensed under the MIT License.