//abc447b
#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>

int main() {
    std::string S;

    std::cin >> S;

    std::string ans;

    std::unordered_set<char> maxChars = {};
    int maxCharCount = 0;
    std::unordered_map<char, int> charMap;
    for (int i=0;i<S.size();++i) {
        charMap[S[i]] = charMap[S[i]] + 1;
        if (charMap[S[i]] > maxCharCount) {
            maxCharCount = charMap[S[i]];
            maxChars = {S[i]};
        } else if (charMap[S[i]] == maxCharCount) {
            maxChars.insert(S[i]);
        }
    }

    for (int i=0;i<S.size();++i) {
        if (!maxChars.contains(S[i])) {
            ans += S[i];
        }
    }
    std::cout << ans << std::endl;
}