output: createNode.o writeHeader.o writeFile.o sortNodes.o readFile.o insertCode.o huffmanTree.o freeTrie.o findNode.o decompressFile.o compressFile.o collectHeaderInfo.o assignBit.o main.o 
	gcc ./createNode.o ./writeHeader.o ./writeFile.o ./sortNodes.o ./readFile.o ./insertCode.o ./huffmanTree.o ./freeTrie.o ./findNode.o ./decompressFile.o ./compressFile.o ./collectHeaderInfo.o ./assignBit.o ./main.o -o bin/output

main.o: ./src/main.c
	gcc -Wall -std=c99 -Iinclude ./src/main.c -c 

createNode.o: ./src/createNode.c 
	gcc -Wall -std=c99 -Iinclude ./src/createNode.c -c

assignBit.o: ./src/assignBit.c 
	gcc -Wall -std=c99 -Iinclude ./src/assignBit.c -c

collectHeaderInfo.o: ./src/collectHeaderInfo.c
	gcc -Wall -std=c99 -Iinclude ./src/collectHeaderInfo.c -c 

compressFile.o: ./src/compressFile.c
	gcc -Wall -std=c99 -Iinclude ./src/compressFile.c -c 

decompressFile.o: ./src/decompressFile.c
	gcc -Wall -std=c99 -Iinclude ./src/decompressFile.c -c

findNode.o: ./src/findNode.c 
	gcc -Wall -std=c99 -Iinclude ./src/findNode.c -c

freeTrie.o: ./src/freeTrie.c 
	gcc -Wall -std=c99 -Iinclude ./src/freeTrie.c -c

huffmanTree.o: ./src/huffmanTree.c 
	gcc -Wall -std=c99 -Iinclude ./src/huffmanTree.c -c

insertCode.o: ./src/insertCode.c 
	gcc -Wall -std=c99 -Iinclude ./src/insertCode.c -c

readFile.o: ./src/readFile.c 
	gcc -Wall -std=c99 -Iinclude ./src/readFile.c -c

sortNodes.o: ./src/sortNodes.c 
	gcc -Wall -std=c99 -Iinclude ./src/sortNodes.c -c

writeFile.o: ./src/writeFile.c 
	gcc -Wall -std=c99 -Iinclude ./src/writeFile.c -c

writeHeader.o: ./src/writeHeader.c 
	gcc -Wall -std=c99 -Iinclude ./src/writeHeader.c -c

clean:
	rm -f *.o bin/output