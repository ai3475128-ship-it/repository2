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
TEST(Output, AlignedByColumns)
{
    linalg::Matrix m = { {1, 333}, {22, 1} };
    std::ostringstream ss;
    ss << m;
    EXPECT_EQ(ss.str(), "| 1 333|\n|22   1|\n");
}

TEST(Arithmetic, UnaryOperators)
{
    linalg::Matrix a = { {1, -2}, {3, 4} };
    EXPECT_TRUE(+a == a);
    EXPECT_TRUE(-a == (linalg::Matrix{ {-1, 2}, {-3, -4} }));
}

TEST(Arithmetic, AddSubtract)
{
    linalg::Matrix a = { {1, 2}, {3, 4} };
    linalg::Matrix b = { {5, 6}, {7, 8} };
    EXPECT_TRUE(a + b == (linalg::Matrix{ {6, 8}, {10, 12} }));
    EXPECT_TRUE(b - a == (linalg::Matrix{ {4, 4}, {4, 4} }));
    linalg::Matrix c = a;
    c += b;
    c -= b;
    EXPECT_TRUE(c == a);
}

TEST(Arithmetic, MultiplyByNumber)
{
    linalg::Matrix a = { {1, 2}, {3, 4} };
    EXPECT_TRUE(2 * a == a * 2);
    EXPECT_TRUE(a * 2 == (linalg::Matrix{ {2, 4}, {6, 8} }));
    a *= 0.5;
    EXPECT_EQ(a(1, 1), 2.0);
}

TEST(Arithmetic, MatrixMultiply)
{
    linalg::Matrix a = { {1, 2}, {3, 4} };
    linalg::Matrix b = { {5, 6}, {7, 8} };
    EXPECT_TRUE(a * b == (linalg::Matrix{ {19, 22}, {43, 50} }));
    linalg::Matrix c = a;
    c *= b;
    EXPECT_TRUE(c == a * b);
    linalg::Matrix row = { {1, 2, 3} };
    linalg::Matrix col = { 1, 2, 3 };
    EXPECT_EQ((row * col).rows(), 1u);
    EXPECT_EQ((row * col)(0, 0), 14.0);
}

TEST(Arithmetic, IncompatibleSizesThrow)
{
    linalg::Matrix a(2, 2), b(3, 3);
    EXPECT_THROW(a + b, std::runtime_error);
    EXPECT_THROW(a - b, std::runtime_error);
    EXPECT_THROW(a * b, std::runtime_error);
}

TEST(Arithmetic, ComparisonWithTolerance)
{
    linalg::Matrix a = { 0.3 * 3 };
    linalg::Matrix b = { 0.9 };
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(linalg::Matrix(2, 2) != linalg::Matrix(2, 3));
}
TEST(Computations, NormTraceDet)
{
    linalg::Matrix a = { {2, 1}, {5, 3} };
    EXPECT_NEAR(a.norm(), std::sqrt(39.0), 1e-9);
    EXPECT_NEAR(a.trace(), 5.0, 1e-9);
    EXPECT_NEAR(a.det(), 1.0, 1e-9);
    EXPECT_NEAR((linalg::Matrix{ {0, 1}, {1, 0} }).det(), -1.0, 1e-9);
    EXPECT_THROW(linalg::Matrix(2, 3).det(), std::runtime_error);
    EXPECT_THROW(linalg::Matrix(2, 3).trace(), std::runtime_error);
}

TEST(Computations, Rank)
{
    EXPECT_EQ((linalg::Matrix{ {1, 2}, {2, 4} }).rank(), 1u);
    EXPECT_EQ((linalg::Matrix{ {2, 1}, {5, 3} }).rank(), 2u);
    EXPECT_EQ((linalg::Matrix{ {1, 2, 3}, {4, 5, 6} }).rank(), 2u);
}

TEST(Computations, GaussReturnsSameObject)
{
    linalg::Matrix a = { {2, 1, 5}, {5, 3, 13} };
    linalg::Matrix& r = a.gauss_forward();
    EXPECT_EQ(&r, &a);
    a.gauss_backward();
    EXPECT_TRUE(a == (linalg::Matrix{ {1, 0, 2}, {0, 1, 1} }));
}

TEST(Functions, ConcatenateTranspose)
{
    linalg::Matrix a = { {1, 2}, {3, 4} };
    linalg::Matrix b = { 5, 6 };
    EXPECT_TRUE(concatenate(a, b) == (linalg::Matrix{ {1, 2, 5}, {3, 4, 6} }));
    EXPECT_TRUE(transpose(linalg::Matrix{ {1, 2, 3} }) == (linalg::Matrix{ 1, 2, 3 }));
    EXPECT_THROW(concatenate(a, linalg::Matrix(3, 1)), std::runtime_error);
}

TEST(Functions, InvertAndSingular)
{
    linalg::Matrix a = { {2, 1}, {5, 3} };
    EXPECT_TRUE(invert(a) == (linalg::Matrix{ {3, -1}, {-5, 2} }));
    EXPECT_THROW(invert(linalg::Matrix{ {1, 2}, {2, 4} }), std::runtime_error);
    EXPECT_THROW(invert(linalg::Matrix(2, 3)), std::runtime_error);
}

TEST(Functions, Solve)
{
    linalg::Matrix A = { {2, 1}, {5, 3} };
    linalg::Matrix f = { 3, 8 };
    EXPECT_TRUE(solve(A, f) == (linalg::Matrix{ 1, 1 }));
    EXPECT_TRUE(A * solve(A, f) == f);
}

// свойства из задания
class Properties : public ::testing::Test
{
protected:
    linalg::Matrix A1 = { {2, 1, 0}, {1, 3, 1}, {0, 1, 4} };
    linalg::Matrix A2 = { {1, 2, 0}, {0, 1, 3}, {2, 0, 1} };
};

TEST_F(Properties, ZeroPowerIsIdentity)
{
    linalg::Matrix E = { {1, 0, 0}, {0, 1, 0}, {0, 0, 1} };
    EXPECT_TRUE(power(A1, 0) == E);
}

TEST_F(Properties, InverseOfTransposeEqualsTransposeOfInverse)
{
    EXPECT_TRUE(invert(transpose(A1)) == transpose(invert(A1)));
}

TEST_F(Properties, InverseOfProduct)
{
    EXPECT_TRUE(invert(A1 * A2) == invert(A2) * invert(A1));
}

TEST_F(Properties, PowerOfInverse)
{
    EXPECT_TRUE(power(invert(A1), 6) == power(power(invert(A1), -2), 3) * 1.0 ||
                power(invert(A1), 6) == power(A1, -6));
    EXPECT_TRUE(A1 * invert(A1) == power(A1, 0));
}