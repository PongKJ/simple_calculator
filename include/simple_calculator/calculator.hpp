#pragma once
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <format>
#include <iostream>
#include <numbers>
#include <string>

#include "stack.hpp"

using namespace std;

class Calculator {
private:
    Stack< double > m_num;
    Stack< string > m_opr;

    [[nodiscard]] static int precedence( const string& str ) {
        if ( str == "+" || str == "-" ) {
            return 0;
        }
        else if ( str == "*" || str == "/" || str == "%" ) {
            return 1;
        }
        else if ( str == "^" ) {
            return 2;
        }
        else if ( str == "sin" || str == "cos" || str == "tan" || str == "sqrt" || str == "log" || str == "!" ) {
            return 3;
        }
        else {
            [[unlikely]] throw invalid_argument( format( "invalid operator:'{}'", str ) );
        }
    }
    static bool isBinary( const string& opr ) {
        return opr == "+" || opr == "-" || opr == "*" || opr == "/" || opr == "^" || opr == "%";
    }
    static double readNum( string::const_iterator& it ) {
        string t;
        while ( isNum( it ) )
            t += *it++;
        return stod( t );
    }
    static double readConstant( string::const_iterator& it ) {
        if ( string( it, it + 2 ) == "pi" ) {
            it += 2;
            return numbers::pi;
        }
        else if ( string( it, it + 1 ) == "e" ) {
            it += 1;
            return numbers::e;
        }
        throw invalid_argument( format( "invalid constant start on:'{}'", *it ) );
    }
    static string readOperator( string::const_iterator& it, string::difference_type max_len ) {
        const string opr[] = { "+", "-", "*", "/", "^", "sin", "cos", "tan", "sqrt", "log", "%", "!" };
        for ( const auto& o : opr ) {
            auto len = static_cast< string::difference_type >( o.length() );
            if ( len >= max_len ) {
                // 如果剩余字符长度小于操作符长度，直接跳过
                continue;
            }
            if ( string( it, it + len ) == o ) {
                it += len;
                return o;
            }
        }
        throw invalid_argument( format( "invalid operator start on:'{}'", *it ) );
    }
    void calculate() {
        if ( m_opr.top() == "(" ) {
            // 在此遇到左括号，说明存在未匹配的左括号，直接弹出，不做任何运算
            // 这时相当于括号不起作用
            cout << "warn: unmatched bracket find" << endl;
            m_opr.pop();
            return;
        }
        double rhs = m_num.top();
        m_num.pop();
        if ( isBinary( m_opr.top() ) ) {
            if ( m_num.empty() ) {
                if ( m_opr.top() == "-" ) {
                    // Deal with the minus sign rather than the subtraction sign.
                    m_num.push( -rhs );
                }
                else if ( m_opr.top() == "+" ) {
                    m_num.push( rhs );
                }
                else {
                    throw invalid_argument( format( "invalid expression" ) );
                }
                m_opr.pop();
                return;
            }
            double lhs = m_num.top();
            m_num.pop();
            if ( m_opr.top() == "+" ) {
                m_num.push( lhs + rhs );
                cout << "calc:" << lhs << "+" << rhs << "=" << m_num.top() << endl;
            }
            else if ( m_opr.top() == "-" ) {
                m_num.push( lhs - rhs );
                cout << "calc:" << lhs << "-" << rhs << "=" << m_num.top() << endl;
            }
            else if ( m_opr.top() == "*" ) {
                m_num.push( lhs * rhs );
                cout << "calc:" << lhs << "*" << rhs << "=" << m_num.top() << endl;
            }
            else if ( m_opr.top() == "/" ) {
                m_num.push( lhs / rhs );
                cout << "calc:" << lhs << "/" << rhs << "=" << m_num.top() << endl;
            }
            else if ( m_opr.top() == "^" ) {
                m_num.push( std::pow( lhs, rhs ) );
                cout << "calc:" << lhs << "^" << rhs << "=" << m_num.top() << endl;
            }
            else if ( m_opr.top() == "%" ) {
                m_num.push( std::fmod( lhs, rhs ) );
                cout << "calc:" << lhs << "%" << rhs << "=" << m_num.top() << endl;
            }
        }
        else {
            if ( m_opr.top() == "sin" ) {
                m_num.push( std::sin( rhs ) );
                cout << "calc:sin(" << rhs << ")" << "=" << m_num.top() << endl;
            }
            else if ( m_opr.top() == "cos" ) {
                m_num.push( std::cos( rhs ) );
                cout << "calc:cos(" << rhs << ")" << "=" << m_num.top() << endl;
            }
            else if ( m_opr.top() == "tan" ) {
                m_num.push( std::tan( rhs ) );
                cout << "calc:tan(" << rhs << ")" << "=" << m_num.top() << endl;
            }
            else if ( m_opr.top() == "sqrt" ) {
                m_num.push( std::sqrt( rhs ) );
                cout << "calc:sqrt(" << rhs << ")" << "=" << m_num.top() << endl;
            }
            else if ( m_opr.top() == "log" ) {
                // TODO: Make it support mutable base
                // inline double log_base(double value, double base) {
                //      return std::log(value) / std::log(base);
                //  }
                m_num.push( std::log( rhs ) );
                cout << "calc:log(" << rhs << ")" << "=" << m_num.top() << endl;
            }
            else if ( m_opr.top() == "!" ) {
                cout << "calc:" << rhs << "!" << endl;
                if ( rhs == 0 ) {
                    m_num.push( 1 );
                }
                else {
                    // NOTE: Factorial is only defined for non-negative integers
                    auto allow_error = std::numeric_limits< double >::digits10;
                    if ( rhs < 0 ) {
                        throw invalid_argument(
                            format( "calculate factorial only defined for non-negative number, found :'{}!'", rhs ) );
                    }
                    if ( abs( std::floor( rhs ) - rhs ) > allow_error ) {
                        throw invalid_argument(
                            format( "calculate factorial only defined for integers, found :'{}!'", rhs ) );
                    }
                    else {
                        // There is some accuracy error in the double type, so we need to round it
                        rhs = std::round( rhs );
                    }
                    double result = 1;
                    for ( int i = 1; i <= rhs; ++i ) {
                        result *= i;
                    }
                    m_num.push( result );
                }
                cout << "calc:" << rhs << "! " << "=" << m_num.top() << endl;
            }
            else {
                throw invalid_argument( format( "invalid operator:'{}'", m_opr.top() ) );
            }
        }
        m_opr.pop();
    }
    static bool isNum( string::const_iterator& c ) {
        return ( *c >= '0' && *c <= '9' ) || *c == '.';
    }

    static bool isConstant( string::const_iterator& c ) {
        // NOTE: Before calculating pi, we need to replace utf-8 "π" with "pi"
        return string( c, c + 2 ) == "pi" || string( c, c + 1 ) == "e";
    }

public:
    Calculator() = default;
    double doIt( const string& _expression ) {
        // Clear the stacks
        m_num.clear();
        m_opr.clear();
        // 处理空字符串
        if ( _expression.empty() ) {
            return 0;
        }
        // 处理空格
        string expression = _expression;
        expression.erase( std::remove( expression.begin(), expression.end(), ' ' ), expression.end() );
        // 处理不以等号结尾的表达式
        if ( expression.back() != '=' ) {
            throw invalid_argument( "expression should end with '='" );
        }
        for ( auto it = expression.cbegin(); it != expression.cend(); ) {
            if ( isNum( it ) ) {
                m_num.push( readNum( it ) );
            }
            else if ( isConstant( it ) ) {
                m_num.push( readConstant( it ) );
            }
            else {
                if ( *it == '=' ) {
                    while ( !m_opr.empty() ) {
                        calculate();
                    }
                    return m_num.top();
                }
                else if ( *it == '(' ) {
                    m_opr.push( "(" );
                    it++;
                }
                else if ( *it == ')' ) {
                    while ( !m_opr.empty() ) {
                        if ( m_opr.top() == "(" ) {
                            break;
                        }
                        calculate();
                    }
                    if ( m_opr.empty() ) {
                        throw invalid_argument( "unmatched brackets" );
                    }
                    m_opr.pop();  // pop "("
                    it++;
                }
                else {
                    auto opr = readOperator( it, expression.cend() - it );
                    while ( !m_opr.empty() && m_opr.top() != "(" && precedence( opr ) <= precedence( m_opr.top() ) ) {
                        if ( isBinary( opr ) ) {
                            calculate();
                        }
                        if ( m_num.empty() ) {
                            // If the number stack is empty, it means that the expression is invalid
                            // To prevent forever looping, we'll throw an exception
                            throw invalid_argument( "invalid expression" );
                        }
                    }
                    m_opr.push( opr );
                }
            }
        }
        return m_num.top();
    }
};
