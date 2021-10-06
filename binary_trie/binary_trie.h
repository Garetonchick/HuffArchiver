#pragma once
#include <vector>
#include <stdexcept>
#include <queue>

template <class T>
class BinaryTrie {
    struct Node;

public:
    struct BinaryPath {
        bool operator<(const BinaryPath& o) const;

        size_t code = 0;
        size_t length = 0;
    };

    class Iterator {
        friend BinaryTrie;

    public:
        void operator++();

        const T& operator*() const;
        bool operator==(const Iterator& o) const;
        bool operator!=(const Iterator& o) const;

        BinaryPath GetPath() const;

    private:
        Iterator(const Node* node, const BinaryPath& path);
        Iterator() = default;

    private:
        const Node* node_ = nullptr;
        BinaryPath path_;
    };

    class Traverser {
        friend BinaryTrie;

    public:
        const BinaryPath& operator*() const;

        bool HasValue() const;
        const T& GetValue() const;
        bool CanGoLeft() const;
        bool CanGoRight() const;

        void GoLeft();
        void GoRight();

    private:
        Traverser(const Node* node, const BinaryPath& path);

    private:
        const Node* node_ = nullptr;
        BinaryPath path_;
    };

private:
    struct Node {
        T* value = nullptr;
        Node* left_child = nullptr;
        Node* right_child = nullptr;
        Node* ancestor = nullptr;
    };

public:
    BinaryTrie() = default;
    explicit BinaryTrie(const T& root_value);
    BinaryTrie(const BinaryTrie& o) = delete;
    BinaryTrie& operator=(const BinaryTrie& o) = delete;
    BinaryTrie(BinaryTrie&& o) noexcept;
    BinaryTrie& operator=(BinaryTrie&& o) noexcept;
    ~BinaryTrie();

    void Merge(BinaryTrie&& o);
    void Insert(const T& value, BinaryPath path);

    Iterator begin() const;
    Iterator end() const;

    Traverser GetRootTraverser() const;

private:
    Node* root_ = nullptr;
    Node* begin_node_ = nullptr;
    BinaryPath begin_path_;
};

template <class T>
BinaryTrie<T>::BinaryTrie(const T& root_value) {
    begin_node_ = root_ = new Node();
    root_->value = new T(root_value);
}

template <class T>
BinaryTrie<T>::BinaryTrie(BinaryTrie&& o) noexcept {
    root_ = o.root_;
    begin_node_ = o.begin_node_;
    begin_path_ = o.begin_path_;
    o.root_ = nullptr;
}

template <class T>
BinaryTrie<T>& BinaryTrie<T>::operator=(BinaryTrie&& o) noexcept {
    root_ = o.root_;
    begin_node_ = o.begin_node_;
    begin_path_ = o.begin_path_;
    o.root_ = nullptr;

    return *this;
}

template <class T>
BinaryTrie<T>::~BinaryTrie() {
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

template <class T>
void BinaryTrie<T>::Merge(BinaryTrie&& o) {
    if (o.root_ == nullptr) {
        return;
    }

    if (root_ == nullptr) {
        (*this) = std::move(o);
        return;
    }

    Node* new_root = new Node();

    new_root->left_child = root_;
    new_root->right_child = o.root_;

    root_ = new_root;
    o.root_ = nullptr;

    root_->left_child->ancestor = root_;
    root_->right_child->ancestor = root_;

    ++begin_path_.length;
    begin_path_.code <<= 1;
}

template <class T>
void BinaryTrie<T>::Insert(const T& value, BinaryPath path) {
    if (root_ == nullptr) {
        root_ = new Node();
    }

    Node* current_node = root_;

    for (size_t i = 0; i < path.length; ++i) {
        Node* next_node = current_node->left_child;
        bool right_child = ((path.code >> i) & 1);

        if (right_child) {
            next_node = current_node->right_child;
        }

        if (next_node == nullptr) {
            if (i + 1 == path.length) {
                next_node = new Node();
                next_node->value = new T(value);
            } else {
                next_node = new Node();
            }

            if (right_child) {
                current_node->right_child = next_node;
            } else {
                current_node->left_child = next_node;
            }

            current_node->ancestor = current_node;
        }

        current_node = next_node;
    }

    if (path < begin_path_) {
        begin_path_ = path;
        begin_node_ = current_node;
    }
}

template <class T>
typename BinaryTrie<T>::Iterator BinaryTrie<T>::begin() const {
    return Iterator(begin_node_, begin_path_);
}

template <class T>
typename BinaryTrie<T>::Iterator BinaryTrie<T>::end() const {
    return BinaryTrie::Iterator(nullptr, {});
}

template <class T>
typename BinaryTrie<T>::Traverser BinaryTrie<T>::GetRootTraverser() const {
    return Traverser(root_, {});
}

template <class T>
bool BinaryTrie<T>::BinaryPath::operator<(const BinaryPath& o) const {
    for (size_t i = 0; i < std::min(length, o.length); ++i) {
        bool bit1 = ((code >> i) & 1);
        bool bit2 = ((o.code >> i) & 1);

        if (bit1 != bit2) {
            if (bit1 < bit2) {
                return true;
            } else {
                return false;
            }
        }
    }

    if (length <= o.length) {
        return false;
    }

    return true;
}

template <class T>
BinaryTrie<T>::Iterator::Iterator(const BinaryTrie::Node* node, const BinaryPath& path) : node_(node), path_(path) {
}

template <class T>
void BinaryTrie<T>::Iterator::operator++() {
    if (node_ == nullptr) {
        return;
    }

    bool was_right_turn = false;
    const Node* last_node_ = node_;

    do {
        const Node* node_copy_ = node_;

        if (!was_right_turn && node_->right_child != nullptr && node_->right_child != last_node_) {
            node_ = node_->right_child;
            path_.code |= (size_t(1) << path_.length);
            ++path_.length;
            was_right_turn = true;
        } else if (was_right_turn && node_->left_child != nullptr && node_->left_child != last_node_) {
            node_ = node_->left_child;
            ++path_.length;
        } else {
            node_ = node_->ancestor;

            if (path_.length != 0 && (path_.code & (size_t(1) << (path_.length - 1)))) {
                path_.code ^= (size_t(1) << (path_.length - 1));
            }

            if (path_.length != 0) {
                --path_.length;
            }
        }

        last_node_ = node_copy_;
    } while (node_ != nullptr && node_->value == nullptr);
}

template <class T>
const T& BinaryTrie<T>::Iterator::operator*() const {
    if (node_ == nullptr) {
        throw std::runtime_error("BINARY_TRIE::ITERATOR::OPERATOR*: End iterator can't be dereferenced");
    }

    if (node_->value == nullptr) {
        throw std::runtime_error("BINARY_TRIE::ITERATOR::OPERATOR*: Attempt to dereference iterator without value");
    }

    return *node_->value;
}

template <class T>
bool BinaryTrie<T>::Iterator::operator!=(const BinaryTrie::Iterator& o) const {
    return node_ != o.node_;
}

template <class T>
bool BinaryTrie<T>::Iterator::operator==(const BinaryTrie::Iterator& o) const {
    return node_ == o.node_;
}

template <class T>
typename BinaryTrie<T>::BinaryPath BinaryTrie<T>::Iterator::GetPath() const {
    return path_;
}

template <class T>
BinaryTrie<T>::Traverser::Traverser(const BinaryTrie::Node* node, const BinaryTrie::BinaryPath& path)
    : node_(node), path_(path) {
}

template <class T>
const typename BinaryTrie<T>::BinaryPath& BinaryTrie<T>::Traverser::operator*() const {
    return path_;
}

template <class T>
bool BinaryTrie<T>::Traverser::HasValue() const {
    return node_ != nullptr && node_->value != nullptr;
}
template <class T>
const T& BinaryTrie<T>::Traverser::GetValue() const {
    if (node_ == nullptr || node_->value == nullptr) {
        throw std::runtime_error("BINARY_TRIE::TRAVERSER::GET_VALUE: Attempt to get empty value");
    }

    return *node_->value;
}
template <class T>
bool BinaryTrie<T>::Traverser::CanGoLeft() const {
    return node_->left_child != nullptr;
}
template <class T>
bool BinaryTrie<T>::Traverser::CanGoRight() const {
    return node_->right_child != nullptr;
}
template <class T>
void BinaryTrie<T>::Traverser::GoLeft() {
    if (node_ != nullptr) {
        node_ = node_->left_child;
        ++path_.length;
    }
}
template <class T>
void BinaryTrie<T>::Traverser::GoRight() {
    if (node_ != nullptr) {
        node_ = node_->right_child;
        path_.code |= (size_t(1) << path_.length);
        ++path_.length;
    }
}