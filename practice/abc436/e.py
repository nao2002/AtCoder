#abc436e
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

class UnionFind:
    def __init__(self, length):
        """
        length: 配列長
        """
        self.length = length
        self.par = [-1]*length
    
    def root(self, x):
        """
        x番目の要素の親を取得
        x: index(0-indexed)

        return: 要素の親index(0-indexed)
        """
        if self.par[x] < 0:
            return x
        else:
            self.par[x] = self.root(self.par[x])
            return self.par[x]
    
    def unite(self, x, y):
        """
        x番目とy番目の所属するグループを合成
        x: index(0-indexed)
        y: index(0-indexed)

        return: 合成後のグループのサイズ
        """
        x,y = self.root(x), self.root(y)
        if x == y:
            return self.par[x]
        if x > y:
            x,y = y,x
        self.par[x] += self.par[y]
        self.par[y] = x
        return -self.par[x]
    
    def size(self, x):
        """
        x番目の所属するグループのサイズを取得
        x: index(0-indexed)

        return: グループのサイズ
        """
        return -self.par[self.root(x)]
    
    def parents(self):
        """
        全ての親のindexを取得 O(N)

        return: 全ての親のindex(0-indexed)の配列
        """
        return [i for i in range(self.length) if self.par[i] < 0]
    
N = int(input())
P = list(map(int,input().split()))

uf = UnionFind(N+1)

for p in P:
    nxt = P[p-1]
    uf.unite(p, nxt)

parents = uf.parents()

ans = 0
for par in parents:
    group_size = uf.size(par)
    if group_size >= 2:
        ans += math.comb(group_size, 2)

print(ans)
