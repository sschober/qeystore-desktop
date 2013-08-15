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
    bool eventFilter(QObject *, QEvent *);
    ~MainWindow();

public slots:
    void newEntry();
    void tableViewClicked(const QModelIndex&);
    void changePassword();


private:
    QSqlDatabase init_db();
    void refreshTableView();
    void writeEntryToDb(QString url, QString username, QString password);

    Ui::MainWindow *ui;
    QSqlDatabase db;
    bool editFlag;
};

#endif // MAINWINDOW_H
