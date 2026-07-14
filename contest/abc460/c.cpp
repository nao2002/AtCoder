//abc460c
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <set>

int main()
{
    int N,M;
    std::cin >> N >> M;
    std::vector<int> A(N);
    std::vector<int> B(M);

    for (int i=0; i<N;i++) {
        std::cin >> A[i];
    }
    for (int i=0; i<M;i++) {
        std::cin >> B[i];
    }
    std::sort(A.begin(),A.end());
    std::sort(B.begin(),B.end());

    int idxA,idxB;
    idxA = N-1;
    idxB = M-1;

    int ans = 0;
    while (idxA >= 0 && idxB >= 0) {
        if (B[idxB] <= A[idxA]*2) {
            ans += 1;
            --idxA;
            --idxB;
        } else {
            --idxB;
        }
    }

    std::cout << ans << std::endl;

    return 0;
}