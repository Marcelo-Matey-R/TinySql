//
// Created by josueaperez on 6/14/26.
//

#ifndef TINYSQLDB_BSTINDEX_H
#define TINYSQLDB_BSTINDEX_H

#include "Index.h"
#include <string>

class BSTIndex : public Index {
    private:
    struct BSTNode {
        std::string key;
        int offset;
        BSTNode *left;
        BSTNode *right;
        BSTNode(const std::string &key, int offset)
        : key(key), offset(offset), left(nullptr), right(nullptr) {}
    };
    BSTNode *root;
    public:
    BSTIndex();
    ~BSTIndex() override;

    void insert(const std::string &key, int offset) override;
    int search(const std::string &key) override;
    void remove(const std::string &key) override;
    private:
    void insert(BSTNode *&node, const std::string &key, int offset);
    int search(BSTNode *node, const std::string &key);
    void remove(BSTNode *&node, const std::string &key);
    BSTNode *findMin(BSTNode *node);
    void destroy(BSTNode *node);
};


#endif //TINYSQLDB_BSTINDEX_H
