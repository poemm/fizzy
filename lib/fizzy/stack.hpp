#pragma once

#include <cassert>
#include <cstdint>
#include <memory>
#include <vector>

namespace fizzy
{
template <typename T>
class Stack : public std::vector<T>
{
public:
    using difference_type = typename std::vector<T>::difference_type;

    using std::vector<T>::vector;

    using std::vector<T>::back;
    using std::vector<T>::emplace_back;
    using std::vector<T>::pop_back;
    using std::vector<T>::resize;
    using std::vector<T>::size;

    // Also used: size(), resize(), clear(), empty(), end()

    void push(T val) { emplace_back(val); }

    T pop()
    {
        const auto res = back();
        pop_back();
        return res;
    }

    T& operator[](size_t index) noexcept { return std::vector<T>::operator[](size() - index - 1); }

    T& top() noexcept { return (*this)[0]; }

    /// Drops @a num_elements elements from the top of the stack.
    void drop(size_t num_elements = 1) noexcept { resize(size() - num_elements); }

    void shrink(size_t new_size) noexcept { resize(new_size); }
};

class OperandStack
{
    static constexpr auto small_storage_size = 256 / sizeof(uint64_t);

    /// The pointer to the top item, or below the stack bottom if stack is empty.
    uint64_t* m_top;

    uint64_t* m_bottom;

    uint64_t* m_locals;

    uint64_t m_small_storage[small_storage_size];

    /// The unbounded storage for items.
    std::unique_ptr<uint64_t[]> m_large_storage;

public:
    /// Default constructor. Sets the top item pointer to below the stack bottom.
    explicit OperandStack(size_t num_locals, size_t max_stack_height)
    {
        const auto storage_size_required = num_locals + max_stack_height;

        uint64_t* storage;
        if (storage_size_required <= small_storage_size)
        {
            storage = &m_small_storage[0];
        }
        else
        {
            m_large_storage = std::make_unique<uint64_t[]>(storage_size_required);
            storage = &m_large_storage[0];
        }

        m_locals = storage;
        m_bottom = m_locals + num_locals;
        m_top = m_bottom - 1;
    }

    OperandStack(const OperandStack&) = delete;
    OperandStack& operator=(const OperandStack&) = delete;

    uint64_t& local(size_t index) noexcept { return m_locals[index]; }

    /// The current number of items on the stack (aka stack height).
    size_t size() noexcept { return static_cast<size_t>(m_top + 1 - m_bottom); }

    /// Returns the reference to the top item.
    /// Requires non-empty stack.
    auto& top() noexcept
    {
        assert(size() != 0);
        return *m_top;
    }

    /// Returns the reference to the stack item on given position from the stack top.
    /// Requires index < size().
    auto& operator[](size_t index) noexcept
    {
        assert(index < size());
        return *(m_top - index);
    }

    /// Pushes an item on the stack.
    /// The stack max height limit is not checked.
    void push(uint64_t item) noexcept { *++m_top = item; }

    /// Returns an item popped from the top of the stack.
    /// Requires non-empty stack.
    auto pop() noexcept
    {
        assert(size() != 0);
        return *m_top--;
    }

    /// Shrinks the stack to the given size by dropping item from the top.
    /// Requires size <= size();
    [[gnu::no_sanitize("pointer-overflow"), clang::no_sanitize("pointer-overflow")]] void shrink(
        size_t size) noexcept
    {
        assert(size <= this->size());
        // For size == 0, the m_top will point below the storage.
        m_top = m_bottom + (size - 1);
    }
};
}  // namespace fizzy
