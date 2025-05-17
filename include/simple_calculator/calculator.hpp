#pragma once
#include <cmath>
#include <format>
#include <iostream>
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
        else if ( str == "*" || str == "/" ) {
            return 1;
        }
        else if ( str == "^" ) {
            return 2;
        }
        else if ( str == "sin" || str == "cos" || str == "tan" || str == "sqrt" ) {
            return 3;
        }
        else {
            [[unlikely]] throw invalid_argument( format( "invalid operator:'{}'", str ) );
        }
    }
    static bool isBinary( const string& opr ) {
        return opr == "+" || opr == "-" || opr == "*" || opr == "/" || opr == "^";
    }
    static double readNum( string::const_iterator& it ) {
        string t;
        while ( isNum( it ) )
            t += *it++;
        return stod( t );
    }
    static string readOperator( string::const_iterator& it ) {
        const string opr[] = { "+", "-", "*", "/", "^", "sin", "cos", "tan", "sqrt" };
        for ( const auto& o : opr ) {
            auto len = static_cast< long >( o.length() );
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
            // TODO: refactor this
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
        }
        m_opr.pop();
    }
    static bool isNum( string::const_iterator& c ) {
        return ( *c >= '0' && *c <= '9' ) || *c == '.';
    }

    static bool isPi( string::const_iterator& c ) {
        return *c == 'p' && *( c + 1 ) == 'i';
    }

public:
    Calculator() = default;
    double doIt( const string& exp ) {
        m_num.clear();
        // 处理空字符串
        if ( exp.empty() ) {
            return 0;
        }
        // 处理不以等号结尾的表达式
        if ( exp.back() != '=' ) {
            throw invalid_argument( "expression should end with '='" );
        }
        for ( auto it = exp.begin(); it != exp.end(); ) {
            if ( isNum( it ) ) {
                m_num.push( readNum( it ) );
            }
            else if ( isPi( it ) ) {
                m_num.push( std::atan( 1.0 ) * 4 );
                it += 2;
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
                    auto opr = readOperator( it );
                    while ( !m_opr.empty() && m_opr.top() != "(" && precedence( opr ) <= precedence( m_opr.top() ) ) {
                        if ( isBinary( opr ) ) {
                            calculate();
                        }
                    }
                    m_opr.push( opr );
                }
            }
        }
        return m_num.top();
    }
};
