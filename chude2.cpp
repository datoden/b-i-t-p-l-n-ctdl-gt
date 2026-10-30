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