#include <gtest/gtest.h>

#include <simple_calculator/calculator.hpp>
#include <cmath>
#include <stdexcept>

// 测试计算器功能
TEST( CalculatorTest, BasicOperator ) {
    Calculator c;
    EXPECT_EQ( c.doIt( "1+2=" ), 3 );
    EXPECT_EQ( c.doIt( "2-1=" ), 1 );
    EXPECT_EQ( c.doIt( "1*2=" ), 2 );
    EXPECT_EQ( c.doIt( "2/1=" ), 2 );
    // 存在误差的除法
    auto allow_error = std::numeric_limits< double >::digits10;
    EXPECT_NEAR( c.doIt( "1/3=" ), 1.0 / 3, allow_error );
}

TEST( CalculatorTest, WithBrackets ) {
    Calculator c;
    EXPECT_EQ( c.doIt( "1+(2+3)=" ), 6 );
    EXPECT_EQ( c.doIt( "(1+2)*3=" ), 9 );
    EXPECT_EQ( c.doIt( "(1+2)*(3+4)=" ), 21 );
    EXPECT_EQ( c.doIt( "(1+2)*(3+4)/2=" ), 10.5 );
    EXPECT_EQ( c.doIt( "(1+2)*(3+4)/2*2=" ), 21 );
}

TEST( CalculatorTest, SinCosTan ) {
    Calculator c;
    // 存在一定的精度误差
    auto allow_error = std::numeric_limits< double >::digits10;
    EXPECT_NEAR( c.doIt( "sin(pi/2)=" ), 1, allow_error );
    EXPECT_NEAR( c.doIt( "sin(pi/3)=" ), sqrt( 3 ) / 2, allow_error );
    EXPECT_NEAR( c.doIt( "(1+cos(pi/3))*2=" ), 3, allow_error );
}

TEST( CalculatorTest, Sqrt ) {
    Calculator c;
    // 存在一定的精度误差
    auto allow_error = std::numeric_limits< double >::digits10;
    EXPECT_EQ( c.doIt( "sqrt(4)=" ), 2 );
    EXPECT_EQ( c.doIt( "sqrt(9)=" ), 3 );
    EXPECT_NEAR( c.doIt( "sqrt(2)=" ), sqrt( 2 ), allow_error );
    EXPECT_NEAR( c.doIt( "(1+sqrt(3))*2=" ), ( 1 + sqrt( 3 ) ) * 2, allow_error );
}

TEST( CalculatorTest, Square ) {
    Calculator c;
    EXPECT_EQ( c.doIt( "2^2=" ), 4 );
    EXPECT_EQ( c.doIt( "3^3=" ), 27 );
    EXPECT_EQ( c.doIt( "2*2^3=" ), 16 );
    EXPECT_EQ( c.doIt( "(2*2)^2=" ), 16 );
}

TEST( CalculatorTest, DotSupport ) {
    Calculator c;
    // 测试小数点
    EXPECT_EQ( c.doIt( "1.5+2.5=" ), 4 );
    EXPECT_EQ( c.doIt( "1.5*2.5=" ), 3.75 );
    EXPECT_EQ( c.doIt( "1.5/2.5=" ), 0.6 );
    EXPECT_EQ( c.doIt( "1.5^2=" ), 2.25 );
    EXPECT_EQ( c.doIt( "sqrt(4.0)=" ), 2 );
}

TEST( CalculatorTest, UnmatchBrackets ) {
    Calculator c;
    // 允许左括号不匹配，这时相当于括号不起作用
    EXPECT_EQ( c.doIt( "(1+2*3=" ), 7 );
    EXPECT_EQ( c.doIt( "1*(2+3=" ), 5 );
    // 不允许右括号不匹配，抛出异常
    EXPECT_THROW( c.doIt( "1+2*3)=" ), invalid_argument );
}

TEST( CalculatorTest, InvalidInput ) {
    Calculator c;
    // 允许输入空字符串
    EXPECT_NO_THROW( c.doIt( "" ) );
    // 必须以等号结尾(Mybe don't needed to check this, As we can ensure this in GUI layer)
    EXPECT_NO_THROW( c.doIt( "1+2*3=" ) );
    EXPECT_THROW( c.doIt( "1+2*3" ), invalid_argument );
    EXPECT_THROW( c.doIt( "1+2*3=1+2*3" ), invalid_argument );
}
