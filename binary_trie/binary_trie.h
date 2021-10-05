#pragma once
#include <vector>
#include <stdexcept>

template <class T>
class BinaryTrie {
    class Node;

public:
    struct BinaryPath {
        bool operator<(const BinaryPath& o);

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
    class Node {
    public:
        explicit Node(const T& value);
        Node(const Node& o) = delete;
        Node& operator=(const Node& o) = delete;
        Node() = default;
        ~Node();

        void SetLeftChild(Node* node);
        void SetRightChild(Node* node);
        void SetAncestor(Node* node);

        Node* GetLeftChild();
        Node* GetRightChild();
        Node* GetAncestor();

        const Node* GetLeftChild() const;
        const Node* GetRightChild() const;
        const Node* GetAncestor() const;
        const T& GetValue() const;

        bool HasValue() const;
        bool IsTerminal() const;

    private:
        T* value_ = nullptr;
        Node* children_[2] = {nullptr};
        Node* ancestor_ = nullptr;
    };

public:
    explicit BinaryTrie(const T& root_value);
    BinaryTrie() = default;
    BinaryTrie(const BinaryTrie& o) = delete;
    BinaryTrie& operator=(const BinaryTrie& o) = delete;
    BinaryTrie(BinaryTrie&& o) = default;
    BinaryTrie& operator=(BinaryTrie&& o) = default;
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
const typename BinaryTrie<T>::BinaryPath& BinaryTrie<T>::Traverser::operator*() const {
    return path_;
}

template <class T>
bool BinaryTrie<T>::Traverser::HasValue() const {
    return node_ != nullptr && node_->HasValue();
}
template <class T>
const T& BinaryTrie<T>::Traverser::GetValue() const {
    return node_->GetValue();
}
template <class T>
bool BinaryTrie<T>::Traverser::CanGoLeft() const {
    return node_->GetLeftChild() != nullptr;
}
template <class T>
bool BinaryTrie<T>::Traverser::CanGoRight() const {
    return node_->GetRightChild() != nullptr;
}
template <class T>
void BinaryTrie<T>::Traverser::GoLeft() {
    if (node_ != nullptr) {
        node_ = node_->GetLeftChild();
        ++path_.length;
    }
}
template <class T>
void BinaryTrie<T>::Traverser::GoRight() {
    if (node_ != nullptr) {
        node_ = node_->GetRightChild();
        path_.code |= (size_t(1) << path_.length);
        ++path_.length;
    }
}
template <class T>
BinaryTrie<T>::Traverser::Traverser(const BinaryTrie::Node* node, const BinaryTrie::BinaryPath& path)
    : node_(node), path_(path) {
}

template <class T>
BinaryTrie<T>::BinaryTrie(const T& root_value) {
    begin_node_ = root_ = new Node(root_value);
}

template <class T>
BinaryTrie<T>::~BinaryTrie() {
    delete root_;
}

template <class T>
void BinaryTrie<T>::Insert(const T& value, BinaryPath path) {
    if (root_ == nullptr) {
        root_ = new Node();
    }

    Node* current_node = root_;

    for (size_t i = 0; i < path.length; ++i) {
        Node* next_node = current_node->GetLeftChild();
        bool right_child = ((path.code >> i) & 1);

        if (right_child) {
            next_node = current_node->GetRightChild();
        }

        if (next_node == nullptr) {
            if (i + 1 == path.length) {
                next_node = new Node(value);
            } else {
                next_node = new Node();
            }

            if (right_child) {
                current_node->SetRightChild(next_node);
            } else {
                current_node->SetLeftChild(next_node);
            }

            current_node->SetAncestor(current_node);
        }

        current_node = next_node;
    }
}

template <class T>
void BinaryTrie<T>::Merge(BinaryTrie&& o) {
    if (this == &o) {
        throw std::invalid_argument("BINARY_TRIE: trie can't be merged with itself");
    }

    if (o.begin() == o.end()) {
        return;
    }

    if (root_ == nullptr) {
        (*this) = std::move(o);
        return;
    }

    Node* new_root = new Node();

    new_root->SetLeftChild(root_);
    new_root->SetRightChild(o.root_);

    root_ = new_root;
    o.root_ = nullptr;

    root_->GetLeftChild()->SetAncestor(root_);
    root_->GetRightChild()->SetAncestor(root_);

    ++begin_path_.length;
    begin_path_.code <<= 1;
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
const T& BinaryTrie<T>::Iterator::operator*() const {
    if (node_ == nullptr) {
        throw std::runtime_error("BINARY_TRIE::ITERATOR: end() iterator can't be dereferenced");
    }

    if (!node_->HasValue()) {
        throw std::runtime_error("BINARY_TRIE::ITERATOR: attempt to dereference iterator without value");
    }

    return node_->GetValue();
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

        if (!was_right_turn && node_->GetRightChild() != nullptr && node_->GetRightChild() != last_node_) {
            node_ = node_->GetRightChild();
            path_.code |= (size_t(1) << path_.length);
            ++path_.length;
            was_right_turn = true;
        } else if (was_right_turn && node_->GetLeftChild() != nullptr && node_->GetLeftChild() != last_node_) {
            node_ = node_->GetLeftChild();
            ++path_.length;
        } else {
            node_ = node_->GetAncestor();

            if (path_.length != 0 && (path_.code & (size_t(1) << (path_.length - 1)))) {
                path_.code ^= (size_t(1) << (path_.length - 1));
            }

            if (path_.length != 0) {
                --path_.length;
            }
        }

        last_node_ = node_copy_;
    } while (node_ != nullptr && !node_->HasValue());
}

template <class T>
BinaryTrie<T>::Iterator::Iterator(const BinaryTrie::Node* node, const BinaryPath& path) : node_(node), path_(path) {
}

template <class T>
bool BinaryTrie<T>::Iterator::operator!=(const BinaryTrie::Iterator& o) const {
    return node_ != o.node_;
}

template <class T>
typename BinaryTrie<T>::BinaryPath BinaryTrie<T>::Iterator::GetPath() const {
    return path_;
}
template <class T>
bool BinaryTrie<T>::Iterator::operator==(const BinaryTrie::Iterator& o) const {
    return node_ == o.node_;
}

template <class T>
BinaryTrie<T>::Node::Node(const T& value) {
    this->value_ = new T(value);
}

template <class T>
BinaryTrie<T>::Node::~Node() {
    delete value_;

    for (auto& child : children_) {
        delete child;
    }
}

template <class T>
bool BinaryTrie<T>::Node::HasValue() const {
    return value_ != nullptr;
}

template <class T>
const T& BinaryTrie<T>::Node::GetValue() const {
    if (value_ == nullptr) {
        throw std::runtime_error("BINARY_TRIE::NODE: attempt to get value out of empty node");
    }

    return *value_;
}
template <class T>
void BinaryTrie<T>::Node::SetLeftChild(BinaryTrie::Node* node) {
    children_[0] = node;
}
template <class T>
void BinaryTrie<T>::Node::SetRightChild(BinaryTrie::Node* node) {
    children_[1] = node;
}

template <class T>
void BinaryTrie<T>::Node::SetAncestor(BinaryTrie::Node* node) {
    ancestor_ = node;
}

template <class T>
typename BinaryTrie<T>::Node* BinaryTrie<T>::Node::GetLeftChild() {
    return children_[0];
}

template <class T>
typename BinaryTrie<T>::Node* BinaryTrie<T>::Node::GetRightChild() {
    return children_[1];
}

template <class T>
typename BinaryTrie<T>::Node* BinaryTrie<T>::Node::GetAncestor() {
    return ancestor_;
}

template <class T>
const typename BinaryTrie<T>::Node* BinaryTrie<T>::Node::GetLeftChild() const {
    return children_[0];
}

template <class T>
const typename BinaryTrie<T>::Node* BinaryTrie<T>::Node::GetRightChild() const {
    return children_[1];
}

template <class T>
const typename BinaryTrie<T>::Node* BinaryTrie<T>::Node::GetAncestor() const {
    return ancestor_;
}

template <class T>
bool BinaryTrie<T>::Node::IsTerminal() const {
    return children_[0] == nullptr && children_[1] == nullptr;
}

template <class T>
bool BinaryTrie<T>::BinaryPath::operator<(const BinaryPath& o) {
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