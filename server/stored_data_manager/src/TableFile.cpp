//
// Created by josueaperez on 6/14/26.
//

#include "TableFile.h"
#include "Encryptor.h"
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <sstream>

// la parte de crear el archivo
void TableFile::create(const std::string &path,
                       const std::vector<std::string> &columns,
                       const std::vector<std::string> &types,
                       const std::vector<int> &sizes) {
    std::ofstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot create table file: " + path);
}

// la parte de insertar
int TableFile::insert(const std::string &path,
                      const std::vector<std::string> &values,
                      const TableMeta &meta) {
    std::ofstream file(path, std::ios::binary | std::ios::app);
    if (!file) throw std::runtime_error("Cannot open table file: " + path);

    // el offset es la posición final del archivo
    int offset = (int) file.tellp();
    writeRecord(file, values, meta);
    return offset;
}

// la parte de leer con los offsets
std::vector<std::string> TableFile::readAt(const std::string& path,
                                            int offset,
                                            const TableMeta& meta) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot open table file: " + path);

    file.seekg(offset);
    char deleted;
    file.read(&deleted, 1);
    if (deleted == 1) return {};

    return readRecord(file, meta);
}

// la parte de escribir con offsets
void TableFile::writeAt(const std::string& path,
                        int offset,
                        const std::vector<std::string>& values,
                        const TableMeta& meta) {
    std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
    if (!file) throw std::runtime_error("Cannot open table file: " + path);

    file.seekp(offset + 1); // +1 para saltar el flag de deleted
    for (int i = 0; i < (int)values.size(); i++) {
        char buffer[meta.sizes[i]];
        memset(buffer, 0, meta.sizes[i]);
        strncpy(buffer, values[i].c_str(), meta.sizes[i] - 1);
        auto encrypted = Encryptor::encrypt(buffer, meta.sizes[i]);
        file.write(encrypted.data(), meta.sizes[i]);
    }
}

// la parte de marcar como eliminado
void TableFile::markDeleted(const std::string& path,
                             int offset,
                             const TableMeta& meta) {
    std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
    if (!file) throw std::runtime_error("Cannot open table file: " + path);

    file.seekp(offset);
    char deleted = 1;
    file.write(&deleted, 1);
}

// la parte de verificar si esta vacio
bool TableFile::isEmpty(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) return true;
    return file.tellg() == 0;
}

//la parte de busqueda secuencial
std::vector<std::vector<std::string>> TableFile::scan(
    const std::string& path,
    const TableMeta& meta,
    const std::string& whereColumn,
    const std::string& whereValue,
    const std::string& op) {

    std::vector<std::vector<std::string>> results;
    std::ifstream file(path, std::ios::binary);
    if (!file) return results;

    while (file.peek() != EOF) {
        char deleted;
        file.read(&deleted, 1);

        if (deleted == 1) {
            // saltar si un registro esta eliminado
            file.seekg(meta.recordSize - 1, std::ios::cur);
            continue;
        }

        auto record = readRecord(file, meta);
        if (whereColumn.empty() || matchesWhere(record, meta, whereColumn, whereValue, op))
            results.push_back(record);
    }
    return results;
}

std::vector<int> TableFile::scanOffsets(
    const std::string& path,
    const TableMeta& meta,
    const std::string& whereColumn,
    const std::string& whereValue,
    const std::string& op) {

    std::vector<int> offsets;
    std::ifstream file(path, std::ios::binary);
    if (!file) return offsets;

    while (file.peek() != EOF) {
        int offset = (int)file.tellg();
        char deleted;
        file.read(&deleted, 1);

        if (deleted == 1) {
            file.seekg(meta.recordSize - 1, std::ios::cur);
            continue;
        }

        auto record = readRecord(file, meta);
        if (whereColumn.empty() || matchesWhere(record, meta, whereColumn, whereValue, op))
            offsets.push_back(offset);
    }
    return offsets;
}

std::vector<std::pair<std::vector<std::string>, int>> TableFile::scanWithOffsets(
    const std::string& path,
    const TableMeta& meta) {

    std::vector<std::pair<std::vector<std::string>, int>> results;
    std::ifstream file(path, std::ios::binary);
    if (!file) return results;

    while (file.peek() != EOF) {
        int offset = (int)file.tellg();
        char deleted;
        file.read(&deleted, 1);

        if (deleted == 1) {
            file.seekg(meta.recordSize - 1, std::ios::cur);
            continue;
        }

        auto record = readRecord(file, meta);
        results.push_back({ record, offset });
    }
    return results;
}

// la parte de los helpers privados

void TableFile::writeRecord(std::ofstream& file,
                             const std::vector<std::string>& values,
                             const TableMeta& meta) {
    // flag de deleted = 0
    char deleted = 0;
    file.write(&deleted, 1);

    for (int i = 0; i < (int)values.size(); i++) {
        char buffer[meta.sizes[i]];
        memset(buffer, 0, meta.sizes[i]);
        strncpy(buffer, values[i].c_str(), meta.sizes[i] - 1);
        auto encrypted = Encryptor::encrypt(buffer, meta.sizes[i]);
        file.write(encrypted.data(), meta.sizes[i]);
    }
}

std::vector<std::string> TableFile::readRecord(std::ifstream& file,
                                                const TableMeta& meta) {
    std::vector<std::string> record;
    for (int i = 0; i < (int)meta.sizes.size(); i++) {
        char buffer[meta.sizes[i]];
        file.read(buffer, meta.sizes[i]);
        auto decrypted = Encryptor::decrypt(buffer, meta.sizes[i]);
        record.push_back(std::string(decrypted.data()));
    }
    return record;
}

bool TableFile::matchesWhere(const std::vector<std::string>& record,
                              const TableMeta& meta,
                              const std::string& whereColumn,
                              const std::string& whereValue,
                              const std::string& op) {
    int colIndex = -1;
    for (int i = 0; i < (int)meta.columns.size(); i++)
        if (meta.columns[i] == whereColumn) { colIndex = i; break; }

    if (colIndex == -1) return false;

    const std::string& val = record[colIndex];

    if (op == "=")    return val == whereValue;
    if (op == ">")    return val > whereValue;
    if (op == "<")    return val < whereValue;
    if (op == "not")  return val != whereValue;
    if (op == "like") {
        // soportar *texto* osea buscar substring
        std::string pattern = whereValue;
        if (pattern.front() == '*') pattern = pattern.substr(1);
        if (pattern.back() == '*')  pattern = pattern.substr(0, pattern.size() - 1);
        return val.find(pattern) != std::string::npos;
    }
    return false;
}