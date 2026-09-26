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
    