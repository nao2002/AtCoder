//abc460b
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_set>
#include <unordered_map>

int main()
{
    int T;
    std::cin >> T;

    for (int t=0; t<T; ++t) {
        int64_t X1,Y1,R1,X2,Y2,R2;

        std::cin >> X1 >> Y1 >> R1 >> X2 >> Y2 >> R2;

        int64_t dist = (X2-X1) * (X2-X1) + (Y2-Y1) * (Y2-Y1);

        if ((R2-R1)*(R2-R1) <= dist && dist <= (R1+R2)*(R1+R2)) {
            std::cout << "Yes" << std::endl;
        } else {
            std::cout << "No" << std::endl;
        }
    }
    return 0;
}