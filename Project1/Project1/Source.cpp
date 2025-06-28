/*
    Author: Morgan Coker
    CoAuthor: Suni Lopez
    Date: June 23, 2025
    Assignment: Assignment Five – Comprehensive Challenge
    Description: This program allows the user to spellcheck, encrypt, or decrypt text files. It uses a fixed dictionary file to identify unknown words in a selected text. 
                 For encryption and decryption, it applies a custom monoalphabetic cipher based on the keyword "FEATHER." 
                 Users select input files and name output files through a file browser. The program runs in a loop until the user chooses to exit.
    Sources used: WChargin's word list: gist.github.com/WChargin/8927565
                  YouTube tutorials
                  W3Schools (web)
                  Big C++: Late Objects by Cay Horstmann
       
*/

#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <cctype>
#include <windows.h>
#include <commdlg.h>
#include <limits>

using namespace std;

// Opens a file dialog box for the user to select a text file
string browseFile() {
    wchar_t filename[MAX_PATH] = L"";
    OPENFILENAMEW ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = L"Text Files\0*.txt\0All Files\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    
    // Show dialog and return selected file path
    if (GetOpenFileNameW(&ofn)) {
        char buffer[MAX_PATH];
        size_t converted;
        wcstombs_s(&converted, buffer, MAX_PATH, filename, MAX_PATH - 1);
        buffer[MAX_PATH - 1] = '\0';
        return string(buffer);
    }
    return "";
}

// Generates a cipher alphabet based on a keyword, followed by unused letters in reverse order
string generateCipherAlphabet(const string& keyword) {
    string result;
    unordered_set<char> seen;
    
    // Add unique letters from keyword
    for (char c : keyword) {
        c = toupper(c);
        if (isalpha(c) && seen.insert(c).second) result += c;
    }
    // Add remaining unused letters in reverse alphabetical order
    for (char c = 'Z'; c >= 'A'; --c) {
        if (seen.find(c) == seen.end()) result += c;
    }
    return result;
}

// Builds a mapping of characters for encryption or decryption
unordered_map<char, char> buildCipherMap(const string& alphabet, bool decrypt = false) {
    string base = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    unordered_map<char, char> map;
    
    // Map letters based on whether we're encrypting or decrypting
    for (int i = 0; i < 26; ++i) {
        if (decrypt)
            map[alphabet[i]] = base[i]; // Reverse napping
        else
            map[base[i]] = alphabet[i]; // Forward mapping
    }
    return map;
}

// Processes file encryption or decryption using a cipher map
void processCipher(const string& inputPath, const string& outputPath, bool decrypt, const string& keyword) {
    ifstream in(inputPath);
    ofstream out(outputPath);
    
    // Validate file access
    if (!in || !out) {
        cerr << "File error!" << endl;
        return;
    }

    // Build cipher map and transform file content
    auto cipherMap = buildCipherMap(generateCipherAlphabet(keyword), decrypt);
    char ch;
    
    
    while (in.get(ch)) {
        if (isalpha(ch)) {
            bool lower = islower(ch);
            char enc = cipherMap[toupper(ch)];
            out.put(lower ? tolower(enc) : enc);
        }
        else {
            out.put(ch);
        }
    }
    cout << (decrypt ? "Decryption" : "Encryption") << " complete: " << outputPath << endl;
}

// BREAK BREAK BREAK BREAK BREAK BREAK - ***************PROGRAM CHANGES FROM ENCRYPTION TO SPELLCHECK HERE*************** - BREAK BREAK BREAK BREAK BREAK BREAK

// Runs a spellcheck by comparing each word in a file to a dictionary
void runSpellCheck(const string& filePath, const string& dictPath) {
    unordered_set<string> dictionary;
    ifstream dict(dictPath);
    
    // Load dictionary words into a set
    if (!dict) {
        cerr << "Cannot open dictionary." << endl;
        return;
    }
    string word;
    while (dict >> word) {
        for (char& c : word) c = tolower(c);
        dictionary.insert(word);
    }
    
    // Open file to be checked
    ifstream file(filePath);
    if (!file) {
        cerr << "Cannot open input file." << endl;
        return;
    }
    bool error = false;

    // Check each word against the dictionary
    while (file >> word) {
        string clean;
        for (char c : word) if (isalpha(c)) clean += tolower(c);
        if (!clean.empty() && dictionary.find(clean) == dictionary.end()) {
            cout << "Unknown word: " << clean << endl;
            error = true;
        }
    }
    if (!error) cout << "No spelling errors found.\n";
}

// BREAK BREAK BREAK BREAK BREAK BREAK - ***************PROGRAM CHANGES FROM SPELLCHECK TO MAIN MENU HERE*************** - BREAK BREAK BREAK BREAK BREAK BREAK

// Main menu and program loop
int main() {
    while (true) {
        cout << "\nWordTools Menu:\n";
        cout << "1. Spellcheck a file\n";
        cout << "2. Encrypt a file\n";
        cout << "3. Decrypt a file\n";
        cout << "4. Exit\n";
        cout << "Select: ";

        int choice;
        cin >> choice;
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        // Spellcheck options
        if (choice == 1) {
            cout << "Select the file to spellcheck...\n";
            string input = browseFile();
            cout << "Select the dictionary file...\n";
            string dict = "C:\\Users\\ashto\\source\\repos\\Project1\\Project1\\x64\\Debug\\words.txt";
            if (!input.empty() && !dict.empty()) runSpellCheck(input, dict);
        }

        // Encryption or decryption options
        else if (choice == 2 || choice == 3) {
            cout << "Select the file to process...\n";
            string input = browseFile();
            if (input.empty()) continue;
            string output;
            cout << "Output file name: ";
            getline(cin, output);
            processCipher(input, output, choice == 3, "FEATHER");
        }
        else if (choice == 4) {
            cout << "Exiting WordTools.\n";
            break;
        }

        // Invalid input
        else {
            cout << "Invalid selection. Try again.\n";
        }
    }
    return 0;
}
