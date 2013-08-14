#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void newEntry();

private:
    QSqlDatabase init_db();
    Ui::MainWindow *ui;
    QSqlDatabase db;

};

#endif // MAINWINDOW_H
