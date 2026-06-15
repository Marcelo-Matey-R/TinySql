//
// Created by josueaperez on 6/14/26.
//

#ifndef TINYSQLDB_INDEXFACTORY_H
#define TINYSQLDB_INDEXFACTORY_H

#include "Index.h"
#include "BSTIndex.h"
#include "BTreeIndex.h"
#include <string>
#include <memory>

class IndexFactory {
public:
    static std::unique_ptr<Index> create(const std::string &type);
};


#endif //TINYSQLDB_INDEXFACTORY_H
