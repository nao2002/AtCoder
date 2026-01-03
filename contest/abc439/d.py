#abc439d
import sys
from collections import defaultdict
from collections import deque
import heapq
import math
# from sortedcontainers import SortedList, SortedDict, SortedSet
try:
    import pypyjit
    pypyjit.set_param('max_unroll_recursion=-1')
except ImportError:
    pass
sys.setrecursionlimit(10**8)
sys.set_int_max_str_digits(0)
def input(): return (sys.stdin.readline()).rstrip()

N = int(input())
A = list(map(int,input().split()))

seven = defaultdict(list)
five = defaultdict(list)
three = defaultdict(list)

for i in range(N):
    if A[i] % 7 == 0:
        seven[A[i]//7].append(i)
    if A[i] % 5 == 0:
        five[A[i]//5].append(i)
    if A[i] % 3 == 0:
        three[A[i]//3].append(i)

view_five = five.keys()

def find_small_idx(left, right, search_target, arr):
    while left < right:
        center = (left+right)//2
        if arr[center] == search_target: #見つかった時
            return center #indexを返す
        elif arr[center] < search_target:
            left = center+1
        else:
            right = center
    else: #見つからなかった時
        return left
    
def find_bigger_idx(left, right, search_target, arr):
    while left <= right:
        center = (left+right)//2
        if arr[center] == search_target: #見つかった時
            return center #indexを返す
        elif arr[center] > search_target:
            right = center - 1
        else:
            left = center + 1
    else: #見つからなかった時
        return right

# print(seven)
# print(five)
# print(three)
ans = 0
for key in view_five:    
    seven_idx = 0
    three_idx = 0
    # print("key", key)
    for i in range(len(five[key])):
        idx = five[key][i]
        if not key in seven or len(seven[key]) == 0 or seven[key][-1] < idx:
            break
        if not key in three or len(three[key]) == 0 or three[key][-1] < idx:
            break
        seven_idx = find_small_idx(seven_idx, len(seven[key])-1, idx+1, seven[key])
        three_idx = find_small_idx(three_idx, len(three[key])-1, idx+1, three[key])
        # print(seven_idx, three_idx)
        # print(idx, seven[key][seven_idx], three[key][three_idx])
        ans += (len(seven[key])-seven_idx) * (len(three[key])-three_idx)

    seven_idx = len(seven[key])-1
    three_idx = len(three[key])-1
    for i in range(len(five[key])-1, -1, -1):
        idx = five[key][i]
        # print("idx", idx)
        if not key in seven or len(seven[key]) == 0 or seven[key][0] > idx:
            break
        if not key in three or len(three[key]) == 0 or three[key][0] > idx:
            break
        seven_idx = find_bigger_idx(0, seven_idx, idx-1, seven[key])
        three_idx = find_bigger_idx(0, three_idx, idx-1, three[key])
        # print(seven_idx, three_idx)
        # print(idx, seven[key][seven_idx], three[key][three_idx])
        ans += (seven_idx+1) * (three_idx+1)

print(ans)