#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_StartButton_clicked();
    void on_CellButton_clicked(int n0, int n1);
    void updateTimer();

    void on_SaveButton_clicked();

    void on_LoadButton_clicked();

    void on_RatingButton_clicked();

private:
    Ui::MainWindow *ui;
    int elapsedTime;
    QTimer *timer;
    int **dynamicArray;
    int ROW;
    int COL;
    int mines;
    bool start;
    bool win;
    void GenerateMatrix(int n0, int n1, int ROW, int COL, int **arr, int mines);
    void Func(int n0, int n1, int ROW, int COL, int **arr);
    void BlockButtons();
    void saveElapsedTime();
};

#endif // MAINWINDOW_H
