#pragma once

#include <QTextEdit>

class Displayer : public QTextEdit {
    Q_OBJECT

public:
    explicit Displayer( QWidget* parent = nullptr );
    ~Displayer() override;

public:
    void clearDisplay();
    void setExpression( const QString& expr );
    void setResult( const QString& result );
    QString getExpressionLine();
    QString getResultLine();
};
