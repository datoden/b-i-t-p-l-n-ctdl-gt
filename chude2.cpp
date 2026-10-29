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
