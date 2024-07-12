#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <map>
#include <queue>
#include <iomanip>
#include<chrono>
//auto start = std::chrono::high_resolution_clock::now();
//auto end = std::chrono::high_resolution_clock::now();
//auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//cout << "Running time of compression" << duration.count() << " microseconds." << endl;
struct Node {
    int value; // Pixel value
    int freq;  // Frequency of the pixel
    Node* left;
    Node* right;

    Node() : value(-1), left(nullptr), right(nullptr) {}
    Node(int value, int freq) : value(value), freq(freq), left(nullptr), right(nullptr) {}
    Node(int value) : value(value), freq(0), left(nullptr), right(nullptr) {}
    Node(Node* left, Node* right) : value(0), freq(0), left(left), right(right) {}

};

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq; // Min Heap
    }
};

// Function to construct Huffman Tree by minHeap
Node* buildHuffmanTree(std::unordered_map<int, int>& frequencyMap) {
    std::priority_queue<Node*, std::vector<Node*>, Compare> minHeap;

    // Create nodes for each unique pixel and add to min heap
    for (const auto& pair : frequencyMap) {
        minHeap.push(new Node(pair.first, pair.second));//Sort after pushing into heap
    }

    // Build Huffman Tree
    while (minHeap.size() > 1) {
        Node* left = minHeap.top();//Least
        minHeap.pop();
        Node* right = minHeap.top();//Second least
        minHeap.pop();

        //建一個new node左右連起加進去
        Node* combined = new Node(-1, left->freq + right->freq); // -1 indicates an internal node
        combined->left = left;
        combined->right = right;

        minHeap.push(combined);
    }

    return minHeap.top(); // Root of Huffman Tree
}

// Function to encode Huffman Tree
void encodeHuffmanTree(Node* root, std::string str, std::unordered_map<int, std::string>& huffmanCode) {
    if (root == nullptr) {
        return;
    }

    // Found a leaf node
    if (!root->left && !root->right) { 
        huffmanCode[root->value] = str;
    }

    encodeHuffmanTree(root->left, str + "0", huffmanCode);
    encodeHuffmanTree(root->right, str + "1", huffmanCode);
}

// Function to encode specified Image
std::string encodeImage(const std::vector<std::vector<int>>& image,
                        const std::unordered_map<int, std::string>& huffmanCode) {
    std::string encodedImage;
    for (const auto& row : image) {
        for (int pixel : row) {
            encodedImage += huffmanCode.at(pixel);
        }
    }
    return encodedImage;
}

// Function to turn string(encoded image) into binary
std::vector<char> stringToBinary(const std::string& encodedString) {
    std::vector<char> binaryData;
    char currentByte = 0;
    int bitCount = 0;

    for (char bit : encodedString) {
        // Set the bit in currentByte
        currentByte = (currentByte << 1) | (bit == '1');

        bitCount++;
        if (bitCount == 8) {
            // Byte is filled, add to vector
            binaryData.push_back(currentByte);
            currentByte = 0;
            bitCount = 0;
        }
    }

    // Handle the last byte if it's not full
    if (bitCount > 0) {
        currentByte <<= (8 - bitCount);
        binaryData.push_back(currentByte);
    }

    return binaryData;
}

// Function to serialize huffman tree for compressing
void serializeTree(Node* root, std::vector<char>& encoding) {
    if (root == nullptr) return;

    if (!root->left && !root->right) {
        encoding.push_back('1');
        encoding.push_back(root->value);
    }
    else {
        encoding.push_back('0');
        serializeTree(root->left, encoding);
        serializeTree(root->right, encoding);
    }
}

// Function to reconstruct the Huffman tree
Node* decodeHuffmanTree(std::ifstream& file) {
    if (file.eof()) return nullptr;

    char marker;
    file.get(marker);

    if (marker == '1') {
        if (file.eof()) return nullptr;
        char ch;
        file.get(ch);
        return new Node(static_cast<unsigned char>(ch));
    }
    else {
        Node* left = decodeHuffmanTree(file);
        Node* right = decodeHuffmanTree(file);
        return new Node(left, right);
    }
}

// Function to decode the encoded data using the Huffman tree
std::vector<int> decodeData(Node* root, const std::string& encodedData) {
    std::vector<int> decodedData;
    Node* current = root; // For decoding a part of bits

    // Tracing tree using VLR to decode only for leaves
    for (char byte : encodedData) {
        for (int i = 7; i >= 0; --i) {
            bool bit = (byte >> i) & 1; // 提取每個位元

            if (bit == 0)
                current = current->left;
            else
                current = current->right;

            if (current->left == nullptr && current->right == nullptr) {
                decodedData.push_back(current->value);
                current = root; // 重置到根節點
            }                      
        }
    }
    return decodedData;
}

// Function to draw the TABLE of pixel values and corresponding Huffman code
void printHuffmanTable(const std::unordered_map<int, std::string>& huffmanCode) {
    // Sort the map
    std::map<int, std::string> orderedHC(huffmanCode.begin(), huffmanCode.end());

    // 表格標題
    std::cout << std::endl; 
    std::cout << std::left << std::setw(12) << "Pixel Value" << "Huffman Code" << std::endl;
    std::cout << std::string(30, '-') << std::endl; // 分隔線

    // 遍歷映射並打印每個項目
    for (const auto& pair : orderedHC) {
        std::cout << std::left << std::setw(12) << pair.first << pair.second << std::endl;
    }
    std::cout << std::endl;
}

// Function to draw HISTOGRAM of pixel values and corresponding frequency
void printHistogram(const std::unordered_map<int, int>& frequencyMap) {

    // 找出最大頻率
    int maxFrequency = 0;
    for (const auto& pair : frequencyMap) {
        if (pair.second > maxFrequency) {
            maxFrequency = pair.second;
        }
    }

    // 定義最大長度和正規化比例
    const int maxLength = 100; // 可以自定義最大長度
    double normalizationRatio = static_cast<double>(maxLength) / maxFrequency;

    std::map<int, int> orderedFmap(frequencyMap.begin(), frequencyMap.end());


    std::cout << "Histogram of frequency: (one # indicates " << 1 / normalizationRatio << " times, no # indicates less than the number)" << std::endl;
    for (const auto& pair : orderedFmap) {
        int pixelValue = pair.first;
        int normalizedLength = static_cast<int>(pair.second * normalizationRatio);

        std::cout << std::left << std::setw(4) << pixelValue  << ": ";
        for (int i = 0; i < normalizedLength; ++i) {
            std::cout << "#";
        }
        std::cout << std::endl;
    }

}

std::streamsize getFileSize(const std::string& filePath) {
    std::ifstream file(filePath, std::ifstream::ate | std::ifstream::binary);

    if (!file) {
        return -1;
    }

    // Get size of file
    std::streamsize size = file.tellg();
    file.close();
    return size;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " -c/-d filename" << std::endl;
        return 1;
    }
    std::string mode = argv[1];     // "-c" or "-d"
    std::string filename = argv[2]; // File name, e.g., "test.pgm" or "test.hc"
    std::string filename2;          // File name, e.g., "tested.pgm"
    if (argc >= 4) filename2 = argv[3];

    // For encoding
    if (mode == "-c") {
        // Compression code
        std::cout << "Compressing " << filename << std::endl;


        auto start = std::chrono::high_resolution_clock::now();


        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file" << std::endl;
            return 1;
        }

        std::string line;
        getline(file, line);  // Read the magic number (e.g., "P2")
        getline(file, line);  // Read the next line which might be a comment
        if (line[0] == '#') {
            getline(file, line); // Read the next line if the previous was a comment
        }

        std::stringstream ss(line);
        int width, height, maxVal;
        ss >> width >> height;  // Read image dimensions
        file >> maxVal;         // Read maximum pixel value

        std::vector<std::vector<int>> pixels(height, std::vector<int>(width));
        std::unordered_map<int, int> frequencyMap;//<index, frequency>
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                file >> pixels[i][j];
                ++frequencyMap[pixels[i][j]];
            }
        }

        file.close();

        // Now, pixels[][] contains the pixel values of the PGM image

        // Build huffman tree
        Node* root = buildHuffmanTree(frequencyMap);

        // Build huffman code and Serialize
        std::unordered_map<int, std::string> huffmanCode;//<index, encode number>
        encodeHuffmanTree(root, "", huffmanCode);
        std::vector<char> treeStructure;
        serializeTree(root, treeStructure);        

        // Encode specified image
        std::string encodedImageData = encodeImage(pixels, huffmanCode);
        std::vector<char> binaryData = stringToBinary(encodedImageData);

        // Open a file stream to write
        std::string compressedFile = filename + ".hc";
        std::ofstream outputFile(compressedFile, std::ios::binary);
        if (outputFile.is_open()) {
            outputFile.write(reinterpret_cast<const char*>(&width), sizeof(width));
            outputFile.write(reinterpret_cast<const char*>(&height), sizeof(height));

            // Output char(byte) by char
            outputFile.write(treeStructure.data(), treeStructure.size());
            outputFile.write(binaryData.data(), binaryData.size());
            outputFile.close();

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << "Running time of compression is " << duration.count() << " milliseconds." << std::endl;

            std::cout << "Image successfully encoded and saved to '"<< compressedFile <<"'" << std::endl;
        }
        else {
            std::cerr << "Unable to open file for writing." << std::endl;
            return 1;
        }

        printHuffmanTable(huffmanCode);// Print Table
        printHistogram(frequencyMap);  // Print Histogram

        std::streamsize originalSize, compressedSize;
        originalSize = getFileSize(filename);// Print Original size
        compressedSize = getFileSize(compressedFile);// Print Compressed size

        if (originalSize >= 0) {
            std::cout << "Original image size: " << originalSize << " bytes." << std::endl;
        }
        else {
            std::cerr << "Error reading file size." << std::endl;
        }
        if (compressedSize >= 0) {
            std::cout << "Compressed image size: " << compressedSize << " bytes." << std::endl;
        }
        else {
            std::cerr << "Error reading file size." << std::endl;
        }
        //std::cout << "Compression ratio: " << (originalSize - compressedSize) / originalSize << " %" << std::endl;
    }
    // For decoding
    else if (mode == "-d") {
        // Decompression code
        std::cout << "Decompressing " << filename << std::endl;

        auto start = std::chrono::high_resolution_clock::now();

        std::ifstream inputFile(filename, std::ios::binary);
        if (!inputFile.is_open()) {
            std::cerr << "Unable to open file." << std::endl;
            return 1;
        }

        // read width and height
        int width, height;
        inputFile.read(reinterpret_cast<char*>(&width), sizeof(width));
        inputFile.read(reinterpret_cast<char*>(&height), sizeof(height));

        // read tree and reconstruct
        Node* root = decodeHuffmanTree(inputFile);

        // read rest of encoded data and decode
        std::string encodedData(std::istreambuf_iterator<char>(inputFile), {});
        std::vector<int> decodedImage = decodeData(root, encodedData);
        std::cout << "File .hc successfully inputed" << std::endl;
        inputFile.close();

        //------------output below---------------
        std::ofstream outputFile(filename2);
        if (outputFile.is_open()) {
            outputFile << "P2" << std::endl
                       << width << " " << height << std::endl
                       << "255" << std::endl;

            for (int value : decodedImage) {
                outputFile << value << " " ;
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << "Running time of depression is " << duration.count() << " milliseconds." << std::endl;

            std::cout << "Image successfully decoded and saved to '" << filename2 << "'" << std::endl;

        }
        else {
            std::cerr << "Unable to open file for writing." << std::endl;
            return 1;
        }
        
    
        outputFile.close();

    }
    else {
        std::cerr << "Invalid mode. Use -c for compression and -d for decompression." << std::endl;
        return 1;
    }

    

    return 0;
}
