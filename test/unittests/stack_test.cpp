#include "stack.hpp"
#include "types.hpp"
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
    Stack<char> stack{'w', 'x', 'y', 'z'};

    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.size(), 4);
    EXPECT_EQ(stack.peek(), 'z');
    EXPECT_EQ(stack.peek(1), 'y');
    EXPECT_EQ(stack.peek(2), 'x');
    EXPECT_EQ(stack.peek(3), 'w');
    EXPECT_EQ(stack.size(), 4);

    stack.drop();
    EXPECT_EQ(stack.size(), 3);
    EXPECT_EQ(stack.peek(), 'y');

    stack.drop(2);
    EXPECT_EQ(stack.size(), 1);
    EXPECT_EQ(stack.peek(), 'w');

    stack.drop();
    EXPECT_EQ(stack.size(), 0);
    EXPECT_TRUE(stack.empty());
}

TEST(asan, string)
{
    std::string s;
    s.resize(100);
    EXPECT_NE(s[100], 1);
}

TEST(asan, bytes)
{
    fizzy::bytes s;
    s.resize(100);
    EXPECT_NE(s[100], 1);
}

TEST(asan, vector)
{
    std::vector<char> s;
    const auto e = 'x';
    s.reserve(2);
    EXPECT_GE(s.capacity(), 2);
    EXPECT_EQ(s.size(), 0);
    s.push_back(e);
    EXPECT_EQ(s[0], e);

    EXPECT_NE(s[1], e);
}
