#arc422a
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

T = int(input())

for _ in range(T):
    K = int(input())
    l = len(str(K))
    flag = False
    for i in range(1,101):
        base_num = K * i
        num = base_num
        for j in range(l):
            if num % 100 == 0:
                print(base_num)
                flag = True
                break
            num = num // 10
        if flag:
            break

# a問題メモ
# 最大でも100倍
# //10した結果の%100が0なら0が2連続


# b問題メモ
# 0 1 1 2
# 1 2 1 2 2 1 2 2 1 2 2 2
# 12221222212222122222




# c
# cb b b b
# cb a a b
# cb a a b
# cb b b b