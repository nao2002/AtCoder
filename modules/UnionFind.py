class UnionFind:
    def __init__(self, length):
        """
        length: 配列長
        """
        self.length = length
        self.par = [-1]*length
    
    def root(self, x):
        """
        x番目の要素の親を取得
        x: index(0-indexed)

        return: 要素の親index(0-indexed)
        """
        if self.par[x] < 0:
            return x
        else:
            self.par[x] = self.root(self.par[x])
            return self.par[x]
    
    def unite(self, x, y):
        """
        x番目とy番目の所属するグループを合成
        x: index(0-indexed)
        y: index(0-indexed)

        return: 合成後のグループのサイズ
        """
        x,y = self.root(x), self.root(y)
        if x == y:
            return -self.par[x]
        # グループのサイズが大きい方を親にする
        if self.par[x] > self.par[y]:
            x,y = y,x
        self.par[x] += self.par[y]
        self.par[y] = x
        return -self.par[x]
    
    def size(self, x):
        """
        x番目の所属するグループのサイズを取得
        x: index(0-indexed)

        return: グループのサイズ
        """
        return -self.par[self.root(x)]
    
    def parents(self):
        """
        全ての親のindexを取得 O(N)

        return: 全ての親のindex(0-indexed)の配列
        """
        return [i for i in range(self.length) if self.par[i] < 0]

