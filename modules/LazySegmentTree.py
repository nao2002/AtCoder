class LazySegmentTree:
    """
    遅延セグメント木
    """

    def __init__(self, init_values, segfunc, ide_ele, mapping, composition):
        """
        init_values: 配列の初期値
        segfunc(left, right): 区間に対して行う関数
        ide_ele: 単位元
        mapping(operation, value, length): 区間更新を集計値に反映する関数
        composition(new, old): old に new を適用する合成関数
        """
        # 元の要素数
        self._element_count = len(init_values)
        self._segfunc = segfunc
        self._ide_ele = ide_ele
        self._mapping = mapping
        self._composition = composition

        # 葉の数（element_count以上の最小の2のべき乗、最低1）
        self._leaf_count = 1 << (max(1, self._element_count) - 1).bit_length()
        self._height = self._leaf_count.bit_length() - 1

        # セグメント木本体 (1-indexed)
        self._tree = [ide_ele] * (2 * self._leaf_count)

        # 各ノードに含まれる実際の要素の数（ダミー葉は0個として換算）
        self._length = [0] * (2 * self._leaf_count)

        # 遅延情報が無いことを表す番兵
        self.NO_UPDATE = object()
        # 遅延情報の記録用配列
        self._lazy = [self.NO_UPDATE] * self._leaf_count

        # 葉に初期値と要素数をセット
        for index, value in enumerate(init_values):
            node = self._leaf_count + index
            self._tree[node] = value
            self._length[node] = 1

        # 葉から根に向かって構築
        for node in range(self._leaf_count - 1, 0, -1):
            self._length[node] = self._length[2 * node] + self._length[2 * node + 1]
            self._pull(node)

    def _pull(self, node):
        """左右の子ノードから親ノードの集計値を再計算する。"""
        self._tree[node] = self._segfunc(self._tree[2 * node], self._tree[2 * node + 1])

    def _apply(self, node, operation):
        """このノードの集計値だけ更新し、子への反映は保留する。"""
        self._tree[node] = self._mapping(operation, self._tree[node], self._length[node])

        if node < self._leaf_count:
            old = self._lazy[node]
            if old is self.NO_UPDATE:
                self._lazy[node] = operation
            else:
                self._lazy[node] = self._composition(operation, old)

    def _push(self, node):
        """保留していた更新を左右の子へ伝える。"""
        operation = self._lazy[node]
        if operation is self.NO_UPDATE:
            return

        self._apply(2 * node, operation)
        self._apply(2 * node + 1, operation)
        self._lazy[node] = self.NO_UPDATE

    def _push_path(self, node):
        """根から指定ノードの親まで、遅延を上から順に伝える。"""
        for level in range(self._height, 0, -1):
            self._push(node >> level)

    def _push_boundaries(self, left, right):
        """区間端点にかかる（途中で分割される）ノードの遅延を伝える。"""
        for level in range(self._height, 0, -1):
            if left & ((1 << level) - 1):
                self._push(left >> level)
            if right & ((1 << level) - 1):
                self._push((right - 1) >> level)

    def _pull_boundaries(self, left, right):
        """区間更新で一部分だけ変わった祖先の集計値を再計算する。"""
        for level in range(1, self._height + 1):
            if left & ((1 << level) - 1):
                self._pull(left >> level)
            if right & ((1 << level) - 1):
                self._pull((right - 1) >> level)

    def update(self, index, value):
        """
        index番目の値をvalueに更新
        index: 更新する要素の位置 (0-indexed)
        value: 更新後の値
        """
        assert 0 <= index < self._element_count

        node = self._leaf_count + index
        self._push_path(node)
        self._tree[node] = value

        # 根に向かって親ノードを更新
        while node > 1:
            node >>= 1
            self._pull(node)

    def apply(self, left, right, operation):
        """
        [left, right) の各要素に更新操作を適用
        left: 区間の左端 (0-indexed)
        right: 区間の右端 (0-indexed)
        operation: 更新操作
        """
        assert 0 <= left <= right <= self._element_count
        if left == right:
            return

        left += self._leaf_count
        right += self._leaf_count
        original_left, original_right = left, right

        # 区間を分割する前に、両端に残っている遅延を伝える
        self._push_boundaries(left, right)

        while left < right:
            if left & 1:
                self._apply(left, operation)
                left += 1
            if right & 1:
                right -= 1
                self._apply(right, operation)
            left >>= 1
            right >>= 1

        # 部分更新された祖先だけを下から再計算する
        self._pull_boundaries(original_left, original_right)

    def query(self, left, right):
        """
        [left, right) の区間集計値を返す
        left: 区間の左端 (0-indexed)
        right: 区間の右端 (0-indexed)
        """
        assert 0 <= left <= right <= self._element_count
        if left == right:
            return self._ide_ele

        left += self._leaf_count
        right += self._leaf_count
        self._push_boundaries(left, right)

        left_result = self._ide_ele
        right_result = self._ide_ele

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
        self._push_path(node)
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
                    self._push(node)
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
        self._push_path(node - 1)
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
                    self._push(node)
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
