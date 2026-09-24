#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

// --- REVERSE VIGENERE ENGINE ---
class DecryptionEngine {
public:
    vector<unsigned char> transform(const vector<unsigned char>& data, const string& key) {
        vector<unsigned char> result;
        result.reserve(data.size());
        for (size_t i = 0; i < data.size(); i++) {
            unsigned char b = data[i];
            unsigned char k = (unsigned char)key[i % key.length()];
            result.push_back((unsigned char)((b - k + 256) % 256));
        }
        return result;
    }
};
void RestoreFiles(string targetPath, string key) {
    DecryptionEngine engine;
    if (!fs::exists(targetPath)) return;

    vector<fs::path> lockedFiles;
    try {
        for (const auto& entry : fs::recursive_directory_iterator(targetPath)) {
            if (entry.path().extension() == ".locked") {
                lockedFiles.push_back(entry.path());
            }
        }
    } catch (...) { return; }

    for (const auto& filePath : lockedFiles) {
        try {
            // 1. Open the file in binary mode
            ifstream inFile(filePath, ios::binary);
            
            // 2. Read entire file into a vector of unsigned chars
            // This is the critical change to avoid string truncation
            vector<unsigned char> content((istreambuf_iterator<char>(inFile)), (istreambuf_iterator<char>()));
            inFile.close();

            // 3. Decrypt using the vector-based engine
            vector<unsigned char> decrypted = engine.transform(content, key);
            
            // 4. Create the original filename (removing .locked)
            string pathStr = filePath.string();
            string newName = pathStr.substr(0, pathStr.find_last_of("."));
            
            // 5. Write the vector back to disk in binary mode
            ofstream outFile(newName, ios::binary);
            outFile.write(reinterpret_cast<const char*>(decrypted.data()), decrypted.size());
            outFile.close();

            // 6. Delete the encrypted version
            fs::remove(filePath);
            
            cout << "[RESTORED] " << newName << endl; // Optional for your console
        } catch (...) {
            continue; 
        }
    }
}

int main(int argc, char* argv[]) {
    // Correct usage from Kali: .\d.exe --decrypt <path> <raw_key>
    if (argc < 4 || string(argv[1]) != "--decrypt") {
        return 1;
    }

    string path = argv[2];
    string rawKey = argv[3];

    RestoreFiles(path, rawKey);

    return 0;
}