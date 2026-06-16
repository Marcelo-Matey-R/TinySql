//
// Created by josueaperez on 6/14/26.
//

#include "Encryptor.h"

std::vector<char> Encryptor::encrypt(const char* data, int size) {
    std::vector<char> result(data, data + size);
    for (auto& byte : result)
        byte ^= KEY;
    return result;
}

std::vector<char> Encryptor::decrypt(const char* data, int size) {
    return encrypt(data, size); // XOR dos veces = original
}