//abc447d
#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>

int main() {
    std::string S;
    std::cin >> S;

    int aIdx=0,bIdx=0,cIdx = 0;

    int sLength = S.size();
    bool aFound = false, bFound = false, cFound = false;

    int ans = 0;
    while (aIdx < sLength && bIdx < sLength && cIdx < sLength) {
        if (!aFound) {
            if (S[aIdx] == 'A') {
                aFound = true;
                if (bIdx < aIdx) {
                    bIdx = aIdx;
                }
            } else {
                aIdx += 1;
            }
        } else if (!bFound) {
            if (S[bIdx] == 'B') {
                bFound = true;
            } else {
                bIdx += 1;
                if (cIdx < bIdx) {
                    cIdx = bIdx;
                }
            }
        } else if (!cFound) {
            if (S[cIdx] == 'C') {
                cFound = true;
            } else {
                cIdx += 1;
            }
        } else {
            // aもbもcもあった時
            ans += 1;
            aIdx += 1;
            aFound = false;
            bIdx += 1;
            bFound = false;
            cIdx += 1;
            cFound = false;
        }
    }

    std::cout << ans << std::endl;

    return 0;
}