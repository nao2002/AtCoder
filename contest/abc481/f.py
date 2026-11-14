#abc481f
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

class Mint:
    """
    modint型
    mod値には素数を使用すること

    Attributes:
        value: 現在の値
        mod: 使用しているmod値
    """
    __slots__ = ("_value", "_mod")

    def __init__(self, value=0, mod=998244353):
        """
        コンストラクタ

        Args:
            value: デフォルト数値
            mod: 余りの基準値 必ず素数を指定すること
        """
        self._mod = mod
        self._value = int(value) % mod

    def _to_mod_value(self, operand):
        """
        operandを同じmod上の整数値として取り出す

        Args:
            operand: 演算対象
        
        Returns:
            int: 現在のmodで正規化された整数値

        Raises:
            ValueError: operandがMintで、modが一致しない場合
            TypeError: operandが対応していない型の場合
        """
        if isinstance(operand, Mint):
            if self._mod != operand.mod:
                raise ValueError("Mint: cannot operate with different mod")
            return operand._value

        if isinstance(operand, int):
            return operand % self._mod

        raise TypeError(f"Mint: unsupported operand type: {type(operand).__name__}")

    @property
    def value(self):
        """
        valueを取得する

        Returns:
            int: 現在の数値
        """
        return self._value

    @property
    def mod(self):
        """
        mod値を取得する

        Returns:
            int: 使用しているmod値
        """
        return self._mod

    def __repr__(self):
        return f"{type(self).__name__}(value={self._value}, mod={self._mod})"

    def __str__(self):
        return str(self._value)

    def __int__(self):
        return self._value

    def __add__(self, rhs):
        """
        加算処理 +
        """
        rhs_value = self._to_mod_value(rhs)
        return Mint(self._value + rhs_value, self._mod)

    def __iadd__(self, rhs):
        """
        加算代入 +=
        """
        rhs_value = self._to_mod_value(rhs)
        self._value = (self._value + rhs_value) % self._mod
        return self

    def __radd__(self, lhs):
        """
        左辺に自身を加算
        """
        lhs_value = self._to_mod_value(lhs)
        return Mint(lhs_value + self._value, self._mod)

    def __sub__(self, rhs):
        """
        減算処理 -
        """
        rhs_value = self._to_mod_value(rhs)
        return Mint(self._value - rhs_value, self._mod)

    def __isub__(self, rhs):
        """
        減算代入 -=
        """
        rhs_value = self._to_mod_value(rhs)
        self._value = (self._value - rhs_value) % self._mod
        return self

    def __rsub__(self, lhs):
        """
        左辺から自身を減算
        """
        lhs_value = self._to_mod_value(lhs)
        return Mint(lhs_value - self._value, self._mod)

    def __mul__(self, rhs):
        """
        乗算処理 *
        """
        rhs_value = self._to_mod_value(rhs)
        return Mint(self._value * rhs_value, self._mod)

    def __imul__(self, rhs):
        """
        乗算代入 *=
        """
        rhs_value = self._to_mod_value(rhs)
        self._value = (self._value * rhs_value) % self._mod
        return self

    def __rmul__(self, lhs):
        """
        左辺に自身を乗算
        """
        lhs_value = self._to_mod_value(lhs)
        return Mint(lhs_value * self._value, self._mod)

    def __pow__(self, exponent, modulo=None):
        """
        累乗処理
        負の指数の場合は逆元の累乗として扱う

        Args:
            exponent: 指数
        """
        if modulo is not None:
            return NotImplemented

        if not isinstance(exponent, int):
            raise TypeError("Mint: exponent must be int")

        if exponent < 0:
            return self.inverse() ** (-exponent)

        return Mint(pow(self._value, exponent, self._mod), self._mod)

    def inverse(self):
        """
        逆元取得

        Returns:
            Mint: selfの逆元
        
        Raises:
            ZeroDivisionError: selfが0の場合
        """
        if self._value == 0:
            raise ZeroDivisionError("Mint: 0 has no modular inverse")
        # フェルマーの小定理
        return Mint(pow(self._value, self._mod - 2, self._mod), self._mod)

    def __truediv__(self, rhs):
        """
        除算処理 /
        rhsの逆元をかける
        """
        rhs_value = self._to_mod_value(rhs)
        if rhs_value == 0:
            raise ZeroDivisionError("Mint: division by zero")

        rhs_inverse = pow(rhs_value, self._mod - 2, self._mod)
        return Mint(self._value * rhs_inverse, self._mod)

    def __itruediv__(self, rhs):
        """
        除算代入 /=
        rhsの逆元をかける
        """
        rhs_value = self._to_mod_value(rhs)
        if rhs_value == 0:
            raise ZeroDivisionError("Mint: division by zero")

        rhs_inverse = pow(rhs_value, self._mod - 2, self._mod)
        self._value = (self._value * rhs_inverse) % self._mod
        return self

    def __rtruediv__(self, lhs):
        """
        左辺を自身で除算
        """
        lhs_value = self._to_mod_value(lhs)
        return Mint(lhs_value, self._mod) / self

    def __neg__(self):
        return Mint(-self._value, self._mod)

    def __eq__(self, rhs):
        try:
            rhs_value = self._to_mod_value(rhs)
        except (TypeError, ValueError):
            return False
        return self._value == rhs_value
    
    def __bool__(self):
        return self._value != 0
    
    def copy(self):
        """
        Mintを複製する

        Returns:
            Mint: selfのコピー
        """
        return Mint(self._value, self._mod)

N,M = map(int,input().split())
graph = defaultdict(list)

for i in range(M):
    A,B,C = map(int,input().split())
    graph[A].append((B,int(C)))

groups = []

nxt_targets = {}
for nxt, cost in graph[1]:
    if not nxt in nxt_targets:
        nxt_targets[nxt] = cost
    elif nxt_targets[nxt] > cost:
        nxt_targets[nxt] = cost

ans = [None]*(N+1)
ans[1] = Mint(0)
nxt_array = sorted(nxt_targets.items(), key=lambda x: x[1])
cur_idx = 0
cur_cost = 0
cur_group = SortedList()
checked = set()
while cur_idx < len(nxt_array):
    pos, cost = nxt_array[cur_idx]
    checked.add(pos)
    if cur_cost != cost:
        if len(cur_group) != 0:
            groups.append(cur_group)
        cur_group = SortedList()
        cur_cost = cost
    cur_group.add((cost, pos, Mint(cost)))
    cur_idx += 1
else:
    if len(cur_group) != 0:
        groups.append(cur_group)

group_idx = 0
nxt_length = []
sum_nxt_length = 0
while group_idx < len(groups):
    nxt_length.append({})
    cur_group = groups[group_idx]
    checking = set()
    added_this_time = 0
    for cost, pos, sum_cost in cur_group:
        ans[pos] = sum_cost
        for nxt, nxt_cost in graph[pos]:
            if ans[nxt] is None and not nxt in checked:
                if not nxt in nxt_length[-1]:
                    nxt_length[-1][nxt] = (nxt_cost, nxt, sum_cost*10 + Mint(nxt_cost))
                    added_this_time += 1
                elif nxt_length[-1][nxt][0] > nxt_cost:
                    nxt_length[-1][nxt] = (nxt_cost, nxt, sum_cost*10 + Mint(nxt_cost))
                checking.add(nxt)
    checked = checked & checking
    if added_this_time > 0:
        sum_nxt_length += added_this_time
    else:
        nxt_length.pop()

    group_idx += 1

    # 現在の桁が終わった
    if group_idx >= len(groups) and sum_nxt_length > 0:
        checking = set()
        for nxt_list in nxt_length:
            nxt_group = SortedList()
            for key in nxt_list:
                if ans[key] is not None or key in checking:
                    continue
                nxt_group.add((nxt_list[key][0], nxt_list[key][1], nxt_list[key][2]))
                checking.add(key)
            if len(nxt_group) > 0:
                groups.append(nxt_group)
        nxt_length = []
        sum_nxt_length = 0

for i in range(2,N+1):
    print(ans[i])

