//
// Created by josueaperez on 6/14/26.
//

#ifndef TINYSQLDB_BTREEINDEX_H
#define TINYSQLDB_BTREEINDEX_H

#include "Index.h"
#include <string>
#include <vector>

class BTreeIndex : public Index {
private:
    static const int ORDER = 4;

    struct BTreeNode {
        std::vector<std::string> keys;
        std::vector<int> offsets;
        std::vector<BTreeNode*> children;
        bool isLeaf;

        BTreeNode(bool isLeaf = true) : isLeaf(isLeaf) {}
    };

    BTreeNode* root;

public:
    BTreeIndex();
    ~BTreeIndex() override;

    void insert(const std::string &key, int offset) override;
    int search(const std::string &key) override;
    void remove(const std::string &key) override;

private:
    int search(BTreeNode *node, const std::string &key);
    void insert(BTreeNode *node, const std::string &key, int offset);
    void splitChild(BTreeNode *parent, int index, BTreeNode *child);
    void remove(BTreeNode *node, const std::string &key);
    void removeFromLeaf(BTreeNode *node, int index);
    void removeFromNonLeaf(BTreeNode *node, int index);
    std::pair<std::string,int> getPredecessor(BTreeNode *node, int index);
    void fill(BTreeNode *node, int index);
    void borrowFromPrev(BTreeNode *node, int index);
    void borrowFromNext(BTreeNode *node, int index);
    void merge(BTreeNode *node, int index);
    void destroy(BTreeNode *node);
};


#endif //TINYSQLDB_BTREEINDEX_H
