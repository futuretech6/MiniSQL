# Class

BranchIndex：存储B+树非叶节点的index信息

LeafIndex：存储B+树叶节点的index信息

TreeNode：B+树节点的基类

BranchNode：B+树的非叶节点

LeafNode：B+树的叶节点

# Public Methods

`void createIndex(const Table &tableInfo, Index &indexInfo);`：提供表和索引类，在索引类的引用上创建索引

`Data selectEqual(const Table &tableInfo, const Index &indexInfo, string key, int blockOffset = 0);`：等值选取操作，用indexInfo传递比较属性和索引，用字符串key(类型转换)作为比较值。外部调用的时候blockOffset=0(从根开始)，内部递归的时候会另外取值。

`Data selectBetween(const Table &tableInfo, const Index &indexInfo, string keyFrom, string keyTo, int blockOffset = 0);`：范围选取，keyFrom和keyTo是字符型的比较值，其他同上

`void dropIndex(Index &indexInfo);`：删除索引indexInfo