//
// Created by josueaperez on 6/14/26.
//

#include "BTreeIndex.h"
#include <stdexcept>

BTreeIndex::BTreeIndex() {
    root = new BTreeNode(true);
}

BTreeIndex::~BTreeIndex() {
    destroy(root);
}

//los publicos

int BTreeIndex::search(const std::string &key) {
    return search(root, key);
}

void BTreeIndex::insert(const std::string &key, int offset) {
    //verifica si hay duplicados
    if (search(key) != -1)
        throw std::runtime_error("Duplicate key: " + key);

    // si la raiz esta llena se debe dividir
    if ((int)root->keys.size() == ORDER -1) {
        BTreeNode *newRoot = new BTreeNode(false);
        newRoot->children.push_back(root);
        splitChild(newRoot,0,root);
        root=newRoot;
    }
    insert(root, key, offset);
}

void BTreeIndex::remove(const std::string &key) {
    if (search(key) == -1)
        throw std::runtime_error("key not found: " + key);
    remove(root, key);

    // en el caso de que la raiz quede vacia despues del remove
    // el unico hijo es la nueva raiz
    if (root->keys.empty() && !root->isLeaf) {
        BTreeNode *oldRoot = root;
        root = root->children[0];
        delete oldRoot;
    }
}

// Parte de busqueda

int BTreeIndex::search(BTreeNode *node, const std::string &key) {
    int i = 0;
    while (i < (int)node->keys.size() && key > node->keys[i])
        i++;

    if (i < (int)node->keys.size() && key == node->keys[i])
        return node->offsets[i];
    
    if (node->isLeaf)
        return -1;
    return search(node->children[i], key);
}

// Parte de insertar

void BTreeIndex::insert(BTreeNode *node, const std::string &key, int offset) {
    int i = (int)node->keys.size()-1;

    if (node->isLeaf) {
        // se debe insertar en la posicion correcta
        node->keys.push_back("");
        node->offsets.push_back(0);
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i+1] = node->keys[i];
            node->offsets[i+1] = node->offsets[i];
            i--;
        }
        node->keys[i+1] = key;
        node->offsets[i+1] = offset;
    } else {
        // hay que encontrar el hijo correcto
        while (i>=0 && key < node->keys[i])
            i--;
        i++;

        // si el hijo se encuentra lleno hay que partirlo antes
        if ((int)node->children[i]->keys.size() == ORDER -1) {
            splitChild(node,i,node->children[i]);
            if (key > node->keys[i])
                i++;
        }
        insert(node->children[i], key, offset);
    }
}

void BTreeIndex::splitChild(BTreeNode *parent, int index, BTreeNode *child) {
    int mid = ORDER /2 -1;
    BTreeNode *newNode = new BTreeNode(child->isLeaf);

    // hay que copiar la mitad derecha del hijo al nuevo nodo
    for (int i = mid + 1; i< (int)child->keys.size(); i++) {
        newNode->keys.push_back(child->keys[i]);
        newNode->offsets.push_back(child->offsets[i]);
    }
    if (!child->isLeaf) {
        for (int i = mid + 1; i<child->children.size(); i++) {
            newNode->children.push_back(child->children[i]);
        }
    }

    // La key del medio debe subir al padre
    std::string midKey = child->keys[mid];
    int midOffset = child->offsets[mid];

    // recortar el hijo original
    child->keys.resize(mid);
    child->offsets.resize(mid);
    if (!child->isLeaf)
        child->children.resize(mid+1);

    // se inserta el nodo en el padre
    parent->children.insert(parent->children.begin()+index+1, newNode);
    parent->keys.insert(parent->keys.begin()+index,midKey);
    parent->offsets.insert(parent->offsets.begin() + index, midOffset);
}

// la parte de eliminacion

void BTreeIndex::remove(BTreeNode *node, const std::string &key) {
    int i = 0;
    while (i < (int)node->keys.size() && key > node->keys[i])
        i++;

    if (i< (int)node->keys.size() && key == node->keys[i]) {
        if (node->isLeaf)
            removeFromLeaf(node,i);
        else
            removeFromNonLeaf(node,i);
    } else {
        if (node->isLeaf)
            return;
        if ((int)node->children[i]->keys.size() < ORDER/2)
            fill(node,i);
        remove(node->children[i], key);
    }
}

void BTreeIndex::removeFromLeaf(BTreeNode *node, int index) {
    node->keys.erase(node->keys.begin() + index);
    node->offsets.erase(node->offsets.begin() + index);
}

void BTreeIndex::removeFromNonLeaf(BTreeNode *node, int index) {
    auto [predKey, predOffset] = getPredecessor(node,index);
    node->keys[index] = predKey;
    node->offsets[index] = predOffset;
    remove(node->children[index],predKey);
}

std::pair<std::string, int> BTreeIndex::getPredecessor(BTreeNode *node, int index) {
    BTreeNode *current = node->children[index];
    while (!current->isLeaf)
        current = current->children.back();
    return {current->keys.back(), current->offsets.back()};
}

void BTreeIndex::fill(BTreeNode *node, int index) {
    if (index > 0 && (int)node->children[index -1]->keys.size() >= ORDER/2)
        borrowFromPrev(node,index);
    else if (index < (int)node->children.size()-1 &&
        (int)node->children[index+1]->keys.size() >= ORDER/2)
        borrowFromNext(node,index);
    else {
        if (index < (int)node->children.size()-1)
            merge(node,index);
        else
            merge(node, index-1);
    }
}

void BTreeIndex::borrowFromPrev(BTreeNode *node, int index) {
    BTreeNode *child = node->children[index];
    BTreeNode *sibling = node->children[index-1];

    child->keys.insert(child->keys.begin(), node->keys[index-1]);
    child->offsets.insert(child->offsets.begin(), node->offsets[index-1]);

    if (!child->isLeaf)
        child->children.insert(child->children.begin(), sibling->children.back());

    node->keys[index-1] = sibling->keys.back();
    node->offsets[index-1] = sibling->offsets.back();

    sibling->keys.pop_back();
    sibling->offsets.pop_back();
    if (!sibling->isLeaf)
        sibling->children.pop_back();
}

void BTreeIndex::borrowFromNext(BTreeNode *node, int index) {
    BTreeNode *child = node->children[index];
    BTreeNode *sibling = node->children[index+1];

    child->keys.push_back(node->keys[index]);
    child->offsets.push_back(node->offsets[index]);

    if (!child->isLeaf)
        child->children.push_back(sibling->children.front());

    node->keys[index] = sibling->keys.front();
    node->offsets[index] = sibling->offsets.front();

    sibling->keys.erase(sibling->keys.begin());
    sibling->offsets.erase(sibling->offsets.begin());
    if (sibling->isLeaf)
        sibling->children.erase(sibling->children.begin());
}

void BTreeIndex::merge(BTreeNode *node, int index) {
    BTreeNode *left = node->children[index];
    BTreeNode *right = node->children[index+1];

    // se baja la key del padre al nodo de la izquierda
    left->keys.push_back(node->keys[index]);
    left->offsets.push_back(node->offsets[index]);

    // se baja todo del nodo derecho al izquierdo
    for (auto &k : right->keys) left->keys.push_back(k);
    for (auto &o : right->offsets) left->offsets.push_back(o);
    if (!right->isLeaf)
        for (auto *c : right->children) left->children.push_back(c);

    // se elimina la key del padre y del hijo derecho
    node->keys.erase(node->keys.begin() + index);
    node->offsets.erase(node->offsets.begin() + index);
    node->children.erase(node->children.begin() + index);

    delete right;
}

void BTreeIndex::destroy(BTreeNode *node) {
    if (node==nullptr) return;
    if (!node->isLeaf)
        for (auto *child : node->children)
            destroy(child);
    delete node;
}