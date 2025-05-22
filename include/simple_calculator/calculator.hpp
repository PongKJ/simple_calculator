#include <cctype>
#include <cmath>
#include <format>
#include <memory>
#include <numbers>
#include <stdexcept>
#include <string>
#include <utility>

class ASTNode {
public:
    virtual ~ASTNode()                            = default;
    [[nodiscard]] virtual double evaluate() const = 0;
};

class NumberNode : public ASTNode {
    double value;

public:
    explicit NumberNode( double val ) : value( val ) {}
    [[nodiscard]] double evaluate() const override {
        return value;
    }
};

class BinaryOpNode : public ASTNode {
protected:
    std::unique_ptr< ASTNode > left;
    std::unique_ptr< ASTNode > right;

public:
    BinaryOpNode( std::unique_ptr< ASTNode > l, std::unique_ptr< ASTNode > r )
        : left( std::move( l ) ), right( std::move( r ) ) {}
};

class AddNode : public BinaryOpNode {
public:
    using BinaryOpNode::BinaryOpNode;
    [[nodiscard]] double evaluate() const override {
        return left->evaluate() + right->evaluate();
    }
};

class SubtractNode : public BinaryOpNode {
public:
    using BinaryOpNode::BinaryOpNode;
    [[nodiscard]] double evaluate() const override {
        return left->evaluate() - right->evaluate();
    }
};

class MultiplyNode : public BinaryOpNode {
public:
    using BinaryOpNode::BinaryOpNode;
    [[nodiscard]] double evaluate() const override {
        return left->evaluate() * right->evaluate();
    }
};

class DivideNode : public BinaryOpNode {
public:
    using BinaryOpNode::BinaryOpNode;
    [[nodiscard]] double evaluate() const override {
        double denominator = right->evaluate();
        if ( denominator == 0 )
            throw std::runtime_error( "Division by zero" );
        return left->evaluate() / denominator;
    }
};

class PowerNode : public BinaryOpNode {
public:
    using BinaryOpNode::BinaryOpNode;
    [[nodiscard]] double evaluate() const override {
        return std::pow( left->evaluate(), right->evaluate() );
    }
};

class ModuloNode : public BinaryOpNode {
public:
    using BinaryOpNode::BinaryOpNode;
    [[nodiscard]] double evaluate() const override {
        double divisor = right->evaluate();
        if ( divisor == 0 )
            throw std::runtime_error( "Modulo by zero" );
        return std::fmod( left->evaluate(), divisor );
    }
};

class UnaryFunctionNode : public ASTNode {
protected:
    std::unique_ptr< ASTNode > operand;

public:
    explicit UnaryFunctionNode( std::unique_ptr< ASTNode > op ) : operand( std::move( op ) ) {}
};

class SqrtNode : public UnaryFunctionNode {
public:
    using UnaryFunctionNode::UnaryFunctionNode;
    [[nodiscard]] double evaluate() const override {
        double val = operand->evaluate();
        if ( val < 0 )
            throw std::runtime_error( "Square root of negative number" );
        return std::sqrt( val );
    }
};

class SinNode : public UnaryFunctionNode {
public:
    using UnaryFunctionNode::UnaryFunctionNode;
    [[nodiscard]] double evaluate() const override {
        return std::sin( operand->evaluate() );
    }
};

class CosNode : public UnaryFunctionNode {
public:
    using UnaryFunctionNode::UnaryFunctionNode;
    [[nodiscard]] double evaluate() const override {
        return std::cos( operand->evaluate() );
    }
};

class TanNode : public UnaryFunctionNode {
public:
    using UnaryFunctionNode::UnaryFunctionNode;
    [[nodiscard]] double evaluate() const override {
        double val = operand->evaluate();
        if ( std::cos( val ) == 0 )  
            throw std::runtime_error( "Tangent undefined (division by zero)" );
        return std::tan( val );
    }
};

class LgNode : public UnaryFunctionNode {
public:
    using UnaryFunctionNode::UnaryFunctionNode;
    [[nodiscard]] double evaluate() const override {
        double val = operand->evaluate();
        if ( val <= 0 )
            throw std::runtime_error( "Logarithm of non-positive number" );
        return std::log10( val );
    }
};

class LnNode : public UnaryFunctionNode {
public:
    using UnaryFunctionNode::UnaryFunctionNode;
    [[nodiscard]] double evaluate() const override {
        double val = operand->evaluate();
        if ( val <= 0 )
            throw std::runtime_error( "Natural logarithm of non-positive number" );
        return std::log( val );
    }
};

class FactorialNode : public UnaryFunctionNode {
public:
    using UnaryFunctionNode::UnaryFunctionNode;
    [[nodiscard]] double evaluate() const override {
        double val = operand->evaluate();
        // Factorial is only defined for non-negative integers
        if ( val < 0 )
            throw std::runtime_error( "Factorial of negative number" );

        // Check if the value is very close to an integer
        double intPart;
        if ( std::abs( std::modf( val, &intPart ) ) > 1e-10 )
            throw std::runtime_error( "Factorial only defined for non-negative integers" );

        auto n        = static_cast< unsigned int >( intPart );
        double result = 1.0;
        for ( unsigned int i = 2; i <= n; ++i )
            result *= i;
        return result;
    }
};

// clang-format off
enum class TokenType {
    // 普通数字
    NUMBER,
    // 运算符+,-,*,/,^,%,!
    OP_PLUS,OP_MINUS,OP_MUL,OP_DIV,
    OP_POW, OP_MOD,OP_FACTORIAL,
    // 左右括号
    LPAREN,RPAREN,
    // 函数sqrt,sin,cos,tan,lg,ln
    FUNC_SQRT,FUNC_SIN,FUNC_COS,FUNC_TAN,FUNC_LG,FUNC_LN,
    // 常量pi,e
    CONST_PI,CONST_E,
    // 结束标志
    END
};
// clang-format on

struct Token {
    TokenType type;
    double value;  // 仅当type为NUMBER时有效
    explicit Token( TokenType t ) : type( t ), value( 0 ) {}
    explicit Token( double v ) : type( TokenType::NUMBER ), value( v ) {}
};

class Lexer {
    std::string input;
    size_t pos = 0;

    void skipWhitespace() {
        while ( pos < input.size() && std::isspace( input[ pos ] ) )
            pos++;
    }

public:
    explicit Lexer( std::string str ) : input( std::move( str ) ) {}

    Token nextToken() {
        skipWhitespace();
        if ( pos >= input.size() )
            return Token( TokenType::END );

        char c = input[ pos++ ];
        if ( std::isdigit( c ) || c == '.' ) {
            size_t start = pos - 1;
            while ( pos < input.size() && ( std::isdigit( input[ pos ] ) || input[ pos ] == '.' ) )
                pos++;
            return Token( std::stod( input.substr( start, pos - start ) ) );
        }

        if ( std::isalpha( c ) ) {
            size_t start = pos - 1;
            while ( pos < input.size() && std::isalpha( input[ pos ] ) ) {
                pos++;
            }
            std::string identifier = input.substr( start, pos - start );
            if ( identifier == "sqrt" )
                return Token( TokenType::FUNC_SQRT );
            if ( identifier == "sin" )
                return Token( TokenType::FUNC_SIN );
            if ( identifier == "cos" )
                return Token( TokenType::FUNC_COS );
            if ( identifier == "tan" )
                return Token( TokenType::FUNC_TAN );
            if ( identifier == "lg" )
                return Token( TokenType::FUNC_LG );
            if ( identifier == "ln" )
                return Token( TokenType::FUNC_LN );
            if ( identifier == "pi" )
                return Token( TokenType::CONST_PI );
            if ( identifier == "e" )
                return Token( TokenType::CONST_E );
            throw std::runtime_error( "Unknown identifier: " + identifier );
        }

        switch ( c ) {
        case '+':
            return Token( TokenType::OP_PLUS );
        case '-':
            return Token( TokenType::OP_MINUS );
        case '*':
            return Token( TokenType::OP_MUL );
        case '/':
            return Token( TokenType::OP_DIV );
        case '^':
            return Token( TokenType::OP_POW );
        case '%':
            return Token( TokenType::OP_MOD );
        case '!':
            return Token( TokenType::OP_FACTORIAL );
        case '(':
            return Token( TokenType::LPAREN );
        case ')':
            return Token( TokenType::RPAREN );
        default:
            throw std::runtime_error( "Invalid character: " + std::string( 1, c ) );
        }
    }
};

class Parser {
    Lexer& lexer;
    Token currentToken;

    void eat( TokenType expected ) {
        if ( currentToken.type == expected ) {
            currentToken = lexer.nextToken();
        }
        else {
            throw std::runtime_error( "Unexpected token" );
        }
    }

    std::unique_ptr< ASTNode > factor() {
        Token token = currentToken;
        if ( token.type == TokenType::NUMBER ) {
            eat( TokenType::NUMBER );
            return std::make_unique< NumberNode >( token.value );
        }
        else if ( token.type == TokenType::CONST_PI ) {
            eat( TokenType::CONST_PI );
            return std::make_unique< NumberNode >( std::numbers::pi );
        }
        else if ( token.type == TokenType::CONST_E ) {
            eat( TokenType::CONST_E );
            return std::make_unique< NumberNode >( std::numbers::e );
        }
        else if ( token.type == TokenType::LPAREN ) {
            eat( TokenType::LPAREN );
            auto node = expression();
            eat( TokenType::RPAREN );
            return node;
        }
        else if ( token.type == TokenType::OP_MINUS ) {
            eat( TokenType::OP_MINUS );
            return std::make_unique< SubtractNode >( std::make_unique< NumberNode >( 0 ), factor() );
        }
        else if ( token.type == TokenType::OP_PLUS ) {
            eat( TokenType::OP_PLUS );
            return factor();
        }
        else if ( token.type == TokenType::FUNC_SQRT ) {
            eat( TokenType::FUNC_SQRT );
            eat( TokenType::LPAREN );
            auto arg = expression();
            eat( TokenType::RPAREN );
            return std::make_unique< SqrtNode >( std::move( arg ) );
        }
        else if ( token.type == TokenType::FUNC_SIN ) {
            eat( TokenType::FUNC_SIN );
            eat( TokenType::LPAREN );
            auto arg = expression();
            eat( TokenType::RPAREN );
            return std::make_unique< SinNode >( std::move( arg ) );
        }
        else if ( token.type == TokenType::FUNC_COS ) {
            eat( TokenType::FUNC_COS );
            eat( TokenType::LPAREN );
            auto arg = expression();
            eat( TokenType::RPAREN );
            return std::make_unique< CosNode >( std::move( arg ) );
        }
        else if ( token.type == TokenType::FUNC_TAN ) {
            eat( TokenType::FUNC_TAN );
            eat( TokenType::LPAREN );
            auto arg = expression();
            eat( TokenType::RPAREN );
            return std::make_unique< TanNode >( std::move( arg ) );
        }
        else if ( token.type == TokenType::FUNC_LG ) {
            eat( TokenType::FUNC_LG );
            eat( TokenType::LPAREN );
            auto arg = expression();
            eat( TokenType::RPAREN );
            return std::make_unique< LgNode >( std::move( arg ) );
        }
        else if ( token.type == TokenType::FUNC_LN ) {
            eat( TokenType::FUNC_LN );
            eat( TokenType::LPAREN );
            auto arg = expression();
            eat( TokenType::RPAREN );
            return std::make_unique< LnNode >( std::move( arg ) );
        }
        throw std::runtime_error( "Invalid factor" );
    }

    // INFO:优先级按照调用链排列，最先被调用的优先级最低，按照优先级从低到高排列如下:
    // expression() → term() → power_expression() → factorial_expression() → factor()
    std::unique_ptr< ASTNode > factorial_expression() {
        auto node = factor();
        if ( currentToken.type == TokenType::OP_FACTORIAL ) {
            eat( TokenType::OP_FACTORIAL );
            return std::make_unique< FactorialNode >( std::move( node ) );
        }
        return node;
    }

    std::unique_ptr< ASTNode > power_expression() {
        auto node = factorial_expression();
        if ( currentToken.type == TokenType::OP_POW ) {
            eat( TokenType::OP_POW );
            return std::make_unique< PowerNode >( std::move( node ), power_expression() );
        }
        return node;
    }

    std::unique_ptr< ASTNode > term() {
        auto node = power_expression();  
        while ( currentToken.type == TokenType::OP_MUL || currentToken.type == TokenType::OP_DIV
                || currentToken.type == TokenType::OP_MOD ) {
            Token op = currentToken;
            if ( op.type == TokenType::OP_MUL ) {
                eat( TokenType::OP_MUL );
                node = std::make_unique< MultiplyNode >( std::move( node ), power_expression() );
            }
            else if ( op.type == TokenType::OP_DIV ) {
                eat( TokenType::OP_DIV );
                node = std::make_unique< DivideNode >( std::move( node ), power_expression() );
            }
            else {  
                eat( TokenType::OP_MOD );
                node = std::make_unique< ModuloNode >( std::move( node ), power_expression() );
            }
        }
        return node;
    }

    std::unique_ptr< ASTNode > expression() {
        auto node = term();
        while ( currentToken.type == TokenType::OP_PLUS || currentToken.type == TokenType::OP_MINUS ) {
            Token op = currentToken;
            if ( op.type == TokenType::OP_PLUS ) {
                eat( TokenType::OP_PLUS );
                node = std::make_unique< AddNode >( std::move( node ), term() );
            }
            else {
                eat( TokenType::OP_MINUS );
                node = std::make_unique< SubtractNode >( std::move( node ), term() );
            }
        }
        return node;
    }

public:
    explicit Parser( Lexer& l ) : lexer( l ), currentToken( l.nextToken() ) {}

    std::unique_ptr< ASTNode > parse() {
        // handle empty input
        if ( currentToken.type == TokenType::END ) {
            return std::make_unique< NumberNode >( NumberNode( 0 ) );
        }
        auto node = expression();
        // Check that we've reached the end of input
        if ( currentToken.type != TokenType::END ) {
            throw std::runtime_error(
                std::format( "Unexpected token after expression: {}", static_cast< int >( currentToken.type ) ) );
        }
        return node;
    }
};
