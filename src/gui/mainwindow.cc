#include "mainwindow.hpp"

#include <qcontainerfwd.h>
#include <qfont.h>
#include <qlogging.h>
#include <qobject.h>
#include <qsizepolicy.h>

#include <QGridLayout>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <format>
#include <iostream>

MainWindow::MainWindow( QWidget* parent ) : QMainWindow( parent ) {
    // 设置窗口标题和大小
    setWindowTitle( "简易计算器" );
    setFixedSize( 400, 500 );

    // 创建主部件和布局
    auto* centralWidget = new QWidget( this );
    setCentralWidget( centralWidget );
    auto* layout = new QGridLayout( centralWidget );

    calculator = new Calculator();

    // 创建显示区域
    displayer = new Displayer( this );
    displayer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    layout->addWidget( displayer, 0, 0, 1, 5 );

    // 创建按钮
    // clang-format off
    QStringList buttonLabels = {
    	"lg","π","e","C","⌫",
    	"sin","(", ")","n!","/",
    	"cos","7", "8", "9","*",
        "sqrt","4","5","6","-",
        "^","1", "2","3","+",
        "tan","%","0", ".","=" };
    // clang-format on

    // 添加按钮到布局
    QSizePolicy sizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    for ( int i = 0; i < buttonLabels.size(); ++i ) {
        int row = i / 5 + 1;
        int col = i % 5;

        auto* button = new QPushButton( buttonLabels[ i ] );
        button->setSizePolicy( sizePolicy );
        auto font = button->font();
        font.setPointSize( 16 );
        button->setFont( font );

        layout->addWidget( button, row, col );

        // 连接按钮点击信号到槽函数
        connect( button, &QPushButton::clicked, this, &MainWindow::onButtonClicked );
    }
    layout->setSpacing( 3 );
    layout->setContentsMargins( 15, 15, 15, 15 );
}

MainWindow::~MainWindow() {
    delete calculator;
};

void MainWindow::onButtonClicked() {
    auto* button = qobject_cast< QPushButton* >( sender() );
    if ( !button )
        return;

    QString buttonText  = button->text();
    QString displayText = displayer->getExpressionLine();

    if ( buttonText == "C" ) {
        displayer->clearDisplay();
    }
    else if ( buttonText == "=" ) {
        displayText = displayText + "=";
        displayer->setExpression( displayText );
        // replace utf-8 "π" with "pi"
        displayText.replace( "π", "pi" );
        try {
            double result = calculator->doIt( displayText.toStdString() );
            displayer->setResult( QString::number( result ) );
        }
        catch ( const std::exception& e ) {
            std::cerr << "Error: " << e.what() << std::endl;
            displayer->setResult( QString::fromStdString( format( "Bad expression" ) ) );
        }
    }
    else {
        if ( displayText.contains( "=" ) ) {
            // Clear last calculate result
            displayText.clear();
            displayer->clearDisplay();
        }
        if ( buttonText == "⌫" ) {
            if ( !displayText.isEmpty() ) {
                displayText.chop( 1 );
                displayer->setExpression( displayText );
            }
        }
        else if ( buttonText == "sin" || buttonText == "cos" || buttonText == "sqrt" || buttonText == "tan"
                  || buttonText == "lg" ) {
            displayer->setExpression( displayText + buttonText + "(" );
        }
        else if ( buttonText == "n!" ) {
            displayer->setExpression( displayText + "!" );
        }
        else {
            displayer->setExpression( displayText + buttonText );
        }
    }
}
