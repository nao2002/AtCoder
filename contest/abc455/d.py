#abc455d
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

class LinkedListData:
    def __init__(self, num):
        self.num = num
        self.left = None
        self.right = None

    def removeLeft(self):
        if self.left != None:
            self.left.removeRight()
        self.left = None
    
    def setLeft(self, LinkedListData):
        self.left = LinkedListData
        self.left.setRight(self)
    
    def setRight(self, LinkedListData):
        self.right = LinkedListData
    
    def removeRight(self):
        self.right = None
    
N,Q = map(int,input().split())

cards = [None]*N

for i in range(N):
    cards[i] = LinkedListData(i+1)

for i in range(Q):
    C,P = map(int,input().split())
    C -= 1
    P -= 1
    cards[C].removeLeft()
    cards[C].setLeft(cards[P])

ans = [0]*N
for i in range(N):
    if cards[i].left != None:
        continue
    num = 1
    nxt = cards[i].right
    while nxt != None:
        nxt = nxt.right
        num += 1
    ans[i] = num
print(*ans)