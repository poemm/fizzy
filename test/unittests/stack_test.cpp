#include "stack.hpp"
#include <gtest/gtest.h>

using namespace fizzy;

TEST(stack, push_and_pop)
{
    Stack<char> stack;

    EXPECT_EQ(stack.size(), 0);
    EXPECT_TRUE(stack.empty());

    stack.push('a');
    stack.push('b');
    stack.push('c');

    EXPECT_EQ(stack.size(), 3);

    EXPECT_EQ(stack.pop(), 'c');
    EXPECT_EQ(stack.pop(), 'b');
    EXPECT_EQ(stack.pop(), 'a');

    EXPECT_EQ(stack.size(), 0);
    EXPECT_TRUE(stack.empty());
}

TEST(stack, drop_and_peek)
{
    Stack<char> stack;
    stack.push('w');
    stack.push('x');
    stack.push('y');
    stack.push('z');

    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.size(), 4);
    EXPECT_EQ(stack.top(), 'z');
    EXPECT_EQ(stack[0], 'z');
    EXPECT_EQ(stack[1], 'y');
    EXPECT_EQ(stack[2], 'x');
    EXPECT_EQ(stack[3], 'w');
    EXPECT_EQ(stack.size(), 4);

    stack.drop();
    EXPECT_EQ(stack.size(), 3);
    EXPECT_EQ(stack.top(), 'y');

    stack.drop(2);
    EXPECT_EQ(stack.size(), 1);
    EXPECT_EQ(stack.top(), 'w');

    stack.drop();
    EXPECT_EQ(stack.size(), 0);
    EXPECT_TRUE(stack.empty());
}

TEST(stack, clear)
{
    Stack<char> stack;
    stack.push('a');
    stack.push('b');

    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.size(), 2);

    stack.drop(0);
    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.size(), 2);

    stack.clear();
    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(stack.size(), 0);
}

TEST(stack, resize)
{
    Stack<char> stack;
    stack.push('a');
    stack.push('b');

    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.size(), 2);

    // grow stack
    stack.resize(4);
    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.top(), 0);
    EXPECT_EQ(stack[1], 0);
    EXPECT_EQ(stack[2], 'b');
    EXPECT_EQ(stack[3], 'a');
    EXPECT_EQ(stack.size(), 4);

    stack.drop();
    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.size(), 3);

    // shrink stack
    stack.resize(1);
    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.size(), 1);

    stack.clear();
    EXPECT_TRUE(stack.empty());
}

TEST(stack, iterator)
{
    Stack<char> stack;
    stack.push('a');
    stack.push('b');
    stack.push('c');

    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.size(), 3);

    auto bottom_item = stack.begin();
    EXPECT_EQ(*bottom_item, 'a');

    auto top_item = stack.end() - 1;
    EXPECT_EQ(*top_item, 'c');

    for (auto it = stack.begin(); it < stack.end(); it++)
        *it = 'x';

    for (unsigned item = 0; item < stack.size(); item++)
        EXPECT_EQ(stack[item], 'x');
}

TEST(stack, clear_on_empty)
{
    Stack<char> stack;
    stack.clear();
}

TEST(operand_stack, construct)
{
    OperandStack stack(0);
    EXPECT_EQ(stack.size(), 0);
    stack.shrink(0);
    EXPECT_EQ(stack.size(), 0);
}

TEST(operand_stack, top)
{
    OperandStack stack(1);
    EXPECT_EQ(stack.size(), 0);

    stack.push(1);
    EXPECT_EQ(stack.size(), 1);
    EXPECT_EQ(stack.top(), 1);
    EXPECT_EQ(stack[0], 1);

    stack.top() = 101;
    EXPECT_EQ(stack.size(), 1);
    EXPECT_EQ(stack.top(), 101);
    EXPECT_EQ(stack[0], 101);

    stack.shrink(0);
    EXPECT_EQ(stack.size(), 0);

    stack.push(2);
    EXPECT_EQ(stack.size(), 1);
    EXPECT_EQ(stack.top(), 2);
    EXPECT_EQ(stack[0], 2);
}

TEST(operand_stack, small)
{
    OperandStack stack(3);
    EXPECT_EQ(stack.size(), 0);

    stack.push(1);
    stack.push(2);
    stack.push(3);
    EXPECT_EQ(stack.size(), 3);
    EXPECT_EQ(stack.top(), 3);
    EXPECT_EQ(stack[0], 3);
    EXPECT_EQ(stack[1], 2);
    EXPECT_EQ(stack[2], 1);

    stack[0] = 13;
    stack[1] = 12;
    stack[2] = 11;
    EXPECT_EQ(stack.size(), 3);
    EXPECT_EQ(stack.top(), 13);
    EXPECT_EQ(stack[0], 13);
    EXPECT_EQ(stack[1], 12);
    EXPECT_EQ(stack[2], 11);

    EXPECT_EQ(stack.pop(), 13);
    EXPECT_EQ(stack.size(), 2);
    EXPECT_EQ(stack.top(), 12);
}

TEST(operand_stack, large)
{
    constexpr auto max_height = 33;
    OperandStack stack(max_height);

    for (unsigned i = 0; i < max_height; ++i)
        stack.push(i);

    EXPECT_EQ(stack.size(), max_height);
    for (int expected = max_height - 1; expected >= 0; --expected)
        EXPECT_EQ(stack.pop(), expected);
    EXPECT_EQ(stack.size(), 0);
}

TEST(operand_stack, shrink)
{
    constexpr auto max_height = 60;
    OperandStack stack(max_height);

    for (unsigned i = 0; i < max_height; ++i)
        stack.push(i);

    EXPECT_EQ(stack.size(), max_height);
    constexpr auto new_height = max_height / 3;
    stack.shrink(new_height);
    EXPECT_EQ(stack.size(), new_height);
    EXPECT_EQ(stack.top(), new_height - 1);
    EXPECT_EQ(stack[0], new_height - 1);
    EXPECT_EQ(stack[new_height - 1], 0);
}

TEST(operand_stack, rbegin_rend)
{
    OperandStack stack(3);
    EXPECT_EQ(stack.rbegin(), stack.rend());

    stack.push(1);
    stack.push(2);
    stack.push(3);
    EXPECT_NE(stack.rbegin(), stack.rend());
    EXPECT_EQ(*stack.rbegin(), 1);
    EXPECT_EQ(*(stack.rend() - 1), 3);
}

TEST(operand_stack, to_vector)
{
    OperandStack stack(3);
    EXPECT_TRUE(std::vector(stack.rbegin(), stack.rend()).empty());

    stack.push(1);
    stack.push(2);
    stack.push(3);

    EXPECT_EQ(std::vector(stack.rbegin(), stack.rend()), (std::vector<uint64_t>{1, 2, 3}));
}
