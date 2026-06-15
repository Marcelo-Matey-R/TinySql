//
// Created by josueaperez on 6/14/26.
//

#include "IndexFactory.h"
#include <stdexcept>

std::unique_ptr<Index> IndexFactory::create(const std::string &type) {
    if (type == "BST") return std::make_unique<BSTIndex>();
    if (type == "BTREE") return std::make_unique<BTreeIndex>();
    throw std::runtime_error("Unknown index type: " + type);
}