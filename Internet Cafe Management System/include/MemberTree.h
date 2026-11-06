#pragma once
#include <vector>
#include <algorithm>
#include "Types.h"

// AVL TREE (self-balancing BST): member accounts keyed by member ID.
// Lookup and registration stay O(log n) however many members join.
class MemberTree {
    struct Node { Member data; Node* left; Node* right; int height; };
    Node* root = nullptr;
    int total = 0;

    static int h(Node* n) { return n ? n->height : 0; }
    static void update(Node* n) { n->height = 1 + std::max(h(n->left), h(n->right)); }
    static int balanceOf(Node* n) { return n ? h(n->left) - h(n->right) : 0; }

    static Node* rotateRight(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        x->right = y;
        update(y); update(x);
        return x;
    }
    static Node* rotateLeft(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        y->left = x;
        update(x); update(y);
        return y;
    }

    static Node* insertNode(Node* n, const Member& m) {
        if (!n) return new Node{m, nullptr, nullptr, 1};
        if (m.id < n->data.id) n->left = insertNode(n->left, m);
        else                   n->right = insertNode(n->right, m);
        update(n);
        int b = balanceOf(n);
        if (b > 1  && m.id < n->left->data.id)  return rotateRight(n);                                      // LL
        if (b < -1 && m.id > n->right->data.id) return rotateLeft(n);                                       // RR
        if (b > 1  && m.id > n->left->data.id)  { n->left = rotateLeft(n->left);   return rotateRight(n); } // LR
        if (b < -1 && m.id < n->right->data.id) { n->right = rotateRight(n->right); return rotateLeft(n); } // RL
        return n;
    }

    // inorder traversal: visits members in ascending ID order (used for the member list)
    static void inorder(Node* n, std::vector<Member>& out) {
        if (!n) return;
        inorder(n->left, out);
        out.push_back(n->data);
        inorder(n->right, out);
    }
    // postorder traversal: frees children before their parent (used for cleanup)
    static void destroy(Node* n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

public:
    ~MemberTree() { destroy(root); }

    Member* find(int id) {                                  // O(log n)
        Node* cur = root;
        while (cur) {
            if (id == cur->data.id) return &cur->data;
            cur = (id < cur->data.id) ? cur->left : cur->right;
        }
        return nullptr;
    }
    bool insert(const Member& m) {                          // O(log n)
        if (find(m.id)) return false;
        root = insertNode(root, m);
        total++;
        return true;
    }
    std::vector<Member> listAll() const {                   // O(n), sorted by ID
        std::vector<Member> out;
        inorder(root, out);
        return out;
    }
    int size() const { return total; }
};
