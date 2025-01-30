
#include <iostream>
#include <vector>
#include <string>
#include <Windows.h>
#include <sstream>
#include <iosfwd>
#include <iomanip>
#include <ios>
#include <fstream>
#include <stdlib.h>
#include <cstdint>
#include <iterator>


// Initial key
const uint32_t KEY = 0xa7d46c76;

/*
* stdlib.h
unsigned int __cdecl _rotl(
	_In_ unsigned int _Value,
	_In_ int          _Shift
	);
*/

// Rotate Left
inline uint32_t _ROTL(uint32_t value, int shift) {
	return _rotl(value, shift);
}

// Rotate Right
inline uint32_t _ROTR(uint32_t value, int shift) {
	return _rotl(value, -shift);
}

std::vector<BYTE> hex_to_bytes(const std::string& hex) {
	std::vector<BYTE> bytes;
	for (size_t i = 0; i < hex.length(); i += 2) {
		std::string byte_string = hex.substr(i, 2);
		BYTE byte = static_cast<BYTE>(strtol(byte_string.c_str(), nullptr, 16));
		bytes.push_back(byte);
	}
	return bytes;
}

void append_decrypted_4bytes(std::vector<BYTE>& decrypted, uint32_t decrypted_4bytes) {
	decrypted.push_back((decrypted_4bytes >> 24) & 0xFF); // Most significant byte (24-31 bits)
	decrypted.push_back((decrypted_4bytes >> 16) & 0xFF); // Second most significant byte (16-23 bits)
	decrypted.push_back((decrypted_4bytes >> 8) & 0xFF);  // Second least significant byte (8-15 bits)
	decrypted.push_back(decrypted_4bytes & 0xFF);         // Least significant byte (0-7 bits)
}

uint32_t decrypt_4bytes(uint32_t _4bytes_encrypted, uint32_t keystream_value) {

	// Rotate left for 3 bits
	uint32_t _4bytes_rotated = _ROTL(_4bytes_encrypted, 3);

	// xor keystream with rotated 
	uint32_t _4bytes_rotated_xored = keystream_value ^ _4bytes_rotated;

	return _4bytes_rotated_xored;
}

std::vector<uint32_t> generate_keystream(size_t size_in_dwords, uint32_t _key) {
	std::vector<uint32_t> keystream(size_in_dwords);

	// Generate key, skip (i = 0) 1st 4 bytes because they are not encrypted
	for (size_t i = 1; i < size_in_dwords; ++i) {
		// Store key to keystream
		keystream[i] = _key;
		// Rotate key left for 5 bits
		_key = _ROTL(_key, 5);
	}
	return keystream;
}

std::vector<BYTE> decrypt_DXXD_Ransomware(const std::vector<BYTE>& encrypted) {
	std::cout << "Decrypting ..." << std::endl;
	std::vector<BYTE> decrypted = {  };
	uint32_t* encrypted_data = (uint32_t*)encrypted.data();
	size_t encrypted_buffer_size_in_dwords = encrypted.size() / sizeof(uint32_t);

	// Generate keystream for data buffer
	std::vector<uint32_t> keystream = generate_keystream(encrypted_buffer_size_in_dwords, KEY);

	// Swap 1st 4 bytes, BE (Big Endian) to LE (Little Endian)
	uint32_t first4bytes = _byteswap_ulong(encrypted_data[0]);

	// Append 1st 4 bytes that are not encrypted
	append_decrypted_4bytes(decrypted, first4bytes);

	// Decrypt buffer, skip 1st 4 bytes
	for (size_t i = 1; i < encrypted_buffer_size_in_dwords; ++i) {
		//std::cout << "Decrypting:        " << encrypted_data[i] << std::endl;

		// Decrypt 4 bytes at time with unique key
		uint32_t decrypted_4bytes = decrypt_4bytes(encrypted_data[i], keystream[i]);
		//std::cout << "decrypted_4bytes:  " << decrypted_4bytes << std::endl;

		append_decrypted_4bytes(decrypted, decrypted_4bytes);
		//std::cout << "decrypted.data():  " << decrypted.data() << std::endl;
	}

	//// Trim last bytes if required so decrypted file matches original bytes
	//if (decrypted.size() != encrypted.size()) {
	//	std::cout << "Trimming  string end ..." << std::endl;
	//	decrypted.resize(encrypted.size());
	//}

	return decrypted;
}

std::string read_file_as_hex_string(const std::string& filename) {

	// Open file as binary
	std::ifstream file(filename, std::ios::binary);

	if (!file.is_open()) {
		std::cerr << "Can't open file." << std::endl;
		return {};
	}

	// Load binary data into the buffer
	std::vector<BYTE> buffer(
		(std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>()
	);

	// Iterate each byte and save it as hex to stream
	std::ostringstream hex_stream;

	for (const auto& byte : buffer) {
		hex_stream << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
	}

	return hex_stream.str();
}

void write_hex_string_to_file(const std::vector<BYTE>& hex_string, const std::string& filename) {

	// Open file as binary
	std::ofstream outputFile(filename, std::ios::binary);

	if (outputFile.is_open()) {
		// Write the vector data to the file
		outputFile.write(reinterpret_cast<const char*>(hex_string.data()), hex_string.size());
		std::cout << "\nFile " << filename << " written successfully!\n\n";
	}
	else {
		std::cerr << "Error opening output file." << std::endl;
	}
	outputFile.close();
}

int main(int argc, char** argv) {

	//// Decypher for crypto algorithm
	//// DXXD Ransomware

	/*
	* Example:
				/ *   N  a t i o n a l   T r e n d s   C S S * /
	   normal	2F2A204E 6174696F6E616C205472656E6473204353532A2F 0D0A0D0A
	   crypted	2F2A204E A300D4385E9C9D92FE77B820750E492AED2A8732 BAB4085C
				^		 ^										  ^
				|		 Crypted with DXXD Ransomware			  File endings
				Non crypted 4 bytes
	*/

	if (argc != 3) {
		std::cout << "Usage: DXXD_Ransomware_Decrypt_Algorithm.exe <path to crypted file> <path to destination file>" << std::endl;
		return 1;
	}

	// Load file as hex string
	std::string file_hex_crypted = read_file_as_hex_string(argv[1]);

	// Convert hex to vector of bytes
	std::vector<BYTE> crypted_bytes = hex_to_bytes(file_hex_crypted);

	// Decrypt DXXD Ransomware
	std::vector<BYTE> decrypted_bytes = decrypt_DXXD_Ransomware(crypted_bytes);

	// Print decrypted bytes hex string, only first x bytes
	std::cout << "\ndecrypted_bytes hex (first 200):\n";
	for (size_t i = 0; i < decrypted_bytes.size() && i < 200; ++i) {
		std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)decrypted_bytes[i];
	}
	std::cout << std::endl;

	// Print decrypted bytes as characters, only first x bytes
	std::cout << "\ndecrypted_bytes text (first 200):\n";
	for (size_t i = 0; i < decrypted_bytes.size() && i < 200; ++i) {
		std::cout << decrypted_bytes[i];
	}
	std::cout << std::endl;

	// Save decrypted buffer to file
	write_hex_string_to_file(decrypted_bytes, argv[2]);

	std::cout << "Press key to exit\n";
	std::cin.get();

	return 0;
}
