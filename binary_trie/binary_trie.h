#pragma once
#include <vector>
#include <stdexcept>

template <class T>
class BinaryTrie {
    class Node;

public:
    struct BinaryPath {
        size_t code;
        size_t length;
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
    BinaryTrie(const BinaryTrie& o) = delete;  // TODO: add copying
    BinaryTrie& operator=(const BinaryTrie& o) = delete;
    BinaryTrie(BinaryTrie&& o) noexcept;
    BinaryTrie& operator=(BinaryTrie&& o) noexcept;
    ~BinaryTrie();

    void Merge(BinaryTrie&& o);

    Iterator begin() const;
    Iterator end() const;

    Traverser GetRootTraverser() const;

private:
    Node* root_ = nullptr;
    Node* begin_node_ = nullptr;
    size_t begin_height_ = 0;
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
BinaryTrie<T>::BinaryTrie(BinaryTrie&& o) noexcept
    : root_(o.root_), begin_node_(o.begin_node_), begin_height_(o.begin_height_) {
    o.root_ = nullptr;
}

template <class T>
BinaryTrie<T>::~BinaryTrie() {
    delete root_;
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
    o.begin_node_ = nullptr;

    root_->GetLeftChild()->SetAncestor(root_);
    root_->GetRightChild()->SetAncestor(root_);

    ++begin_height_;
}

template <class T>
typename BinaryTrie<T>::Iterator BinaryTrie<T>::begin() const {
    return Iterator(begin_node_, BinaryPath{.length = begin_height_});
}

template <class T>
typename BinaryTrie<T>::Iterator BinaryTrie<T>::end() const {
    return BinaryTrie::Iterator(nullptr, {});
}
template <class T>
BinaryTrie<T>& BinaryTrie<T>::operator=(BinaryTrie&& o) noexcept {
    this->root_ = o.root_;
    this->begin_node_ = o.begin_node_;
    this->begin_height_ = o.begin_height_;
    o.root_ = nullptr;

    return *this;
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
