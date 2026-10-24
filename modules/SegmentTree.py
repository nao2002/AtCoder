class SegmentTree:
    """
    セグメント木
    """

    def __init__(self, init_values, segfunc, ide_ele):
        """
        init_values: 配列の初期値
        segfunc(left, right): 区間に対して行う関数
        ide_ele: 単位元
        """
        # 元の要素数
        self._element_count = len(init_values)
        self._segfunc = segfunc
        self._ide_ele = ide_ele
        # 葉の数（element_count以上の最小の2のべき乗、最低1）
        self._leaf_count = 1 << (max(1, self._element_count) - 1).bit_length()
        # セグメント木本体 (1-indexed)
        self._tree = [ide_ele] * (2 * self._leaf_count)

        # 葉に初期値をセット
        for index, value in enumerate(init_values):
            self._tree[self._leaf_count + index] = value

        # 葉から根に向かって構築
        for node in range(self._leaf_count - 1, 0, -1):
            left = self._tree[2 * node]
            right = self._tree[2 * node + 1]
            self._tree[node] = self._segfunc(left, right)

    def update(self, index, value):
        """
        index番目の値をvalueに更新
        index: 更新する要素の位置 (0-indexed)
        value: 更新後の値
        """
        assert 0 <= index < self._element_count

        node = self._leaf_count + index
        self._tree[node] = value

        # 根に向かって親ノードを更新
        while node > 1:
            left = self._tree[node & ~1]
            right = self._tree[node | 1]
            node >>= 1
            self._tree[node] = self._segfunc(left, right)

    def query(self, left, right):
        """
        [left, right)な半開区間にsegfuncを適用した結果を返す
        left: 区間の左端 (0-indexed)
        right: 区間の右端 (0-indexed)
        """
        left_result = self._ide_ele
        right_result = self._ide_ele

        left += self._leaf_count
        right += self._leaf_count
        while left < right:
            # 左端が右の子だった場合
            if left & 1:
                left_result = self._segfunc(left_result, self._tree[left])
                left += 1
            # 右端が右の子だった場合 (半開区間なため取得対象は直前の左の子となる)
            if right & 1:
                right_result = self._segfunc(self._tree[right - 1], right_result)
            left >>= 1
            right >>= 1
        return self._segfunc(left_result, right_result)

    def max_right(self, left, condition_func):
        """
        [left, right) の結果が条件を満たす最大のrightを返す
        left: 区間の左端 (0-indexed)
        condition_func(result): 結果を受け取りboolを返す関数
        """
        assert 0 <= left <= self._element_count
        assert condition_func(self._ide_ele)

        if left == self._element_count:
            return left

        node = self._leaf_count + left
        result = self._ide_ele

        while True:
            # 左端から始まる大きな区間を探す
            while (node & 1) == 0:
                node >>= 1

            candidate = self._segfunc(result, self._tree[node])

            if not condition_func(candidate):
                # 条件を満たさなくなった区間を細分化
                # 葉の階層に到達するまで繰り返す
                while node < self._leaf_count:
                    node <<= 1
                    candidate = self._segfunc(result, self._tree[node])

                    if condition_func(candidate):
                        result = candidate
                        node += 1

                return node - self._leaf_count

            result = candidate
            node += 1

            # 今見ている階層の右端まで確認しきった
            if (node & -node) == node:
                break

        return self._element_count

    def min_left(self, right, condition_func):
        """
        [left, right) の結果が条件を満たす最小のleftを返す
        right: 区間の右端 (0-indexed)
        condition_func(result): 結果を受け取りboolを返す関数
        """
        assert 0 <= right <= self._element_count
        assert condition_func(self._ide_ele)

        if right == 0:
            return 0

        node = self._leaf_count + right
        result = self._ide_ele

        while True:
            node -= 1

            # 右端までの大きな区間を探す
            while node > 1 and (node & 1):
                node >>= 1

            candidate = self._segfunc(self._tree[node], result)

            if not condition_func(candidate):
                # 条件を満たさなくなった区間を細分化
                # 葉の階層に到達するまで繰り返す
                while node < self._leaf_count:
                    node = (node << 1) | 1
                    candidate = self._segfunc(self._tree[node], result)

                    if condition_func(candidate):
                        result = candidate
                        node -= 1

                # condがTrueな左端を返すために自身の直後を返す
                return node + 1 - self._leaf_count

            result = candidate

            # 今見ている階層の左端まで確認しきった
            if (node & -node) == node:
                break

        return 0
