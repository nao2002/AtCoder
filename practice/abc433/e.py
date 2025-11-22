#abc433e
import sys
from collections import defaultdict
from collections import deque
import heapq
import pypyjit
import random
# from sortedcontainers import SortedList, SortedDict, SortedSet
pypyjit.set_param('max_unroll_recursion=-1')
sys.setrecursionlimit(10**8)
sys.set_int_max_str_digits(0)

def input(): return (sys.stdin.readline()).rstrip()

def create_sample():
    N = random.randint(1, 10000)
    M = random.randint(1, 10000)
    num = list(range(N*M))
    random.shuffle(num)
    board = []
    for i in range(N):
        board.append([])
        for _ in range(M):
            rnd = num.pop()
            board[i].append(rnd+1)
    X = []
    Y = []
    for i in range(N):
        mx = -1
        for j in range(M):
            if board[i][j] > mx:
                mx = board[i][j]
        X.append(mx)
    for j in range(M):
        mx = -1
        for i in range(N):
            if board[i][j] > mx:
                mx = board[i][j]
        Y.append(mx)
    
    return [N, M, X, Y]

def recheck_answer(N,M,X,Y,board):
    for i in range(N):
        mx = max(board[i])
        if mx != X[i]:
            return False
    
    for j in range(M):
        mx = -1
        for i in range(N):
            if board[i][j] > mx:
                mx = board[i][j]
        if mx != Y[j]:
            return False

    return True

T = int(input())

for _ in range(T):
    N,M = map(int,input().split())
    X = list(map(int,input().split()))
    Y = list(map(int,input().split()))
    
    set_x = set(X)
    set_y = set(Y)
    if len(set_x) != N or len(set_y) != M:
        print("No")
        continue
    # args = create_sample()
    # N = args[0]
    # M = args[1]
    # X = args[2]
    # Y = args[3]

    x_with_idx = list((X[i],i) for i in range(N))
    y_with_idx = list((Y[i],i) for i in range(M))
    x_with_idx.sort()
    y_with_idx.sort()
    place_map = list(i for i in range(N*M+1))

    def fetch_num(num):
        x = num
        while place_map[x] != x:
            place_map[x] = place_map[place_map[x]]
            x = place_map[x]

        if x == 0:
            return -1
        
        place_map[x] = x - 1
        
        return x

    board = [[-1 for _ in range(M)] for _ in range(N)]
    broke = False
    for x, i in x_with_idx:
        for y, j in y_with_idx:
            minimum = min(X[i], Y[j])
            num = fetch_num(minimum)
            if num <= 0:
                broke = True
                break
            board[i][j] = num
        if broke:
            break
    if broke:
        print("No")
        # print(N,M,X,Y)
        # for i in range(N):
        #     print(*board[i])
        # break
    else:
        print("Yes")
        # if not recheck_answer(N, M, X, Y, board):
        #     print("Recheck Failure!")
        #     print(N,M,X,Y)
        #     for i in range(N):
        #         print(*board[i])
        #     break
        for i in range(N):
          print(*board[i])
