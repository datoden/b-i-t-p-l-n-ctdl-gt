/*
 * DE TAI DT06: HE THONG TU DIEN ANH - VIET & GOI Y TU THONG MINH
 * Cau truc du lieu: Trie / Prefix Tree
 * Ngon ngu: C++17
 *
 * Chuc nang:
 * - Them tu Anh-Viet, tim kiem chinh xac O(L)
 * - Goi y tu theo tien to O(L + K), K la so ky tu ket qua duyet
 * - Liet ke cac tu co chung tien to (DFS)
 * - Xoa tu khoi tu dien
 * - Doc/ghi du lieu CSV don gian
 * - Menu console, kiem tra dau vao co ban
 *
 * Dinh dang file du lieu: english|vietnamese meaning
 * Moi dong mot tu; co the dung dau | trong nghia neu can.
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cctype>
#include <limits>

using namespace std;

struct TrieNode {
    vector<pair<char, unique_ptr<TrieNode>>> children;
    bool isWord = false;
    string meaning;

    TrieNode* getChild(char c) const {
        for (const auto& item : children)
            if (item.first == c) return item.second.get();
        return nullptr;
    }

    TrieNode* ensureChild(char c) {
        for (auto& item : children)
            if (item.first == c) return item.second.get();
        children.push_back({c, make_unique<TrieNode>()});
        return children.back().second.get();
    }

    bool hasChildren() const { return !children.empty(); }

    void eraseChild(char c) {
        children.erase(remove_if(children.begin(), children.end(),
            [c](const pair<char, unique_ptr<TrieNode>>& p) {
                return p.first == c;
            }), children.end());
    }
};

class EnglishVietnameseDictionary {
private:
    TrieNode root;
    size_t wordCount = 0;

    static string normalize(const string& s) {
        string result = s;
        for (char& c : result)
            c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
        return result;
    }

    TrieNode* findNode(const string& word) const {
        TrieNode* current = const_cast<TrieNode*>(&root);
        for (char c : normalize(word)) {
            current = current->getChild(c);
            if (!current) return nullptr;
        }
        return current;
    }

    void collect(TrieNode* node, const string& prefix,
                 vector<pair<string,string>>& result, size_t limit) const {
        if (!node || result.size() >= limit) return;
        if (node->isWord) result.push_back({prefix, node->meaning});
        for (const auto& child : node->children) {
            if (result.size() >= limit) break;
            collect(child.second.get(), prefix + child.first, result, limit);
        }
    }

    bool removeRec(TrieNode* node, const string& word, size_t index) {
        if (index == word.size()) {
            if (!node->isWord) return false;
            node->isWord = false;
            node->meaning.clear();
            --wordCount;
            return !node->hasChildren();
        }
        TrieNode* next = node->getChild(word[index]);
        if (!next) return false;
        bool shouldDelete = removeRec(next, word, index + 1);
        if (shouldDelete) node->eraseChild(word[index]);
        return !node->isWord && !node->hasChildren();
    }

public:
    bool insert(const string& rawWord, const string& meaning) {
        string word = normalize(rawWord);
        if (word.empty() || meaning.empty()) return false;
        TrieNode* current = &root;
        for (char c : word) {
            if (isspace(static_cast<unsigned char>(c))) return false;
            current = current->ensureChild(c);
        }
        if (!current->isWord) ++wordCount;
        current->isWord = true;
        current->meaning = meaning;
        return true;
    }

    bool search(const string& rawWord, string& meaning) const {
        string word = normalize(rawWord);
        TrieNode* node = findNode(word);
        if (!node || !node->isWord) return false;
        meaning = node->meaning;
        return true;
    }

    vector<pair<string,string>> autocomplete(const string& rawPrefix,
                                               size_t limit = 20) const {
        string prefix = normalize(rawPrefix);
        TrieNode* node = findNode(prefix);
        vector<pair<string,string>> result;
        if (node) collect(node, prefix, result, limit);
        return result;
    }

    bool erase(const string& rawWord) {
        string word = normalize(rawWord);
        if (word.empty()) return false;
        TrieNode* node = findNode(word);
        if (!node || !node->isWord) return false;
        removeRec(&root, word, 0);
        return true;
    }

    size_t size() const { return wordCount; }

    bool save(const string& filename) const {
        ofstream out(filename);
        if (!out) return false;
        vector<pair<string,string>> words;
        collect(const_cast<TrieNode*>(&root), "", words,
                numeric_limits<size_t>::max());
        for (const auto& item : words)
            out << item.first << "|" << item.second << "\n";
        return static_cast<bool>(out);
    }

    bool load(const string& filename) {
        ifstream in(filename);
        if (!in) return false;
        string line;
        while (getline(in, line)) {
            if (line.empty()) continue;
            size_t separator = line.find('|');
            if (separator == string::npos) continue;
            string word = line.substr(0, separator);
            string meaning = line.substr(separator + 1);
            insert(word, meaning);
        }
        return true;
    }
};

static void clearLine() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

static string inputLine(const string& prompt) {
    cout << prompt;
    string value;
    getline(cin, value);
    return value;
}

int main() {
    EnglishVietnameseDictionary dictionary;
    dictionary.load("data.txt");

    while (true) {
        cout << "\n========== TU DIEN ANH - VIET (TRIE) ==========\n"
             << "1. Tra cuu tu\n"
             << "2. Them tu moi\n"
             << "3. Goi y tu theo tien to\n"
             << "4. Xoa tu\n"
             << "5. Hien thi so luong tu\n"
             << "6. Luu du lieu\n"
             << "0. Thoat\n";
        string choice = inputLine("Chon chuc nang: ");

        if (choice == "0") {
            dictionary.save("data.txt");
            cout << "Da luu du lieu. Tam biet!\n";
            break;
        } else if (choice == "1") {
            string word = inputLine("Nhap tu tieng Anh: ");
            string meaning;
            if (dictionary.search(word, meaning))
                cout << word << " : " << meaning << "\n";
            else cout << "Khong tim thay tu nay.\n";
        } else if (choice == "2") {
            string word = inputLine("Nhap tu tieng Anh: ");
            string meaning = inputLine("Nhap nghia tieng Viet: ");
            if (dictionary.insert(word, meaning))
                cout << "Them/cap nhat tu thanh cong.\n";
            else cout << "Du lieu khong hop le.\n";
        } else if (choice == "3") {
            string prefix = inputLine("Nhap tien to: ");
            string countText = inputLine("So goi y toi da (Enter = 20): ");
            size_t limit = 20;
            if (!countText.empty()) {
                try { limit = static_cast<size_t>(stoul(countText)); }
                catch (...) { limit = 20; }
                if (limit == 0) limit = 20;
            }
            auto results = dictionary.autocomplete(prefix, limit);
            if (results.empty()) cout << "Khong co tu phu hop.\n";
            else {
                cout << "Cac goi y:\n";
                for (const auto& item : results)
                    cout << " - " << item.first << " : " << item.second << "\n";
            }
        } else if (choice == "4") {
            string word = inputLine("Nhap tu can xoa: ");
            cout << (dictionary.erase(word) ? "Da xoa tu.\n" : "Khong tim thay tu de xoa.\n");
        } else if (choice == "5") {
            cout << "Tong so tu: " << dictionary.size() << "\n";
        } else if (choice == "6") {
            cout << (dictionary.save("data.txt") ? "Luu thanh cong.\n" : "Luu that bai.\n");
        } else {
            cout << "Lua chon khong hop le, vui long thu lai.\n";
        }
    }
    return 0;
}
bool search(const string &word, string &meaning) const {
const TrieNode *node = findNode(word);
if (!node || !node->isEnd) return false;
meaning = node->meaning;
return true;
}
bool erase(const string &word) {
bool deleted = false;
eraseRec(&root, word, 0, deleted);
return deleted;
}
vector<pair<string, string>> suggest(const string &prefix, size_t limit = 10) const {
vector<pair<string, string>> result;
const TrieNode *node = findNode(prefix);
if (!node || limit == 0) return result;
string current = prefix;
collectRec(node, current, result, limit);
return result;
}
vector<pair<string, string>> listAll() const {
vector<pair<string, string>> result;
string current;
28
listRec(&root, current, result);
return result;
}
bool saveToFile(const string &filename) const {
ofstream out(filename);
if (!out) return false;
string current;
saveRec(&root, current, out);
return static_cast<bool>(out);
}
int loadFromFile(const string &filename) {
ifstream in(filename);
if (!in) return 0;
string line;
int count = 0;
while (getline(in, line)) {
size_t sep = line.find('|');
if (sep == string::npos) continue;
string word = line.substr(0, sep);
string meaning = line.substr(sep + 1);
if (meaning.empty() || !normalizeWord(word)) continue;
insert(word, meaning);
++count;
}
return count;
}
};
void showEntries(const vector<pair<string, string>> &items) {
if (items.empty()) {
cout << "Khong co ket qua.\n";
29
return;
}
for (size_t i = 0; i < items.size(); ++i)
cout << i + 1 << ". " << items[i].first
<< " : " << items[i].second << '\n';
}
int readChoice() {
int choice;
if (!(cin >> choice)) {
cin.clear();
cin.ignore(numeric_limits<streamsize>::max(), '\n');
return -1;
}
cin.ignore(numeric_limits<streamsize>::max(), '\n');
return choice;
}
int main() {
Trie dictionary;
const string filename = "tu_vung.txt";
int loaded = dictionary.loadFromFile(filename);
cout << "=== TU DIEN ANH - VIET VA GOI Y TU ===\n";
cout << "So ban ghi doc duoc: " << loaded << "\n";
bool running = true;
while (running) {
cout << "\n---------- MENU ----------\n";
cout << "1. Tra cuu tu\n";
cout << "2. Them / cap nhat tu\n";
cout << "3. Xoa tu\n";
cout << "4. Goi y tu theo tien to\n";
cout << "5. Liet ke toan bo tu dien\n";
cout << "6. Luu du lieu\n";
cout << "0. Luu va thoat\n";
30
cout << "Lua chon: ";
int choice = readChoice();
string word, meaning;
switch (choice) {
case 1: {
cout << "Nhap tu can tra: ";
getline(cin, word);
if (!normalizeWord(word)) {
cout << "Tu khong hop le. Chi dung chu cai A-Z.\n";
break;
}
if (dictionary.search(word, meaning))
cout << word << " nghia la: " << meaning << '\n';
else
cout << "Khong tim thay tu \"" << word << "\".\n";
break;
}
