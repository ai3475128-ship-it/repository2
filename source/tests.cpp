#include <gtest/gtest.h>
#include "linalg.h"

TEST(Constructors, Default)
{
    linalg::Matrix m;
    EXPECT_TRUE(m.empty());
    EXPECT_EQ(m.rows(), 0u);
    EXPECT_EQ(m.columns(), 0u);
}

TEST(Constructors, SizeOnly)
{
    linalg::Matrix m(4, 6);
    EXPECT_EQ(m.rows(), 4u);
    EXPECT_EQ(m.columns(), 6u);
    EXPECT_EQ(m.size(), 24u);
    EXPECT_EQ(m(3, 5), 0.0); // элементы заполнены нулями
}