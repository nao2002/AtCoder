//abc463c
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_set>
#include <unordered_map>

int main()
{
    int64_t N;
    std::cin >> N;

    std::vector<int64_t> times(N+1);
    std::vector<int64_t> heights(N+1);

    times[N] = 1000000000 + 1;
    heights[N] = 0;

    std::vector<std::pair<int64_t,int64_t>> hl(N);

    for (int i=0;i<N;++i) {
        int H,L;
        std::cin >> H >> L;
        hl[i] = std::make_pair(L, H);
    }

    std::sort(hl.begin(), hl.end());

    std::map<int64_t, int64_t> timeToHeight;
    for (int i=N-1;i>=0;--i) {
        heights[i] = std::max(heights[i+1],hl[i].second);
        times[i] = hl[i].first;
        if (timeToHeight.contains(times[i])) {
            timeToHeight[times[i]] = std::max(timeToHeight[times[i]], heights[i]);
        } else {
            timeToHeight[times[i]] = heights[i];
        }
    }

    // std::cout << hl[0].first << std::endl;

    int64_t Q;
    std::cin >> Q;
    for (int i=0; i<Q; ++i) {
        int64_t t;
        std::cin >> t;

        auto ptr = std::upper_bound(times.begin(),times.end(), t);
        int64_t time = *ptr;
        std::cout << timeToHeight[time] << std::endl;
    }

    return 0;
}