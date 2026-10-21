#include <gtest/gtest.h>
#include "linalg.h"
#include <stdexcept>
#include <utility>

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
TEST(Constructors, SingleArgumentIsColumn)
{
    linalg::Matrix m(4);
    EXPECT_EQ(m.rows(), 4u);
    EXPECT_EQ(m.columns(), 1u);
}

TEST(Constructors, InitializerLists)
{
    linalg::Matrix a = { {1, 2, 3}, {4, 5, 6} };
    EXPECT_EQ(a.rows(), 2u);
    EXPECT_EQ(a.columns(), 3u);
    EXPECT_EQ(a(1, 0), 4.0);

    linalg::Matrix b = { {1, 2, 3, 4, 5, 6} };
    EXPECT_EQ(b.rows(), 1u);
    EXPECT_EQ(b.columns(), 6u);

    linalg::Matrix c = { 1, 2, 3, 4, 5, 6 };
    EXPECT_EQ(c.rows(), 6u);
    EXPECT_EQ(c.columns(), 1u);
}

TEST(Constructors, RaggedListThrows)
{
    EXPECT_THROW((linalg::Matrix{ {1, 2, 3}, {4, 5} }), std::runtime_error);
}

TEST(Constructors, CopyIsIndependent)
{
    linalg::Matrix a = { {1, 2}, {3, 4} };
    linalg::Matrix b(a);
    a(0, 0) = 100;
    EXPECT_EQ(b(0, 0), 1.0);
}

TEST(Constructors, MoveLeavesSourceEmpty)
{
    linalg::Matrix a = { {1, 2}, {3, 4} };
    linalg::Matrix b(std::move(a));
    EXPECT_EQ(b.rows(), 2u);
    EXPECT_TRUE(a.empty());
    EXPECT_EQ(a.capacity(), 0u);
}

TEST(Assignment, CopyReusesMemoryIfCapacityEnough)
{
    linalg::Matrix a(4, 6);
    linalg::Matrix b = { {1, 2, 3}, {4, 5, 6} };
    const double* before = a.begin();
    a = b;
    EXPECT_EQ(a.begin(), before); // память не перевыделялась
    EXPECT_EQ(a.capacity(), 24u);
    EXPECT_TRUE(a == b);
}

TEST(Assignment, CopyReallocatesIfNeeded)
{
    linalg::Matrix a;
    linalg::Matrix b = { {1, 2}, {3, 4} };
    a = b;
    EXPECT_EQ(a.capacity(), 4u);
    EXPECT_TRUE(a == b);
}

TEST(Assignment, SelfAssignment)
{
    linalg::Matrix a = { {1, 2}, {3, 4} };
    auto& ref = a;
    a = ref;
    EXPECT_EQ(a(1, 1), 4.0);
}

TEST(Assignment, Move)
{
    linalg::Matrix a(2, 2);
    a = linalg::Matrix{ 1, 2, 3, 4, 5, 6 };
    EXPECT_EQ(a.rows(), 6u);
    EXPECT_EQ(a.columns(), 1u);
}

TEST(Access, ReadWrite)
{
    linalg::Matrix m = { {1.0, 2.0, 3.0}, {4.0, 5.0, 6.0} };
    EXPECT_EQ(m(0, 2), 3.0);
    m(0, 2) = 7.0;
    EXPECT_EQ(m(0, 2), 7.0);
    const linalg::Matrix& c = m;
    EXPECT_EQ(c(1, 0), 4.0);
}

TEST(Access, RangeFor)
{
    linalg::Matrix m = { {1, 2}, {3, 4} };
    double sum = 0;
    for (double x : m) sum += x;
    EXPECT_EQ(sum, 10.0);
}
TEST(Capacity, ReserveAndClear)
{
    linalg::Matrix m = { {1, 2}, {3, 4} };
    m.reserve(100);
    EXPECT_EQ(m.capacity(), 100u);
    EXPECT_EQ(m(1, 1), 4.0); // элементы сохранились
    m.reserve(10);
    EXPECT_EQ(m.capacity(), 100u); // не уменьшается
    m.clear();
    EXPECT_TRUE(m.empty());
    EXPECT_EQ(m.capacity(), 100u);
}

TEST(Capacity, ReshapeWithinCapacityKeepsMemory)
{
    linalg::Matrix m(4, 6);
    const double* before = m.begin();
    m.reshape(2, 3);
    EXPECT_EQ(m.begin(), before);
    EXPECT_EQ(m.capacity(), 24u);
    m.reshape(10, 10);
    EXPECT_EQ(m.capacity(), 100u);
}

TEST(Capacity, ShrinkToFit)
{
    linalg::Matrix m(4, 6);
    m.reshape(2, 2);
    m.shrink_to_fit();
    EXPECT_EQ(m.capacity(), 4u);
    m.clear();
    m.shrink_to_fit();
    EXPECT_EQ(m.capacity(), 0u);
}

TEST(Capacity, OverflowThrows)
{
    EXPECT_THROW(linalg::Matrix(static_cast<std::size_t>(-1), 2), std::runtime_error);
}

TEST(Swap, MethodAndFunction)
{
    linalg::Matrix a = { {1, 2}, {3, 4} };
    linalg::Matrix b = { 1, 2, 3 };
    a.swap(b);
    EXPECT_EQ(a.rows(), 3u);
    EXPECT_EQ(b.rows(), 2u);
    swap(a, b);
    EXPECT_EQ(a.rows(), 2u);
}