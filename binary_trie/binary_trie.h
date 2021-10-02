#pragma once
#include <vector>
#include <stdexcept>

// Yeah, I don't use smart pointers, because I think it's really inefficient
// here and furthermore these classes are simple enough to handle memory freeing by myself

template<class T>
class BinaryTrie {
    friend class Iterator;
    friend class Node;
    class Node;
public:
    class Iterator {
        friend BinaryTrie;
    public:
        const T& operator*() const;
        bool operator!=(const Iterator& o) const;
        void operator++();

    private:
        Iterator(const BinaryTrie::Node* node);

    private:
        const BinaryTrie::Node* node_ = nullptr;
    };

private:
    class Node {
        friend Iterator;
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
        Node* children_[2] = { nullptr };
        Node* ancestor_ = nullptr;
    };

public:
    explicit BinaryTrie(const T& root_value);
    BinaryTrie(const BinaryTrie& o) = delete; // TODO: add copying
    BinaryTrie& operator=(const BinaryTrie& o) = delete;
    BinaryTrie(BinaryTrie&& o) noexcept;
    ~BinaryTrie();

    void Merge(BinaryTrie&& o);

    Iterator begin() const;
    Iterator end() const;

private:
    Node* root_ = nullptr;
    Node* begin_ = nullptr;
};

template <class T>
BinaryTrie<T>::BinaryTrie(const T& root_value) {
    begin_ = root_ = new Node(root_value);
}

template <class T>
BinaryTrie<T>::BinaryTrie(BinaryTrie&& o) noexcept : begin_(o.begin_), root_(o.root_) {
    o.root_ = nullptr;
    o.begin_ = nullptr;
}

template <class T>
BinaryTrie<T>::~BinaryTrie() {
    delete root_;
}

template <class T>
void BinaryTrie<T>::Merge(BinaryTrie&& o) {
    if(this == &o) {
        throw std::invalid_argument("BINARY_TRIE: trie can't be merged with itself");
    }

    Node* new_root = new Node();

    new_root->SetLeftChild(root_);
    new_root->SetRightChild(o.root_);

    root_ = new_root;
    o.root_ = nullptr;
    o.begin_ = nullptr;

    root_->GetLeftChild()->SetAncestor(root_);
    root_->GetRightChild()->SetAncestor(root_);
}

template <class T>
typename BinaryTrie<T>::Iterator BinaryTrie<T>::begin() const {
    return Iterator(begin_);
}

template <class T>
typename BinaryTrie<T>::Iterator BinaryTrie<T>::end() const {
    return BinaryTrie::Iterator(nullptr);
}

template <class T>
const T& BinaryTrie<T>::Iterator::operator*() const {
    if(node_ == nullptr) {
        throw std::runtime_error("BINARY_TRIE::ITERATOR: end() iterator can't be dereferenced");
    }

    if(!node_->HasValue()) {
        throw std::runtime_error("BINARY_TRIE::ITERATOR: attempt to dereference iterator without value");
    }

    return node_->GetValue();
}

template <class T>
void BinaryTrie<T>::Iterator::operator++() {
    if(node_ == nullptr) {
        return;
    }

    bool was_right_turn = false;
    const Node* last_node_ = node_;

    do {
        const Node* node_copy_ = node_;

        if(!was_right_turn && node_->GetRightChild() != nullptr && node_->GetRightChild() != last_node_) {
            node_ = node_->GetRightChild();
            was_right_turn = true;
        } else if(was_right_turn && node_->GetLeftChild() != nullptr && node_->GetLeftChild() != last_node_) {
            node_ = node_->GetLeftChild();
        } else {
            node_ = node_->GetAncestor();
        }

        last_node_ = node_copy_;
    } while(node_ != nullptr && !node_->HasValue());
}

template <class T>
BinaryTrie<T>::Iterator::Iterator(const BinaryTrie::Node* node) : node_(node) {
}

template <class T>
bool BinaryTrie<T>::Iterator::operator!=(const BinaryTrie::Iterator& o) const {
    return node_ != o.node_;
}

template <class T>
BinaryTrie<T>::Node::Node(const T& value) {
    this->value_ = new T(value);
}

template <class T>
BinaryTrie<T>::Node::~Node() {
    delete value_;

    for(auto &child : children_) {
        delete child;
    }
}

template <class T>
bool BinaryTrie<T>::Node::HasValue() const {
    return value_ != nullptr;
}

template <class T>
const T& BinaryTrie<T>::Node::GetValue() const {
    if(value_ == nullptr) {
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
