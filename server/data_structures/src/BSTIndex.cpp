//
// Created by josueaperez on 6/14/26.
//

#include "BSTIndex.h"
#include <stdexcept>

BSTIndex::BSTIndex() : root(nullptr) {}

BSTIndex::~BSTIndex() {
    destroy(root);
}

// Los metodos publicos

void BSTIndex::insert(const std::string& key, int offset) {
    insert(root, key, offset);
}

int BSTIndex::search(const std::string& key) {
    return search(root, key);
}

void BSTIndex::remove(const std::string& key) {
    remove(root, key);
}

// ahora los privados

void BSTIndex::insert(BSTNode *&node, const std::string &key, int offset) {
    if (node==nullptr) {
        node = new BSTNode(key,offset);
        return;
    }
    if (key < node->key)
        insert(node->left, key, offset);
    else if (key > node -> key)
        insert(node->right, key, offset);
    else
        throw std::runtime_error("Duplicate key: " + key);
}

int BSTIndex::search(BSTNode *node,const std::string &key) {
    if (node==nullptr)
        return -1;
    if (key==node->key)
        return node->offset;
    if (key < node->key)
        return search(node->left, key);
    return search(node->right, key);
}

void BSTIndex::remove(BSTNode *&node, const std::string &key) {
    if (node==nullptr)
        return;
    if (key < node->key) {
        remove(node->left, key);
    } else if (key > node->key) {
        remove(node->right, key);
    } else {
        //hay tres casos al remover
        if (node->left == nullptr && node->right == nullptr) {
            //si no tiene hijos
            delete node;
            node =nullptr;
        } else if (node -> left ==nullptr) {
            //si solo tiene hijo derecho
            BSTNode *temp = node;
            node = node->right;
            delete temp;
        } else if (node->right == nullptr) {
            //si tiene hijo zurdo
            BSTNode *temp = node;
            node = node->left;
            delete temp;
        } else {
            // si tiene ambos hijos se reemplaza con el menor
            // del arbol familiar derecho
            BSTNode *min = findMin(node->right);
            node->key = min->key;
            node->offset = min->offset;
            remove(node->right, min->key);
        }
    }
}

BSTIndex::BSTNode* BSTIndex::findMin(BSTNode *node) {
    while (node->left != nullptr)
        node = node->left;
    return node;
}

void BSTIndex::destroy(BSTNode *node) {
    if (node == nullptr)
        return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}