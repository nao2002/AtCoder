#ahc072a
import sys
from collections import defaultdict
from collections import deque
import heapq
import math
from sortedcontainers import SortedList, SortedDict, SortedSet
import time
try:
    import pypyjit
    pypyjit.set_param('max_unroll_recursion=-1')
except ImportError:
    pass
sys.setrecursionlimit(10**6)
sys.set_int_max_str_digits(0)

def input(): return (sys.stdin.readline()).rstrip()

start_time = time.time()

MAX_TIME = 1.8
MAX_STEPS = 100000

N,K = map(int,input().split())

grid = [list(input()) for _ in range(N)]

distances = {}

hq = []
# 各タワーの上限は8匹
slime_stack = [[[] for _ in range(N)] for _ in range(N)]
slime_counts = [[0 for _ in range(N)] for _ in range(N)]

dd = [(-1, 0), (1, 0), (0, -1), (0, 1)]
steps = 0

def check_limit():
    if time.time() - start_time >= MAX_TIME:
        exit()
    if steps >= MAX_STEPS:
        exit()

def is_inside(i, j):
    if i >= 0 and i < N and j >= 0 and j < N:
        return True
    return False

def nest_bfs(char, start_i, start_j):
    lower_char = char.lower()
    distances[lower_char] = [[-1 for _ in range(N)] for _ in range(N)]
    dist_char = distances[lower_char]
    dist_char[start_i][start_j] = 0
    queue = []
    for di, dj in dd:
        if is_inside(start_i+di, start_j+dj) and grid[start_i+di][start_j+dj] != "#":
            dist_char[start_i+di][start_j+dj] = 1
            queue.append((start_i+di, start_j+dj))

    idx = 0
    while idx < len(queue):
        i, j = queue[idx]
        cur_dist = dist_char[i][j]
        cur_char = grid[i][j]

        if cur_char == lower_char:
            heapq.heappush(hq, [-1*cur_dist, i, j])
            slime_stack[i][j].append([cur_char, 1])
            slime_counts[i][j] = 1

        for di, dj in dd:
            if is_inside(i+di, j+dj) and dist_char[i+di][j+dj] == -1 and grid[i+di][j+dj] != "#":
                dist_char[i+di][j+dj] = cur_dist+1
                queue.append((i+di, j+dj))
        idx += 1

# 各nestからのその色のスライムの距離計算
for i in range(N):
    for j in range(N):
        if grid[i][j] != "." and grid[i][j] != "#" and grid[i][j].isupper():
            nest_bfs(grid[i][j], i, j)

# 出力形式: i j k d l
# ij: 0-indexedな座標 k: 残す数 d: 方向UDLR l: 飛距離 (0~k+1)
# ans = []

# RLUDを返す 同一でなく一直線上のどこかである前提
def get_dir(bef_i, bef_j, aft_i, aft_j):
    if bef_i == aft_i:
        if bef_j < aft_j:
            return "R"
        else:
            return "L"
    else:
        if bef_i < aft_i:
            return "D"
        else:
            return "U"

def get_simple_distance(bef_i, bef_j, aft_i, aft_j):
    di = abs(aft_i-bef_i)
    dj = abs(aft_j-bef_j)
    return di+dj

def apply_route(start_i, start_j, target_i, target_j, route_map, moving_slime_count):
    global steps
    routes = []
    cur_i = target_i
    cur_j = target_j
    while cur_i != start_i or cur_j != start_j:
        check_limit()
        nxt_i, nxt_j = route_map[cur_i][cur_j]
        dir = get_dir(nxt_i, nxt_j, cur_i, cur_j)
        routes.append([nxt_i, nxt_j, -1, dir, get_simple_distance(nxt_i, nxt_j, cur_i, cur_j)])
        cur_i, cur_j = route_map[cur_i][cur_j]

    for r in range(len(routes)-1, -1, -1):
        if r == len(routes)-1:
            routes[r][2] = slime_counts[start_i][start_j]-moving_slime_count
        else:
            i = routes[r][0]
            j = routes[r][1]
            routes[r][2] = slime_counts[i][j]
        if steps < MAX_STEPS:
            check_limit()
            print(*routes[r])
            steps += 1

def separate_tower(tower_i, tower_j):
    global steps
    if slime_counts[tower_i][tower_j] == 0:
        return None
    top = slime_stack[tower_i][tower_j][-1]
    top_color = top[0]
    top_count = top[1]
    remain_count = slime_counts[tower_i][tower_j] - top_count
    upper_top_color = top_color.upper()

    if remain_count != 0:
        move_count = top_count
    else:
        # 単色の塊なら半分にする
        move_count = top_count // 2
        if top_count % 2 == 1:
            move_count += 1
        remain_count = top_count - move_count
        if remain_count == 0:
            # 順序管理ができなくなるため行わない
            return None

    target = None
    base_dist = distances[top_color][tower_i][tower_j]
    best_dist = base_dist

    for base_di, base_dj in dd:
        check_limit()
        for length in range(1, remain_count+2):
            di = base_di * length
            dj = base_dj * length
            if is_inside(tower_i+di, tower_j+dj):
                if grid[tower_i+di][tower_j+dj] == "#":
                    break
                dist = distances[top_color][tower_i+di][tower_j+dj]
                if dist < best_dist and slime_counts[tower_i+di][tower_j+dj]+move_count <= 8:
                    target = (tower_i+di,tower_j+dj)
                    best_dist = dist

    new_pos = None

    if target != None:
        if steps < MAX_STEPS:
            print(*[tower_i, tower_j, remain_count, get_dir(tower_i, tower_j, target[0], target[1]), get_simple_distance(tower_i, tower_j, target[0], target[1])])
            steps += 1

        # 元タワー位置から減らす
        slime_stack[tower_i][tower_j][-1][1] -= move_count
        if slime_stack[tower_i][tower_j][-1][1] <= 0:
            slime_stack[tower_i][tower_j].pop()
        slime_counts[tower_i][tower_j] -= move_count

        # 移動先にスライムがいてtopが同色なら加算
        if slime_counts[target[0]][target[1]] != 0 and slime_stack[target[0]][target[1]][-1][0] == top_color:
            slime_stack[target[0]][target[1]][-1][1] += move_count
        else:
            if grid[target[0]][target[1]] != upper_top_color:
                if slime_counts[target[0]][target[1]] == 0:
                    # 移動先にスライムがいなかった場合
                    new_pos = (target[0], target[1])
                slime_stack[target[0]][target[1]].append([top_color, move_count])
        # ゴールに行った場合は加算しない
        if grid[target[0]][target[1]] != upper_top_color:
            slime_counts[target[0]][target[1]] += move_count

    if new_pos != None:
        return [-1*best_dist, new_pos[0], new_pos[1]]
    else:
        return None

def route_bfs(start_i, start_j):
    global steps
    lower_char = slime_stack[start_i][start_j][-1][0]
    upper_char = lower_char.upper()
    queue = [(start_i,start_j)]
    route_map = [[None for _ in range(N)] for _ in range(N)]
    route_map[start_i][start_j] = (start_i,start_j)

    # color , pos , その時の移動中slimeたちのpos
    forcethrough_cand_tower = [None, None, None]

    idx = 0
    moving_slime_count = slime_stack[start_i][start_j][-1][1]

    while idx < len(queue):
        check_limit()
        i,j = queue[idx]
        for base_di, base_dj in dd:
            cur_slime_count = slime_counts[i][j]
            if i == start_i and j == start_j:
                cur_slime_count = cur_slime_count - moving_slime_count
            # 1匹いたら2まで飛べる 2匹いたら3まで飛べる
            for length in range(1, cur_slime_count+2):
                di = base_di * length
                dj = base_dj * length
                if is_inside(i+di, j+dj):
                    if grid[i+di][j+dj] == "#":
                        break
                    # その方向に自身より先に到達済みなデータが存在する
                    if route_map[i+di][j+dj] != None:
                        continue
                    route_map[i+di][j+dj] = (i,j)
                    # そのまま移動するとタワー上限を超える場合
                    if slime_counts[i+di][j+dj]+moving_slime_count > 8:
                        # ゴールでかつ空きがあれば入れる分ずつ入る
                        if grid[i+di][j+dj] == upper_char and slime_counts[i+di][j+dj] < 8:
                            movable = 8 - slime_counts[i+di][j+dj]
                            count = moving_slime_count // movable
                            if moving_slime_count % movable != 0:
                                count += 1

                            # 現在位置までを登録
                            apply_route(start_i, start_j, i, j, route_map, moving_slime_count)
                            # そこからは手動で細かく登録
                            dir = get_dir(i, j, i+di, j+dj)
                            dist = get_simple_distance(i, j, i+di, j+dj)
                            remain = (cur_slime_count+moving_slime_count)
                            for k in range(0, count):
                                if k == count-1:
                                    remain = cur_slime_count
                                else:
                                    remain -= movable
                                if steps < MAX_STEPS:
                                    check_limit()
                                    print(*[i, j, remain, dir, dist])
                                    steps += 1
                            
                            slime_stack[start_i][start_j].pop()
                            slime_counts[start_i][start_j] -= moving_slime_count
                            return None
                        # topを確認し、同一色あるいはその色のゴールに遠いものを優先する
                        if slime_stack[i+di][j+dj][-1][0] == lower_char:
                            # 同一色
                            if forcethrough_cand_tower[0] != lower_char:
                                forcethrough_cand_tower = [lower_char, (i+di, j+dj), (i,j)]
                        else:
                            if forcethrough_cand_tower[0] == lower_char:
                                continue
                            # 別色 見つかった中でその色として遠いものを優先
                            if forcethrough_cand_tower[0] == None:
                                forcethrough_cand_tower = [slime_stack[i+di][j+dj][-1][0], (i+di, j+dj), (i,j)]
                                continue

                            cur_cand_color = forcethrough_cand_tower[0]
                            cur_cand_pos = forcethrough_cand_tower[1]
                            cur_cand_dist = distances[cur_cand_color][cur_cand_pos[0]][cur_cand_pos[1]]

                            new_cand_color = slime_stack[i+di][j+dj][-1][0]
                            new_cand_pos = (i+di, j+dj)
                            new_cand_dist = distances[new_cand_color][new_cand_pos[0]][new_cand_pos[1]]

                            if new_cand_dist > cur_cand_dist:
                                forcethrough_cand_tower = [new_cand_color, new_cand_pos, (i,j)]
                    # そのまま団体で移動できる場合
                    else:
                        # 同一色に重なった
                        if len(slime_stack[i+di][j+dj]) > 0 and slime_stack[i+di][j+dj][-1][0] == lower_char:
                            apply_route(start_i, start_j, i+di, j+dj, route_map, moving_slime_count)
                            slime_stack[i+di][j+dj][-1][1] += moving_slime_count
                            slime_stack[start_i][start_j].pop()
                            slime_counts[i+di][j+dj] += moving_slime_count
                            slime_counts[start_i][start_j] -= moving_slime_count
                            return None
                        # ゴールに着いた
                        elif grid[i+di][j+dj] == upper_char:
                            apply_route(start_i, start_j, i+di, j+dj, route_map, moving_slime_count)
                            slime_stack[start_i][start_j].pop()
                            slime_counts[start_i][start_j] -= moving_slime_count
                            return None
                        # 探索中
                        else:
                            queue.append((i+di, j+dj))
                else:
                    break
        idx += 1

    new_positions = []
    # candtower手前まで移動する
    target_move_pos = forcethrough_cand_tower[2]
    if start_i != target_move_pos[0] or start_j != target_move_pos[1]:
        apply_route(start_i, start_j, target_move_pos[0], target_move_pos[1], route_map, moving_slime_count)
        slime_stack[start_i][start_j].pop()
        slime_counts[start_i][start_j] -= moving_slime_count
        if slime_counts[target_move_pos[0]][target_move_pos[1]] == 0:
            new_positions.append([-1*distances[lower_char][target_move_pos[0]][target_move_pos[1]], target_move_pos[0], target_move_pos[1]])
        slime_stack[target_move_pos[0]][target_move_pos[1]].append([lower_char, moving_slime_count])
        slime_counts[target_move_pos[0]][target_move_pos[1]] += moving_slime_count
    
    new_tower_self = None
    new_tower_other = None
    # 移動中のまとまりが大きくひっかかりすぎる場合は自身の分裂も試みる
    if moving_slime_count > 4:
        new_tower_self = separate_tower(target_move_pos[0], target_move_pos[1])
    
    new_tower_other = separate_tower(forcethrough_cand_tower[1][0], forcethrough_cand_tower[1][1])
    if new_tower_self != None:
        new_positions.append(new_tower_self)
    if new_tower_other != None:
        new_positions.append(new_tower_other)

    for positions in new_positions:
        # 分割工程で新規スライム位置のtopのdistが変わっているかもしれないので再度更新
        positions[0] = -1*distances[slime_stack[positions[1]][positions[2]][-1][0]][positions[1]][positions[2]]

    return new_positions


# 巣に最も遠いスライムからbfsし、最も近い同色スライムあるいは自身の巣へ移動
while hq:
    check_limit()
    dist, i, j = hq[0]
    new_pos = route_bfs(i, j)
    check_limit()
    if slime_counts[i][j] == 0:
        heapq.heappop(hq)
    else:
        heapq.heapreplace(hq, [distances[slime_stack[i][j][-1][0]][i][j], i ,j])
    if new_pos != None:
        for k in range(len(new_pos)):
            if new_pos[k][1] == i and new_pos[k][2] == j:
                continue
            heapq.heappush(hq, new_pos[k])
