#abc479e
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

N = int(input())

tree = defaultdict(list)
#parent, childCount
parents = [-1]*(N+1)
childCounts = [-1]*(N+1)
node_infos = []
lines = []

diffs = [0]*(N+1)

for i in range(N-1):
    A,B = map(int,input().split())
    tree[A].append(B)
    tree[B].append(A)
    lines.append((A,B))

root = A
visited = [False]*(N+1)

def dfs(cur):
    visited[cur] = True
    childCnt = 1
    for nxt in tree[cur]:
        if not visited[nxt]:
            parents[nxt] = cur
            childCnt += dfs(nxt)
    childCounts[cur] = childCnt 
    return childCnt

dfs(root)

score_sum = 0
for i in range(N-1):
    a,b = lines[i]
    score_sum += i+1
    # a -> bの順
    if parents[b] == a:
        if childCounts[b] > childCounts[root] - childCounts[b]:
            diffs[root] -= (i+1)
            diffs[b] += (i+1)
        else:
            # diffs[root] += (i+1)
            diffs[b] -= (i+1)
    # b -> aの順
    else:
        if childCounts[a] > childCounts[root] - childCounts[a]:
            diffs[root] -= (i+1)
            diffs[a] += (i+1)
        else:
            # diffs[root] += (i+1)
            diffs[a] -= (i+1)

visited = [False]*(N+1)
ans = [-1]*N
def score_dfs(cur, score):
    visited[cur] = True
    score += diffs[cur]
    ans[cur-1] = score
    for nxt in tree[cur]:
        if not visited[nxt]:
            score_dfs(nxt, score)

score_dfs(root, score_sum)
# print(score_sum, diffs)
print(*ans)