#include <iostream>
#include <fstream>
#include <algorithm>
#include <queue>
#include <vector>
#include <stack>
#include <utility>
#include <cstdint>


//TODO: Make Huffman sections of code object-oriented
struct TreeNode
{
	TreeNode(TreeNode* leftChild, TreeNode* rightChild, int freq, unsigned char value) : left(leftChild), right(rightChild), frequency(freq), character(value) {}
	TreeNode(TreeNode* leftChild, TreeNode* rightChild, int freq) : left(leftChild), right(rightChild), frequency(freq) {}
	TreeNode() {}
	
	TreeNode* left;
	TreeNode* right;
	int frequency;
	unsigned char character;
};

struct Code
{
	Code(std::vector<char> bits) : bits(bits) {}
	Code() {}
	std::vector<char> bits;
	unsigned char character;
};

struct SearchNode
{
	SearchNode(TreeNode* treeNode, std::vector<char> codeBits) : treeNode(treeNode), codeBits(codeBits) {};
	TreeNode* treeNode;
	std::vector<char> codeBits;
};

int* countCharacters(std::ifstream& file)
{
	int* freq = new int[256];
	std::fill(freq, freq + 256, 0);
	char c;
	while(file.get(c))
	{
		
		freq[(size_t)c]++;
	}
	return freq;
}

TreeNode* buildTree(int* freq)
{
	auto cmp = [](TreeNode* a, TreeNode* b) { return a->frequency > b->frequency; };
	std::priority_queue<TreeNode*, std::vector<TreeNode*>, decltype(cmp)> pq(cmp);
	
	for(int i = 0; i < 256; i++)
	{
		if(freq[i] < 1) continue;
		pq.push(new TreeNode(nullptr,nullptr,freq[i],(unsigned char)i));
	}
	
	if(pq.empty())
	{
		return nullptr;
	}
	
	//pick the two front elements from the top of the queue, create their parent which is sum of their frequencies, then add parent to queue
	while(pq.size() > 1)
	{
		TreeNode* left = pq.top();
		pq.pop();
		TreeNode* right = pq.top();
		pq.pop();
		TreeNode* parent = new TreeNode{left, right, left->frequency + right->frequency};
		pq.push(parent);
	}
	return pq.top();
}

void destroyTree(TreeNode* root)
{
	std::queue<TreeNode*> search;
	search.push(root);
	while(!search.empty())
	{
		TreeNode* node = search.front();
		search.pop();
		if(node->left != nullptr)
		{
			search.push(node->left);
		}
		if(node->right != nullptr)
		{
			search.push(node->right);
		}
		delete(node);
	}
}

std::vector<Code> generateCodes(TreeNode* root)
{
	std::vector<Code> codes(256);
	if(root->left == nullptr && root->right == nullptr)
	{
		std::vector<char> bits = {0};
		codes[root->character] = Code(std::move(bits));
		return codes;
	}
	
	std::stack<SearchNode> search;
	search.emplace(root, std::vector<char>{});
	while(!search.empty())
	{
		TreeNode* node = search.top().treeNode;
		std::vector<char> codeBits = search.top().codeBits;
		search.pop();
		
		if(node->left == nullptr && node->right == nullptr)
		{
			codes[node->character] = Code(std::move(codeBits));
			continue;
		}
		if(node->left != nullptr)
		{
			std::vector<char> leftBits = codeBits;
			leftBits.push_back(0);
			search.emplace(node->left, leftBits);
		}
		if(node->right != nullptr)
		{
			std::vector<char> rightBits = std::move(codeBits);
			rightBits.push_back(1);
			search.emplace(node->right, rightBits);
		}
	}
	return codes;
}

void writeHeader(std::ofstream& outFile, uint16_t frequencyLength, uint8_t bytesPerFrequency, int* freq)
{
	//1 byte-- # of non-padding bits in last
	//for now, its a placeholder and we'll overwrite later
	outFile.put(0);
	
	//2 bytes-- length of frequency table in bytes
	outFile.write((char*)(&frequencyLength), sizeof(uint16_t));
	//1 bytes-- # of bytes that denote each character's frequency
	outFile.put(bytesPerFrequency);
	
	//x bytes-- frequency table bytes
	for(size_t i = 0; i < frequencyLength; i++)
	{
		//1 byte -- the plaintext byte
		outFile.put(i);
		
		unsigned char* ptr = (unsigned char*)&(freq[i]);
		//y bytes-- frequency of that byte
		for(int j = 0; j < bytesPerFrequency; j++)
		{
			outFile.put(ptr[j]);
		}
	}
	//2 bytes-- epsilon that signifies start of content
	outFile.put(0xFF);
	outFile.put(0xFF);
}

void writeEncodedContent(std::ifstream& inFile, std::ofstream& outFile, std::string& outFileName, std::vector<Code>& codes, int* freq)
{
	inFile.clear();
	inFile.seekg(0);
	char c;
	unsigned char bits = 0;
	size_t bitsCounter = 0;
	//TODO: check for big endian/little endian and write in little endian
	while(inFile.get(c))
	{
		std::vector<char>& codeBits = codes[c].bits;
		for(int i = 0; i < codeBits.size(); i++)
		{
			if(bitsCounter == 8)
			{
				outFile.put(bits);
				bits = 0;
				bitsCounter = 0;
			}
			bits = (bits << 1) | codeBits[i];
			bitsCounter++;
		}
	}
	unsigned char validBitsInLastByte = (unsigned char)bitsCounter;
	bits = bits << (8 - bitsCounter);
	outFile.put(bits);
	
	outFile.close();
	std::fstream file(outFileName, std::ios::in | std::ios::out | std::ios::binary);
	file.seekp(0);
	file.put(bitsCounter);
	file.close();
}


void encodeFile(std::ifstream& inFile, std::string& outFileName)
{
	//generating codes
	int* freq = countCharacters(inFile);
	TreeNode* root = buildTree(freq);
	std::vector<Code> codes = generateCodes(root);
	destroyTree(root);
	
	//writing to output file
	std::ofstream outFile(outFileName, std::ios::binary);
	writeHeader(outFile, 256, 4, freq);
	writeEncodedContent(inFile, outFile, outFileName, codes, freq);
	
	
	
	//cleanup
	outFile.close();
	delete freq;
}

void decodeFile(std::ifstream& inFile, std::string& outFileName)
{
	//get information from header
	//1 byte-- # of non-padding bits in last byte
	uint8_t nonPaddingBitsInLastByte;
	inFile.read((char*)(&nonPaddingBitsInLastByte),sizeof(uint8_t));
	
	uint16_t frequencyLength;
	//2 bytes-- length of frequency table in bytes
	inFile.read((char*)(&frequencyLength), sizeof(uint16_t));
	
	uint8_t bytesPerFrequency;
	//1 bytes-- # of bytes that denote each character's frequency
	inFile.read((char*)(&bytesPerFrequency), sizeof(uint8_t));
	
	int* freq = new int[frequencyLength];
	
	//x bytes-- frequency table bytes
	for(size_t i = 0; i < frequencyLength; i++)
	{
		//1 byte -- the plaintext byte
		uint8_t byte;
		inFile.read((char*)(&byte), sizeof(uint8_t));
		
		//y bytes -- the frequency of this byte
		inFile.read((char*)(&(freq[byte])), bytesPerFrequency);
		
	}
	uint16_t epsilon;
	inFile.read((char*)(&epsilon), sizeof(uint16_t));
	
	//decode content
	std::ofstream outFile(outFileName, std::ios::binary);
	TreeNode* root = buildTree(freq);
	TreeNode* node = root;
	size_t bitCounter = 0;
	uint8_t c;
	while(inFile.get((char&)c))
	{
		int validBits = 8;
		if(inFile.peek() == EOF)
		{
			validBits = nonPaddingBitsInLastByte;
		}
		
		for(int i = 7; i >= 8-validBits; i--)
		{
			uint8_t bit = (c >> i)&1;
			
			if(bit)
			{
				node = node->right;
			}
			else
			{
				//check is for edge case where only one char appears in unencoded file
				if(node->left)
					node = node->left;
			}
			if(node->left == nullptr && node->right == nullptr)
			{
				outFile.put(node->character);
				node = root;	
			}
		}
	}
	
	//cleanup
	outFile.close();
	delete freq;
	
	freq = nullptr;
		
	destroyTree(root);
	root = nullptr;
	node = nullptr;
}

//TODO: allow taking in multiple filenames as arguments and encode/decode all of them
//TODO: Make the CLI prettier
int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		std::cout<<"**** Huffman Encode ****" << std::endl;
		std::cout<<"Usage: huffman input_file [-o output_file] "<<std::endl;
		std::cout<<"Options:\n\t-d\tDecode the file (default is encode)"<<std::endl;
		std::cout<<"\t-o\tGive name for output file"<<std::endl;
		return 0;
	}
	
	std::string filename;
	std::string outFileName;
	bool decode = false;
	
	//parsing arguments
	for(int i = 1; i < argc; i++)
	{
		std::string arg = argv[i];
		if(arg == "-d")
		{
			decode = true;
		}
		else if(arg == "-o")
		{
			if(i+1 >= argc || argv[i+1][0] == '-')
			{
				std::cerr << "Output file name flag is set but no name was provided." << std::endl;
				return 1;
			}
			outFileName = argv[i+1];
			i++;
		}
		else if(arg[0] == '-')
		{
			std::cerr << "Error: unknown option " << arg << std::endl;
			return 1;
		}
		else 
		{
			if(!filename.empty())
			{
				std::cerr << "Error: multiple files provided" << std::endl;
				return 1;
			}
			filename = arg;
		}
	}
	if(filename.empty())
	{
		std::cerr << "Error: missing file name" << std::endl;
		return 1;
	}
	
	std::ifstream inFile(filename, std::ios::binary);
	if(!inFile)
	{
		std::cerr << "Error: could not open file" << std::endl;
		return 1;
	}
	
	//ENCODING
	if(!decode)
	{
		if(outFileName.empty())
		{
			outFileName = "encoded_";
			outFileName += filename;
		}
		encodeFile(inFile, outFileName);
		inFile.close();
	}
	//DECODING
	else
	{
		if(outFileName.empty())
		{
			outFileName = "decoded_";
			outFileName += filename;
		}
		decodeFile(inFile, outFileName);
		inFile.close();
	}
}
