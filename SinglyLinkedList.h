#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>

template <typename Type>
class SinglyLinkedList {
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value{};
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator {
        friend class SinglyLinkedList;

        explicit BasicIterator(Node* node)
            : node_(node) {
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept
            : node_(other.node_) {
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return this->node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return this->node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            auto this_copy(*this);
            ++(*this);
            return this_copy;
        }

        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_ != nullptr);
            return &node_->value;
        }

    private:
        Node* node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    SinglyLinkedList() = default;

    SinglyLinkedList(std::initializer_list<Type> values) {
        Assign(values.begin(), values.end());
    }

    SinglyLinkedList(const SinglyLinkedList& other){
    	Assign(other.begin(), other.end());
    }

    SinglyLinkedList& operator=(const SinglyLinkedList& rhs) {
        if (this != &rhs){
        	auto tmp(rhs);
        	swap(tmp);
        }
        return *this;
    }

    ~SinglyLinkedList() {
        Clear();
    }

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{head_.next_node};
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator{nullptr};
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return cbegin();
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return cend();
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator{head_.next_node};
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator{nullptr};
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator{&head_};
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator{const_cast<Node*>(&head_)};
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return cbefore_begin();
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return GetSize() == 0;
    }

    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);

        ++size_;
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        auto& prev_node = pos.node_;
        assert(prev_node);

        prev_node->next_node = new Node(value, prev_node->next_node);

        ++size_;
        return Iterator{prev_node->next_node};
    }

    void PopFront() noexcept {
        assert(!IsEmpty());

        Node* tmp = head_.next_node->next_node;
        delete head_.next_node;
        head_.next_node = tmp;
        --size_;
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert(!IsEmpty());

        Node* const node_to_erase = pos.node_->next_node;
        Node* const node_after_erased = node_to_erase->next_node;

        pos.node_->next_node = node_after_erased;

        delete node_to_erase;
        --size_;

        return Iterator{node_after_erased};
    }

    void Clear() noexcept {
        while (head_.next_node != nullptr) {
            Node* tmp = head_.next_node->next_node;
            delete head_.next_node;
            head_.next_node = tmp;
        }
        size_ = 0;
    }

    void swap(SinglyLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

private:
    template <typename InputIterator>
    void Assign(InputIterator from, InputIterator to) {
    	SinglyLinkedList<Type> tmp;

        Node** node_ptr = &tmp.head_.next_node;
        while (from != to) {
            assert(*node_ptr == nullptr);

            *node_ptr = new Node(*from, nullptr);
            ++tmp.size_;

            node_ptr = &((*node_ptr)->next_node);

            ++from;
        }

        swap(tmp);
    }

    Node head_;
    size_t size_ = 0;
};

template <typename Type>
void swap(SinglyLinkedList<Type>& lhs, SinglyLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SinglyLinkedList<Type>& lhs, const SinglyLinkedList<Type>& rhs) {
    return (&lhs == &rhs)
        || (lhs.GetSize() == rhs.GetSize()
            && std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}

template <typename Type>
bool operator!=(const SinglyLinkedList<Type>& lhs, const SinglyLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SinglyLinkedList<Type>& lhs, const SinglyLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SinglyLinkedList<Type>& lhs, const SinglyLinkedList<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SinglyLinkedList<Type>& lhs, const SinglyLinkedList<Type>& rhs) {
    return (rhs < lhs);
}

template <typename Type>
bool operator>=(const SinglyLinkedList<Type>& lhs, const SinglyLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}
