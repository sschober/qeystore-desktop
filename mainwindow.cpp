#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_newEntry.h"
#include <QDir>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QInputDialog>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionNewEntry, SIGNAL(triggered()), this, SLOT(newEntry()));
    db = init_db();
}

QSqlDatabase MainWindow::init_db(){
    // init home dir path
    QString dotDir = QDir::toNativeSeparators(QDir::home().path()) + QDir::separator() + ".qeystore";
    QString dotDbFile = dotDir + QDir::separator() + "qeystore.db";
    if(!QDir::home().mkpath(".qeystore")){
        qDebug() << "Could not create dot dir: " + dotDir;
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dotDbFile);
    if(!db.open()){
        qDebug() << "Could not open database: " << db.lastError().text();
    }
    QString pwd = QInputDialog::getText(this,"Store password","Please enter the store password:",QLineEdit::Password);

    // Set password
    QSqlQuery query = db.exec(QString("PRAGMA KEY='%1'").arg(pwd));
    if(query.lastError().isValid()){
        qDebug() << "Could not set password: " << query.lastError().text();
    }

    if(!db.tables().contains("passwords")){
        qDebug() << "Creating  table `passwords`...";
        query = db.exec("CREATE TABLE passwords(url text, username text, password test);");
        if(query.lastError().isValid()){
            qDebug() << "Could not create table: " << query.lastError().text();
        }
    }
    qDebug() << "Database file " << dotDbFile << " created/opened";
    QSqlQueryModel *m = new QSqlQueryModel;
    m->setQuery("SELECT * FROM passwords;");
    ui->tableView->setModel(m);
    return db;
}

void MainWindow::newEntry(){
    Ui::NewEntry *ne = new Ui::NewEntry();
    QDialog d;
    ne->setupUi(&d);
    if(d.exec()){
        QSqlQuery q;
        q.prepare("INSERT INTO passwords(url, username, password) VALUES(:url, :username, :password);");
        q.bindValue(":url", ne->le_URL->text());
        q.bindValue(":username",ne->le_Username->text());
        q.bindValue(":password",ne->le_Password->text());
        if(!q.exec()){
            qDebug() << "Could not insert entry: " << q.lastError().text();
        }
        ((QSqlQueryModel*)ui->tableView->model())->setQuery("SELECT * FROM passwords;");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
