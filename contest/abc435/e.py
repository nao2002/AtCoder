#abc435e
import sys
from collections import defaultdict
from collections import deque
import heapq
from sortedcontainers import SortedList, SortedDict, SortedSet
try:
    import pypyjit
    pypyjit.set_param('max_unroll_recursion=-1')
except ImportError:
    pass
sys.setrecursionlimit(10**8)
sys.set_int_max_str_digits(0)
def input(): return (sys.stdin.readline()).rstrip()

N,Q = map(int,input().split())
filled = SortedList()

def search_start(L):
    left = 0
    right = len(filled)-1
    search_target = L
    while left < right:
        center = (left+right)//2
        if filled[center][0] == search_target: #見つかった時
            return center #indexを返す
        elif filled[center][0] < search_target:
            left = center+1
        else:
            right = center
    else: #見つからなかった時
        return left
        

ans = N
for i in range(Q):
    L,R = map(int,input().split())
    ignore = 0
    if len(filled) != 0:
        check_idx = search_start(L)
        if L >= filled[check_idx][0] and R <= filled[check_idx][1]:
            print(ans)
            continue
        # print("check_idx: ", check_idx)
        while check_idx < len(filled) and filled[check_idx][0] > L:
            if R < filled[check_idx][0]:
                break
            if R >= filled[check_idx][1]:
                ignore += filled[check_idx][1] - filled[check_idx][0] + 1
                filled.discard(filled[check_idx])
            else:
                break
        # print("ignore 1: ", ignore)
        if check_idx < len(filled):
            if filled[check_idx][0] > L and R >= filled[check_idx][0]:
                ignore += filled[check_idx][1] - filled[check_idx][0] + 1
                R = filled[check_idx][1]
                filled.discard(filled[check_idx])
        # print("ignore 2: ", ignore)
        if check_idx - 1 < len(filled) and check_idx - 1 >= 0:
            # print(check_idx-1, len(filled), filled)
            if L <= filled[check_idx-1][1]:
                ignore += filled[check_idx-1][1] - filled[check_idx-1][0] + 1
                L = filled[check_idx-1][0]
                filled.discard(filled[check_idx-1])
        # print("ignore 3: ", ignore)
    filled.add((L,R))
    ans -= (R - L) - ignore + 1
    print(ans)
    # print("filled: ", filled)
