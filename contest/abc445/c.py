#abc445c
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
    """
    Union-Find木
    """

    NO_PARENT = -1

    def __init__(self, num_nodes):
        """
        num_nodes: 要素数
        """
        self._num_nodes = num_nodes
        # 各要素の親ノードのindexを格納する配列
        self._parent = [UnionFind.NO_PARENT]*num_nodes
        # 各グループのサイズを格納する配列
        self._group_size = [1]*num_nodes

    @property
    def num_nodes(self):
        """
        要素数を取得 (read-only)
        uf.num_nodesのようにアクセス可能

        return: 要素数
        """
        return self._num_nodes
    
    def find_root(self, node_idx):
        """
        node_idx番目の要素の根を取得
        node_idx: index(0-indexed)

        return: 要素の根のindex(0-indexed)
        """
        if self._parent[node_idx] == UnionFind.NO_PARENT:
            return node_idx
        else:
            self._parent[node_idx] = self.find_root(self._parent[node_idx])
            return self._parent[node_idx]
    
    def unite(self, node_u, node_v):
        """
        node_u番目とnode_v番目の所属するグループを合成
        node_u: index(0-indexed)
        node_v: index(0-indexed)

        return: 合成後のグループのサイズ
        """
        root_x,root_y = self.find_root(node_u), self.find_root(node_v)
        if root_x == root_y:
            return self._group_size[root_x]
        
        # グループのサイズが大きい方を親にする
        if self._group_size[root_x] < self._group_size[root_y]:
            root_x,root_y = root_y,root_x
        
        self._group_size[root_x] += self._group_size[root_y]
        self._group_size[root_y] = 0
        self._parent[root_y] = root_x

        return self._group_size[root_x]
    
    def unite_u_to_v(self, node_u, node_v):
        """
        node_uをnode_vに合成 うそ　時間内から一旦放置

        return: 合成後のグループのサイズ
        """
        root_x,root_y = self.find_root(node_u), self.find_root(node_v)
        if root_x == root_y:
            return self._group_size[root_x]
                
        self._group_size[root_x] += self._group_size[root_y]
        self._group_size[root_y] = 0
        self._parent[root_y] = root_x

        return self._group_size[root_x]
    
    def get_group_size(self, node_idx):
        """
        node_idx番目の所属するグループのサイズを取得
        node_idx: index(0-indexed)

        return: グループのサイズ
        """
        return self._group_size[self.find_root(node_idx)]
    
    def get_all_roots(self):
        """
        全ての根のindexを取得 O(N)

        return: 全ての根のindex(0-indexed)の配列
        """
        return [i for i in range(self._num_nodes) if self._parent[i] == UnionFind.NO_PARENT]


N = int(input())

A = list(map(int,input().split()))

uf = UnionFind(N+1)

for i in range(N):
    uf.unite_u_to_v(A[i],(i+1))

ans = []
for i in range(1,N+1):
    ans.append(uf.find_root(i))


print(*ans)