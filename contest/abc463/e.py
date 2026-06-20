#abc463e
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
sys.setrecursionlimit(10**8)
sys.set_int_max_str_digits(0)

def input(): return (sys.stdin.readline()).rstrip()

N,M,Y = map(int,input().split())

graph = defaultdict(list)

for i in range(M):
    u,v,t = map(int,input().split())
    graph[u].append((v,t))
    graph[v].append((u,t))

X = list(map(int,input().split()))
warpgate_costs = SortedList()
for i in range(len(X)):
    warpgate_costs.add((X[i],i))

queue = SortedList()
current_mins = [10**10]*(N+1)
current_mins[1] = 0
first_regist = [True]*(N+1)
first_regist[1] = False
visited = [False]*(N+1)
visited[1] = True
visited_count = 1
# 道路による更新
for nxt,cost in graph[1]:
    if first_regist[nxt]:
        first_regist[nxt] = False
        current_mins[nxt] = cost
        queue.add((cost,nxt))
    else:
        if cost < current_mins[nxt]:
            queue.remove((current_mins[nxt], nxt))
            current_mins[nxt] = cost
            queue.add((cost,nxt))

# ワープゲートによる更新
min_warp_cost = X[0]
warpgate_costs.remove((X[0],0))
warp_cost = warpgate_costs[0][0] + min_warp_cost + Y
if len(queue) != 0:
    cost,nxt = queue[0]
    if warp_cost < cost:
        nxt = warpgate_costs[0][1]+1
        if first_regist[nxt]:
            first_regist[nxt] = False
            current_mins[nxt] = warp_cost
            queue.add((warp_cost,nxt))
        else:
            queue.remove((current_mins[nxt], nxt))
            current_mins[nxt] = warp_cost
            queue.add((warp_cost,nxt))
else:
    nxt = warpgate_costs[0][1]+1
    if first_regist[nxt]:
        first_regist[nxt] = False
        current_mins[nxt] = warp_cost
        queue.add((warp_cost,nxt))
    else:
        queue.remove((current_mins[nxt], nxt))
        current_mins[nxt] = warp_cost
        queue.add((warp_cost,nxt))

# print(graph)

while queue:
    cur_cost,cur_pos = queue[0]
    # print(f"{cur_pos}に{cur_cost}で到着しました")
    queue.remove((cur_cost,cur_pos))
    visited[cur_pos] = True
    visited_count += 1
    min_warp_cost = min(min_warp_cost,X[cur_pos-1]+cur_cost)
    warpgate_costs.remove((X[cur_pos-1],cur_pos-1))
    for nxt,cost in graph[cur_pos]:
        cost += cur_cost
        if visited[nxt]:
            continue
        if first_regist[nxt]:
            first_regist[nxt] = False
            current_mins[nxt] = cost
            queue.add((cost,nxt))
        else:
            if cost < current_mins[nxt]:
                queue.remove((current_mins[nxt], nxt))
                current_mins[nxt] = cost
                queue.add((cost,nxt))
    
    if len(queue) == 0 and visited_count == N:
        break

    if len(queue) != 0:
        min_cost,tmp_nxt = queue[0]
        warp_cost = warpgate_costs[0][0] + min_warp_cost + Y
        if warp_cost < min_cost:
            warp_nxt = warpgate_costs[0][1]+1
            if first_regist[warp_nxt]:
                first_regist[warp_nxt] = False
                current_mins[warp_nxt] = warp_cost
                queue.add((warp_cost,warp_nxt))
            else:
                queue.remove((current_mins[warp_nxt], warp_nxt))
                current_mins[warp_nxt] = warp_cost
                queue.add((warp_cost,warp_nxt))
    else:
        warp_cost = warpgate_costs[0][0] + min_warp_cost + Y
        warp_nxt = warpgate_costs[0][1]+1
        if first_regist[warp_nxt]:
            first_regist[warp_nxt] = False
            current_mins[warp_nxt] = warp_cost
            queue.add((warp_cost,warp_nxt))
        else:
            queue.remove((current_mins[warp_nxt], warp_nxt))
            current_mins[warp_nxt] = warp_cost
            queue.add((warp_cost,warp_nxt))

ans = []
for i in range(len(current_mins)):
    if i <= 1:
        continue
    ans.append(current_mins[i])
print(*ans)