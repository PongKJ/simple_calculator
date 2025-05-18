#include <cctype>
#include <memory>
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

enum class TokenType { NUMBER, OP_PLUS, OP_MINUS, OP_MUL, OP_DIV, LPAREN, RPAREN, END };

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

        switch ( c ) {
        case '+':
            return Token( TokenType::OP_PLUS );
        case '-':
            return Token( TokenType::OP_MINUS );
        case '*':
            return Token( TokenType::OP_MUL );
        case '/':
            return Token( TokenType::OP_DIV );
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
        throw std::runtime_error( "Invalid factor" );
    }

    std::unique_ptr< ASTNode > term() {
        auto node = factor();
        while ( currentToken.type == TokenType::OP_MUL || currentToken.type == TokenType::OP_DIV ) {
            Token op = currentToken;
            if ( op.type == TokenType::OP_MUL ) {
                eat( TokenType::OP_MUL );
                node = std::make_unique< MultiplyNode >( std::move( node ), factor() );
            }
            else {
                eat( TokenType::OP_DIV );
                node = std::make_unique< DivideNode >( std::move( node ), factor() );
            }
        }
        return node;
    }

public:
    explicit Parser( Lexer& l ) : lexer( l ), currentToken( l.nextToken() ) {}

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
};
