#abc436c
import sys
from collections import defaultdict
from collections import deque
import heapq
# from sortedcontainers import SortedList, SortedDict, SortedSet
try:
    import pypyjit
    pypyjit.set_param('max_unroll_recursion=-1')
except ImportError:
    pass
sys.setrecursionlimit(10**8)
sys.set_int_max_str_digits(0)
def input(): return (sys.stdin.readline()).rstrip()

H,W = map(int,input().split())

m = [["#" for _ in range(W+2)]]
warps = defaultdict(list)

for i in range(H):
    S = list(input())
    for j in range(W):
        if S[j] != "." and S[j] != "#":
            warps[S[j]].append((i+1,j+1))
    m.append(["#"] + S + ["#"])
m.append(["#" for _ in range(W+2)])

# print(m)
idx = 0
queue = [((1,1),0)]
checked = set([(1,1)])
warp_checked = set()

while idx < len(queue):
    pos, cnt = queue[idx]
    # print(idx, pos,cnt , queue)
    idx += 1
    if pos == (H,W):
        print(cnt)
        break
    if m[pos[0]][pos[1]] != "." and m[pos[0]][pos[1]] != "#":
        txt = m[pos[0]][pos[1]]
        if not txt in warp_checked:
            for nxt in warps[txt]:
                if not nxt in checked:
                    queue.append((nxt,cnt+1))
                    checked.add(nxt)
            warp_checked.add(txt)
    right = m[pos[0]+1][pos[1]]
    left = m[pos[0]-1][pos[1]]
    up = m[pos[0]][pos[1]-1]
    down = m[pos[0]][pos[1]+1]
    if right != "#" and not (pos[0]+1,pos[1]) in checked:
        queue.append(((pos[0]+1,pos[1]),cnt+1))
        checked.add((pos[0]+1,pos[1]))
    if left != "#" and not (pos[0]-1,pos[1]) in checked:
        queue.append(((pos[0]-1,pos[1]),cnt+1))
        checked.add((pos[0]-1,pos[1]))
    if up != "#" and not (pos[0],pos[1]-1) in checked:
        queue.append(((pos[0],pos[1]-1),cnt+1))
        checked.add((pos[0],pos[1]-1))
    if down != "#" and not (pos[0],pos[1]+1) in checked:
        queue.append(((pos[0],pos[1]+1),cnt+1))
        checked.add((pos[0],pos[1]+1))
else:
    print(-1)