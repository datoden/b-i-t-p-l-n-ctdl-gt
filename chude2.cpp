#include <array>
#include <cctype>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>
using namespace std;
struct TrieNode {
array<unique_ptr<TrieNode>, 26> child{};
bool isEnd = false;
string meaning;
};
bool normalizeWord(string &word) {
while (!word.empty() &&
isspace(static_cast<unsigned char>(word.front())))
word.erase(word.begin());
while (!word.empty() &&
isspace(static_cast<unsigned char>(word.back())))
word.pop_back();
if (word.empty()) return false;
for (char &ch : word) {
unsigned char c = static_cast<unsigned char>(ch);
if (c > 127 || !isalpha(c)) return false;
ch = static_cast<char>(tolower(c));
if (ch < 'a' || ch > 'z') return false;
}
return true;
}
class Trie {
private:
TrieNode root;
const TrieNode* findNode(const string &word) const {
const TrieNode *cur = &root;
for (char ch : word) {
int idx = ch - 'a';
if (idx < 0 || idx >= 26 || !cur->child[idx])
return nullptr;
cur = cur->child[idx].get();
}
return cur;
}
bool hasChildren(const TrieNode &node) const {
for (const auto &p : node.child)
if (p) return true;
return false;
}
25
bool eraseRec(TrieNode *node, const string &word, size_t pos, bool &deleted) {
if (pos == word.size()) {
if (!node->isEnd) return false;
node->isEnd = false;
node->meaning.clear();
deleted = true;
return !hasChildren(*node);
}
int idx = word[pos] - 'a';
if (idx < 0 || idx >= 26 || !node->child[idx])
return false;
bool removeChild = eraseRec(node->child[idx].get(), word, pos + 1, deleted);
if (removeChild) node->child[idx].reset();
return !node->isEnd && !hasChildren(*node);
}
void collectRec(const TrieNode *node, string &current, vector<pair<string, string>> &out, size_t limit) const {
if (!node || out.size() >= limit) return;
if (node->isEnd)
out.push_back({current, node->meaning});
for (int i = 0; i < 26 && out.size() < limit; ++i) {
if (node->child[i]) {
current.push_back(static_cast<char>('a' + i));
collectRec(node->child[i].get(), current, out, limit);
current.pop_back();
}
26
}
}
void listRec(const TrieNode *node, string &current, vector<pair<string, string>> &out) const {
if (node->isEnd)
out.push_back({current, node->meaning});
for (int i = 0; i < 26; ++i) {
if (node->child[i]) {
current.push_back(static_cast<char>('a' + i));
listRec(node->child[i].get(), current, out);
current.pop_back();
}
}
}
void saveRec(const TrieNode *node, string &current, ostream &out) const {
if (node->isEnd)
out << current << '|' << node->meaning << '\n';
for (int i = 0; i < 26; ++i) {
if (node->child[i]) {
current.push_back(static_cast<char>('a' + i));
saveRec(node->child[i].get(), current, out);
current.pop_back();
}
}
}
public:
bool insert(const string &word, const string &meaning) {
TrieNode *cur = &root;
for (char ch : word) {
int idx = ch - 'a';
27
if (!cur->child[idx])
cur->child[idx] = make_unique<TrieNode>();
cur = cur->child[idx].get();
}
bool isNew = !cur->isEnd;
cur->isEnd = true;
cur->meaning = meaning;
return isNew;
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
