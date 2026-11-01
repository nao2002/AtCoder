#abc482e
# Source: https://github.com/nao2002/AtCoder
import sys
from collections import defaultdict
from collections import deque
import heapq
import math
from sortedcontainers import SortedList, SortedDict, SortedSet
try:
    import pypyjit
    pypyjit.set_param('max_unroll_recursion=-1')
except ImportError:
    pass
sys.setrecursionlimit(10**6)
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

def binary_search(left, right, check):
    while left < right:
        mid = (left + right) // 2

        if check(mid):
            right = mid
        else:
            left = mid + 1

    return left

N = int(input())
tree = defaultdict(list)
for i in range(N-1):
    u,v = map(int,input().split())
    tree[u].append(v)
    tree[v].append(u)

farthests = set()

first_leaf = -1
for i in range(1,N+1):
    if len(tree[i]) == 1:
        first_leaf = i
        break

queue = [(first_leaf,0)]
idx = 0
dists = [-1]*(N+1)
dists[first_leaf] = 0
mx_dist = 0
second_leaf = -1
while idx < len(queue):
    cur, dist = queue[idx]
    for nxt in tree[cur]:
        if dists[nxt] == -1:
            queue.append((nxt, dist+1))
            dists[nxt] = dist+1
    if dist > mx_dist:
        mx_dist = dist
        second_leaf = cur
        farthests.clear()
    if dist == mx_dist:
        farthests.add(cur)
    idx += 1

uf = UnionFind(N+1)
queue = [(second_leaf,0,second_leaf)]
idx = 0
visited = [False]*(N+1)
visited[second_leaf] = True
while idx < len(queue):
    cur, dist, par = queue[idx]
    uf.unite(cur, par)
    for nxt in tree[cur]:
        if not visited[nxt]:
            if dist > dists[cur] and par == second_leaf:
                queue.append((nxt, dist+1, nxt))
            else:
                queue.append((nxt, dist+1, par))
            visited[nxt] = True
    if dist == mx_dist and not cur in farthests:
        farthests.add(cur)
    if dist != mx_dist and cur in farthests and par != second_leaf:
        farthests.remove(cur)
    idx += 1

roots = uf.get_all_roots()
C = list(map(int,input().split()))

color_all = defaultdict(SortedList)
color_group = dict()
for leaf in farthests:
    color_all[C[leaf-1]].add(leaf)
    if not uf.find_root(leaf) in color_group:
        color_group[uf.find_root(leaf)] = defaultdict(SortedList)
    color_group[uf.find_root(leaf)][C[leaf-1]].add(leaf)

ans = 0
for group_id in color_group.keys():
    group = color_group[group_id]
    for color in group.keys():
        for leaf in group[color]:
            my_pos = binary_search(0, len(color_all[color]), lambda x: color_all[color][x] >= leaf)
            all_cnt = len(color_all[color]) - my_pos
            group_pos = binary_search(0, len(group[color]), lambda x: group[color][x] >= leaf)
            group_cnt = len(group[color]) - group_pos
            ans += all_cnt - group_cnt

print(ans)