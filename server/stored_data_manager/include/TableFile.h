//
// Created by josueaperez on 6/14/26.
//

#ifndef SERVER_TABLEFILE_H
#define SERVER_TABLEFILE_H

#include "SystemCatalog.h"
#include <string>
#include <vector>


class TableFile {
    public:
    static void create(const std::string& path,
                       const std::vector<std::string>& columns,
                       const std::vector<std::string>& types,
                       const std::vector<int>& sizes);

    static int insert(const std::string& path,
                      const std::vector<std::string>& values,
                      const TableMeta& meta);

    static std::vector<std::string> readAt(const std::string& path,
                                            int offset,
                                            const TableMeta& meta);

    static void writeAt(const std::string& path,
                        int offset,
                        const std::vector<std::string>& values,
                        const TableMeta& meta);

    static void markDeleted(const std::string& path,
                            int offset,
                            const TableMeta& meta);

    static bool isEmpty(const std::string& path);

    static std::vector<std::vector<std::string>> scan(
        const std::string& path,
        const TableMeta& meta,
        const std::string& whereColumn,
        const std::string& whereValue,
        const std::string& op);

    static std::vector<int> scanOffsets(
        const std::string& path,
        const TableMeta& meta,
        const std::string& whereColumn,
        const std::string& whereValue,
        const std::string& op);

    static std::vector<std::pair<std::vector<std::string>, int>> scanWithOffsets(
        const std::string& path,
        const TableMeta& meta);

private:
    static void writeRecord(std::ofstream& file,
                            const std::vector<std::string>& values,
                            const TableMeta& meta);

    static std::vector<std::string> readRecord(std::ifstream& file,
                                                const TableMeta& meta);

    static bool matchesWhere(const std::vector<std::string>& record,
                              const TableMeta& meta,
                              const std::string& whereColumn,
                              const std::string& whereValue,
                              const std::string& op);
};


#endif //SERVER_TABLEFILE_H
