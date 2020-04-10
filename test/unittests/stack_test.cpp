#include "stack.hpp"
#include <gtest/gtest.h>

using namespace fizzy;

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
