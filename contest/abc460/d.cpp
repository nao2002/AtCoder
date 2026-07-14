//abc460d
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_set>
#include <unordered_map>

int main()
{
    int H,W;
    std::vector<std::pair<int,int>> queue;

    std::cin >> H >> W;

    std::vector<std::vector<char>> grid(H);
    std::vector<std::vector<char>> ans(H);
    std::vector<bool> checked(H*W);

    for (int i=0;i<H;++i) {
        std::string s;
        std::cin >> s;
        for (int j=0;j<W;++j) {
            char c = s[j];
            grid[i].push_back(c);
            ans[i].push_back(c);
        }
    }

    // if (queue.size() == 0) {
    //     for (int i=0; i<H; ++i) {
    //         for (int j=0; j<W; ++j) {
    //             std::cout << grid[i][j];
    //         }
    //         std::cout << std::endl;
    //     }
    //     return 0;
    // }

    std::vector<int> dy{-1,0,1};
    std::vector<int> dx{-1,0,1};
    for (int i=0;i<H;++i) {
        for (int j=0;j<W;++j) {
            if (grid[i][j] == '.') {
                continue;
            }

            bool whiteFound = false; 
            for (const auto& y: dy) {
                if (i+y >= 0 && i+y < H) {
                    for (const auto& x: dx) {
                        if (j+x >= 0 && j+x < W) {
                            if (grid[i+y][j+x] == '.') {
                                whiteFound = true;
                                break;
                            }
                        }
                    }
                    if (whiteFound) break;
                }
            }

            if (whiteFound) {
                queue.push_back(std::make_pair((i*W)+j,0));
                checked[(i*W)+j] = true;
            } else {
                ans[i][j] = '.';
            }
        }
    }

    int idx = 0;
    while (idx < queue.size()) {
        int pos,cnt;
        pos = queue[idx].first;
        cnt = queue[idx].second;
        idx += 1;
        int h = pos / W;
        int w = pos % W;

        if (cnt % 2 == 0) {
            ans[h][w] = '#';
        } else {
            ans[h][w] = '.';
        }

        for (const auto& y: dy) {
            if (h+y >= 0 && h+y < H) {
                for (const auto& x: dx) {
                    if (w+x >= 0 && w+x < W) {
                        if (!checked[(h+y)*W+(w+x)]) {
                            queue.push_back(std::make_pair((h+y)*W+(w+x),cnt+1));
                            checked[(h+y)*W+(w+x)] = true;
                        }
                    }
                }
            }
        }
    }

    for (int i=0; i<H; ++i) {
        for (int j=0; j<W; ++j) {
            std::cout << ans[i][j];
        }
        std::cout << std::endl;
    }

    return 0;
}