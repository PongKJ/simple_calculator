#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <simple_calculator/calculator.hpp>

#include "displayer.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow( QWidget* parent = nullptr );
    ~MainWindow() override;

private:
    Displayer* displayer;
    // For 'Ans' function
    double lastResult = 0.0;

private slots:
    void onButtonClicked();
};
