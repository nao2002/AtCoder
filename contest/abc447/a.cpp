//abc447a
#include <iostream>

int main() {
    int N, M;

    std::cin >> N >> M;

    if (M > (N / 2) + (N % 2)) {
        std::cout << "No" << std::endl;
    } else {
        std::cout << "Yes" << std::endl;
    }
}