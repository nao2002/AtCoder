#abc453d
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

def valid(i,j):
     if i >= 0 and i < H and j >= 0 and j < W:
          return True
     else:
          return False

H,W = map(int,input().split())

grid = [list(input()) for _ in range(H)]

start_pos = None
for i in range(H):
    for j in range(W):
        if grid[i][j] == "S":
            start_pos = (i,j)
            grid[i][j] = "."
            break
    if start_pos != None:
            break
    
last_indicies = [None]*(H*W*4)
dir_skips = {"U": 0, "D": H*W, "L": 2*H*W, "R": 3*H*W}
idx = 0
queue = []
if valid(start_pos[0]-1, start_pos[1]) and grid[start_pos[0]-1][start_pos[1]] != "#":
    compressed_idx = dir_skips["U"]+((start_pos[0]-1)*W+start_pos[1])
    last_indicies[compressed_idx] = -1
    queue.append(compressed_idx)
if valid(start_pos[0]+1, start_pos[1]) and grid[start_pos[0]+1][start_pos[1]] != "#":
    compressed_idx = dir_skips["D"]+((start_pos[0]+1)*W+start_pos[1])
    last_indicies[compressed_idx] = -1
    queue.append(compressed_idx)
if valid(start_pos[0], start_pos[1]-1) and grid[start_pos[0]][start_pos[1]-1] != "#":
    compressed_idx = dir_skips["L"]+(start_pos[0]*W+start_pos[1]-1)
    last_indicies[compressed_idx] = -1
    queue.append(compressed_idx)
if valid(start_pos[0], start_pos[1]+1) and grid[start_pos[0]][start_pos[1]+1] != "#":
    compressed_idx = dir_skips["R"]+(start_pos[0]*W+start_pos[1]+1)
    last_indicies[compressed_idx] = -1
    queue.append(compressed_idx)

found = False
dir_map = {"U": (-1,0), "D": (1,0), "L": (0,-1), "R": (0,1)}
dir_keys = ["U", "D", "L", "R"]

while idx < len(queue):
    cur_compressed = queue[idx]
    last_dir = dir_keys[cur_compressed // (H*W)]
    mod_compressed = cur_compressed % (H*W)
    cur_pos_i = mod_compressed // W
    cur_pos_j = mod_compressed % W
    if grid[cur_pos_i][cur_pos_j] == "G":
        found = True
        break
    elif grid[cur_pos_i][cur_pos_j] == "o":
        next_dir = dir_map[last_dir]
        compressed_idx = dir_skips[last_dir]+((cur_pos_i+next_dir[0])*W+(cur_pos_j+next_dir[1]))
        if valid(cur_pos_i+next_dir[0],cur_pos_j+next_dir[1]) and grid[cur_pos_i+next_dir[0]][cur_pos_j+next_dir[1]] != "#" and last_indicies[compressed_idx] is None:
            last_indicies[compressed_idx] = idx
            queue.append(compressed_idx)
    elif grid[cur_pos_i][cur_pos_j] == "." or grid[cur_pos_i][cur_pos_j] == "x":
        for next_dir_key in dir_keys:
            if grid[cur_pos_i][cur_pos_j] == "x" and next_dir_key == last_dir:
                continue
            next_dir = dir_map[next_dir_key]
            compressed_idx = dir_skips[next_dir_key]+((cur_pos_i+next_dir[0])*W+(cur_pos_j+next_dir[1]))
            if valid(cur_pos_i+next_dir[0],cur_pos_j+next_dir[1]) and grid[cur_pos_i+next_dir[0]][cur_pos_j+next_dir[1]] != "#" and last_indicies[compressed_idx] is None:
                last_indicies[compressed_idx] = idx
                queue.append(compressed_idx)
    idx += 1

if not found:
    print("No")
    exit()

ans = []
nxt_idx = idx
while nxt_idx != -1:
    compressed = queue[nxt_idx]
    last_dir = dir_keys[compressed // (H*W)]
    ans.append(last_dir)
    nxt_idx = last_indicies[compressed]
ans.reverse()

print("Yes")
print("".join(ans))