//abc460a
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_set>
#include <unordered_map>

int main()
{
    int N,M;
    std::cin >> N >> M;

    int ans = 0;

    while (M != 0) {
        M = N % M;
        ans += 1;
    }

    std::cout << ans << std::endl;
    return 0;
}