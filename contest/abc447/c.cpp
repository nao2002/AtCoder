//abc447c
#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>

int main() {
    std::string S,T;

    std::cin >> S;
    std::cin >> T;

    int ans = 0;
    int sIdx = 0,tIdx = 0;

    bool isError = false;
    while (sIdx < S.size() || tIdx < T.size()) {
        if (sIdx == S.size()) {
            if (T[tIdx] == 'A') {
                ans += 1;
                tIdx += 1;
                continue;
            } else {
                isError = true;
                break;
            }
        }
        if (tIdx == T.size()) {
            if (S[sIdx] == 'A') {
                ans += 1;
                sIdx += 1;
                continue;
            } else {
                isError = true;
                break;
            }
        }

        if (S[sIdx] == T[tIdx]) {
            sIdx += 1;
            tIdx += 1;
            continue;
        }

        if (T[tIdx] == 'A') {
            ans += 1;
            tIdx += 1;
        }
        else if (S[sIdx] == 'A') {
            ans += 1;
            sIdx += 1;
        } else {
            isError = true;
            break;
        }
    }

    if (isError) {
        std::cout << -1 << std::endl;
    } else {
        std::cout << ans << std::endl;
    }

    return 0;
}