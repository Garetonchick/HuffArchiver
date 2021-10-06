#pragma once
#include <functional>
#include <queue>

template <class T, class Compare = std::less<T>>
class PriorityQueue {
public:
    ~PriorityQueue();

    void Push(T value);
    void Pop();

    const T& Top() const;
    bool IsEmpty() const;
    size_t GetSize() const;

private:
    struct Node {
        Node* left_child = nullptr;
        Node* right_child = nullptr;
        size_t subtree_size = 0;
        T value;
    };

private:
    Node* root_ = nullptr;
    size_t size_ = 0;
};

template <class T, class Compare>
PriorityQueue<T, Compare>::~PriorityQueue() {
    std::queue<Node*> to_delete;

    to_delete.push(root_);

    while (!to_delete.empty()) {
        Node* node = to_delete.front();
        to_delete.pop();

        if (node != nullptr) {
            if (node->left_child != nullptr) {
                to_delete.push(node->left_child);
            }

            if (node->right_child != nullptr) {
                to_delete.push(node->right_child);
            }
        }

        delete node;
    }
}

template <class T, class Compare>
void PriorityQueue<T, Compare>::Push(T value) {
    Node** current_node = &root_;

    while ((*current_node) != nullptr) {
        ++(*current_node)->subtree_size;

        if (Compare()(value, (*current_node)->value)) {
            std::swap((*current_node)->value, value);
        }

        if ((*current_node)->left_child == nullptr) {
            current_node = &(*current_node)->left_child;
        } else if ((*current_node)->right_child == nullptr) {
            current_node = &(*current_node)->right_child;
        } else {
            if ((*current_node)->left_child->subtree_size < (*current_node)->right_child->subtree_size) {
                current_node = &(*current_node)->left_child;
            } else {
                current_node = &(*current_node)->right_child;
            }
        }
    }

    (*current_node) = new Node{.value = std::move(value)};
    ++size_;
}

template <class T, class Compare>
void PriorityQueue<T, Compare>::Pop() {
    Node** current_node = &root_;
    Node** previous_node = &root_;

    if (size_ == 0) {
        throw std::runtime_error("PriorityQueue::POP: Attempt to pop from empty queue");
    }

    while ((*current_node)->left_child != nullptr || (*current_node)->right_child != nullptr) {
        previous_node = current_node;
        --(*current_node)->subtree_size;

        if ((*current_node)->left_child == nullptr) {
            std::swap((*current_node)->value, (*current_node)->right_child->value);
            current_node = &(*current_node)->right_child;
        } else if ((*current_node)->right_child == nullptr) {
            std::swap((*current_node)->value, (*current_node)->left_child->value);
            current_node = &(*current_node)->left_child;
        } else {
            if (Compare()((*current_node)->right_child->value, (*current_node)->left_child->value)) {
                std::swap((*current_node)->left_child, (*current_node)->right_child);
            }

            std::swap((*current_node)->value, (*current_node)->left_child->value);

            current_node = &(*current_node)->left_child;
        }
    }

    if ((*previous_node)->left_child == (*current_node)) {
        (*previous_node)->left_child = nullptr;
    } else {
        (*previous_node)->right_child = nullptr;
    }

    delete (*current_node);

    --size_;

    if (size_ == 0) {
        root_ = nullptr;
    }
}

template <class T, class Compare>
const T& PriorityQueue<T, Compare>::Top() const {
    if (root_ == nullptr) {
        throw std::runtime_error("PRIORITY_QUEUE::TOP: This function can't be used on empty queue");
    }

    return root_->value;
}

template <class T, class Compare>
bool PriorityQueue<T, Compare>::IsEmpty() const {
    return size_ == 0;
}

template <class T, class Compare>
size_t PriorityQueue<T, Compare>::GetSize() const {
    return size_;
}