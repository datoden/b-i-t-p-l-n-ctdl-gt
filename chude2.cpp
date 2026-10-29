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
