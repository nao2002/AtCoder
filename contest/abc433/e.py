#abc433e
import sys
from collections import defaultdict
from collections import deque
import heapq
import pypyjit
# from sortedcontainers import SortedList, SortedDict, SortedSet
pypyjit.set_param('max_unroll_recursion=-1')
sys.setrecursionlimit(10**8)
sys.set_int_max_str_digits(0)

def input(): return (sys.stdin.readline()).rstrip()

T = int(input())

def fetch_num(place_map, idx):
    if idx == 0:
        if place_map[0] != 1:
            return -1
        place_map[0] = -1
        return 1
    if idx < 0:
        return -1
    if place_map[idx] == idx+1:
        place_map[idx] = place_map[idx-1]
        return idx+1
    else:
        target = fetch_num(place_map, place_map[idx-1]-1)
        place_map[idx] = target - 1
        return target

for _ in range(T):
    N,M = map(int,input().split())
    X = list(map(int,input().split()))
    Y = list(map(int,input().split()))

    x_with_idx = list((X[i],i) for i in range(N))
    y_with_idx = list((Y[i],i) for i in range(M))
    x_with_idx.sort()
    y_with_idx.sort()
    x_idx = 0
    y_idx = 0
    place_map = list(i+1 for i in range(N*M))

    board = [[-1 for _ in range(M)] for _ in range(N)]
    broke = False
    for x, i in x_with_idx:
        for y, j in y_with_idx:
            minimum = min(X[i], Y[j])
            num = fetch_num(place_map, minimum-1)
            if num <= 0:
                broke = True
                break
            board[i][j] = num
        if broke:
            break
    if broke:
        print("No")
    else:
        print("Yes")
        for i in range(N):
            print(*board[i])
