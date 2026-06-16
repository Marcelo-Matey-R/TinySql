//
// Created by josueaperez on 6/14/26.
//

#ifndef SERVER_ENCRYPTOR_H
#define SERVER_ENCRYPTOR_H

#include <vector>

class Encryptor {
private:
    static const char KEY = 0x42;

public:
    static std::vector<char> encrypt(const char* data, int size);
    static std::vector<char> decrypt(const char* data, int size);
};


#endif //SERVER_ENCRYPTOR_H
