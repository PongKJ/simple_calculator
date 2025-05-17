#include "displayer.hpp"

#include <qcontainerfwd.h>
#include <qlogging.h>
#include <qobject.h>
#include <qsizepolicy.h>

#include <QGridLayout>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

Displayer::Displayer( QWidget* parent ) : QTextEdit( parent ) {
    // 创建显示区域
    this->setReadOnly( true );
    this->setAlignment( Qt::AlignRight );
    this->setFixedHeight( 100 );

    QFont font = this->font();
    font.setPointSize( 22 );
    this->setFont( font );
    clearDisplay();
}

Displayer::~Displayer() = default;

void Displayer::clearDisplay() {
    this->clear();
    QTextCursor cursor = this->textCursor();
    // 第一行格式 - 左对齐
    QTextBlockFormat blockFormat;
    blockFormat.setAlignment( Qt::AlignLeft );
    cursor.setBlockFormat( blockFormat );
    cursor.insertText( "" );

    // 添加换行符
    cursor.insertBlock();

    // 第二行格式 - 右对齐
    blockFormat.setAlignment( Qt::AlignRight );
    cursor.setBlockFormat( blockFormat );
    cursor.insertText( "" );

    // 移动光标到文档开始
    cursor.movePosition( QTextCursor::Start );
    this->setTextCursor( cursor );
}

// 获取第一行（表达式行）
QString Displayer::getExpressionLine() {
    QTextDocument* doc = this->document();
    QTextBlock block   = doc->findBlockByLineNumber( 0 );
    return block.text();
}

// 获取第二行（结果行）
QString Displayer::getResultLine() {
    QTextDocument* doc = this->document();
    QTextBlock block   = doc->findBlockByLineNumber( 1 );
    return block.text();
}

void Displayer::setExpression( const QString& expr ) {
    QTextCursor cursor = this->textCursor();
    cursor.movePosition( QTextCursor::Start );

    // 选中第一行
    cursor.movePosition( QTextCursor::EndOfLine, QTextCursor::KeepAnchor );

    // 替换文本
    cursor.removeSelectedText();
    cursor.insertText( expr );
}

void Displayer::setResult( const QString& result ) {
    QTextCursor cursor = this->textCursor();
    cursor.movePosition( QTextCursor::Start );
    cursor.movePosition( QTextCursor::Down );

    // 选中第二行
    cursor.movePosition( QTextCursor::EndOfLine, QTextCursor::KeepAnchor );

    // 替换文本
    cursor.removeSelectedText();
    cursor.insertText( result );
}
