//
// Created by josueaperez on 6/14/26.
//

#ifndef TINYSQLDB_INDEX_H
#define TINYSQLDB_INDEX_H

#include <string>
#include <vector>

class Index {
public:
    virtual void insert(const std::string& key, int offset) = 0;
    virtual int search(const std::string& key) = 0;
    virtual void remove(const std::string& key) = 0;
    virtual ~Index() = default;
};


#endif //TINYSQLDB_INDEX_H
