#include <gtest/gtest.h>

#include <cmath>
#include <map>
#include <simple_calculator/calculator.hpp>
#include <stdexcept>
#include <string>

// 测试计算器功能
TEST( CalculatorTest, BasicOperator ) {
    // clang-format off
    auto parses = std::map< std::string, double >{
        { "1+2", 3 },
		{ "2-1", 1 },
		{ "1*2", 2 },
		{ "2/1", 2 },
		{ "1/3", 1.0 / 3 },
		{ "1+2", 3 },
		{ "2-1", 1 },
    };
    // clang-format on
    auto allow_error = std::numeric_limits< double >::digits10;
    for ( auto& e : parses ) {
        Lexer lexer( e.first );
        Parser parser( lexer );
        auto ast      = parser.parse();
        double result = ast->evaluate();
        EXPECT_NEAR( result, e.second, allow_error );
    }
}

TEST( CalculatorTest, WithBrackets ) {
    // clang-format off
    auto parses = std::map< std::string, double >{
        { "1+(2+3)", 6 },
        { "(1+2)*3", 9 },
        { "(1+2)*(3+4)", 21 },
        { "(1+2)*(3+4)/2", 10.5 },
        { "(1+2)*(3+4)/2*2", 21 },
    };
    // clang-format on
    auto allow_error = std::numeric_limits< double >::digits10;
    for ( auto& e : parses ) {
        Lexer lexer( e.first );
        Parser parser( lexer );
        auto ast      = parser.parse();
        double result = ast->evaluate();
        EXPECT_NEAR( result, e.second, allow_error );
    }
}

TEST( CalculatorTest, WithBlank ) {
    // clang-format off
    auto parses = std::map< std::string, double >{
        { "1 + 2", 3 },
        { "2 - 1", 1 },
        { "1 * 2", 2 },
        { "2 / 1", 2 },
        { "1 / 3", 1.0 / 3 },
    };
    // clang-format on
    auto allow_error = std::numeric_limits< double >::digits10;
    for ( auto& e : parses ) {
        Lexer lexer( e.first );
        Parser parser( lexer );
        auto ast      = parser.parse();
        double result = ast->evaluate();
        EXPECT_NEAR( result, e.second, allow_error );
    }
}

TEST( CalculatorTest, SinCosTan ) {
    // 存在一定的精度误差
    auto allow_error = std::numeric_limits< double >::digits10;
    // clang-format off
    auto parses = std::map< std::string, double >{
        { "sin(pi/2)", 1 },
        { "sin(pi/3)", sqrt( 3 ) / 2 },
        { "(1+cos(pi/3))*2", 3 },
        { "(1+tan(pi/4))*2", 4 },
    };
    // clang-format on
    for ( auto& e : parses ) {
        Lexer lexer( e.first );
        Parser parser( lexer );
        auto ast      = parser.parse();
        double result = ast->evaluate();
        EXPECT_NEAR( result, e.second, allow_error );
    }
}

TEST( CalculatorTest, Sqrt ) {
    // 存在一定的精度误差
    auto allow_error = std::numeric_limits< double >::digits10;
    // clang-format off
    auto parses = std::map< std::string, double >{
        { "sqrt(4)", 2 },
        { "sqrt(9)", 3 },
        { "sqrt(2)", sqrt( 2 ) },
        { "(1+sqrt(3))*2", ( 1 + sqrt( 3 ) ) * 2 },
    };
    // clang-format on
    for ( auto& e : parses ) {
        Lexer lexer( e.first );
        Parser parser( lexer );
        auto ast      = parser.parse();
        double result = ast->evaluate();
        EXPECT_NEAR( result, e.second, allow_error );
    }
}

TEST( CalculatorTest, Lg ) {
    // 存在一定的精度误差
    auto allow_error = std::numeric_limits< double >::digits10;
    // clang-format off
    auto parses = std::map< std::string, double >{
        { "lg(1)", 0 },
        { "lg(e)", 1 },
        { "lg(2)", log10( 2 ) },
        { "lg(3)", log10( 3 ) },
        { "2*(1+lg(4))", 2 * ( 1 + log10( 4 ) ) },
    };
    // clang-format on
    for ( auto& e : parses ) {
        Lexer lexer( e.first );
        Parser parser( lexer );
        auto ast      = parser.parse();
        double result = ast->evaluate();
        EXPECT_NEAR( result, e.second, allow_error );
    }
}

TEST( CalculatorTest, Ln ) {
    // 存在一定的精度误差
    auto allow_error = std::numeric_limits< double >::digits10;
    // clang-format off
    auto parses = std::map< std::string, double >{
        { "ln(1)", 0 },
        { "ln(e)", 1 },
        { "ln(2)", log( 2 ) },
        { "ln(3)", log( 3 ) },
        { "2*(1+ln(4))", 2 * ( 1 + log( 4 ) ) },
    };
    // clang-format on
    for ( auto& e : parses ) {
        Lexer lexer( e.first );
        Parser parser( lexer );
        auto ast      = parser.parse();
        double result = ast->evaluate();
        EXPECT_NEAR( result, e.second, allow_error );
    }
}

TEST( CalculatorTest, Mod ) {
    // clang-format off
    auto parses = std::map< std::string, double >{
        { "1%2", 1 },
        { "2%1", 0 },
        { "3%2", 1 },
        { "4%3", 1 },
        { "2*(5%4+2)", 6 },
    };
    // clang-format on
    auto allow_error = std::numeric_limits< double >::digits10;
    for ( auto& e : parses ) {
        Lexer lexer( e.first );
        Parser parser( lexer );
        auto ast      = parser.parse();
        double result = ast->evaluate();
        EXPECT_NEAR( result, e.second, allow_error );
    }
}

TEST( CalculatorTest, Factorial ) {
    // clang-format off
    auto parses = std::map< std::string, double >{
        { "0!", 1 },
        { "1!", 1 },
        { "2!", 2 },
        { "3!", 6 },
        { "4!", 24 },
        { "1+3*5!", 361 },
        { "1+3*(1+5)!", 2161 },
        { "1+(2*cos(pi/3))!", 2 },
        { "(1+2*sin(pi/6))!", 2 },
    };
    // clang-format on
    auto allow_error = std::numeric_limits< double >::digits10;
    for ( auto& e : parses ) {
        Lexer lexer( e.first );
        Parser parser( lexer );
        auto ast      = parser.parse();
        double result = ast->evaluate();
        EXPECT_NEAR( result, e.second, allow_error );
    }
}

TEST( CalculatorTest, Square ) {
    // clang-format off
    auto parses = std::map< std::string, double >{
        { "2^2", 4 },
        { "3^3", 27 },
        { "2*2^3", 16 },
        { "(2*2)^2", 16 },
    };
    // clang-format on
    auto allow_error = std::numeric_limits< double >::digits10;
    for ( auto& e : parses ) {
        Lexer lexer( e.first );
        Parser parser( lexer );
        auto ast      = parser.parse();
        double result = ast->evaluate();
        EXPECT_NEAR( result, e.second, allow_error );
    }
}

TEST( CalculatorTest, DotSupport ) {
    // 测试小数点
    // clang-format off
    auto parses = std::map< std::string, double >{
        { "1.5+2.5", 4 },
        { "1.5*2.5", 3.75 },
        { "1.5/2.5", 0.6 },
        { "1.5^2", 2.25 },
        { "sqrt(4.0)", 2 },
    };
    // clang-format on
    auto allow_error = std::numeric_limits< double >::digits10;
    for ( auto& e : parses ) {
        Lexer lexer( e.first );
        Parser parser( lexer );
        auto ast      = parser.parse();
        double result = ast->evaluate();
        EXPECT_NEAR( result, e.second, allow_error );
    }
}

TEST( CalculatorTest, UnmatchBrackets ) {
    // 严格检查，不允许左括号不匹配
    EXPECT_THROW(
        {
            Lexer lexer( "(1+2*3" );
            Parser parser( lexer );
            auto ast = parser.parse();
            EXPECT_EQ( ast->evaluate(), 7 );
        },
        std::runtime_error );
    EXPECT_THROW(
        {
            Lexer lexer( "1*(2+3" );
            Parser parser( lexer );
            auto ast = parser.parse();
            EXPECT_EQ( ast->evaluate(), 5 );
        },
        std::runtime_error );
    EXPECT_THROW(
        {
            Lexer lexer( "1+2*3)" );
            Parser parser( lexer );
            parser.parse();
        },
        std::runtime_error );
}

TEST( CalculatorTest, AllowEmptyInput ) {
    // 允许输入空字符串
    EXPECT_NO_THROW( {
        Lexer lexer( "" );
        Parser parser( lexer );
        parser.parse();
    } );
}

TEST( CalculatorTest, InvalidInput ) {
    EXPECT_THROW(
        {
            Lexer lexer( "!!" );
            Parser parser( lexer );
            parser.parse();
        },
        std::runtime_error );
}
