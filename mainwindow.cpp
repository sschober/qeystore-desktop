#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_newEntry.h"
#include <QDir>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlField>
#include <QInputDialog>
#include <QLineEdit>
#include <QClipboard>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionNewEntry, SIGNAL(triggered()), this, SLOT(newEntry()));
    connect(ui->actionChange_Password, SIGNAL(triggered()),this, SLOT(changePassword()));

    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(tableViewClicked(QModelIndex)));

    db = init_db();
}

QSqlDatabase MainWindow::init_db(){
    QSettings settings;
    QString dbPath =
            settings.value(
                "db/path",
                //QDir::toNativeSeparators(QDir::home().path()) + QDir::separator() + ".qeystore"
                "").value<QString>();
    if(dbPath == ""){
        dbPath = QFileDialog::getExistingDirectory(this,"Select directory for keystore database");
    }
    QString dbFileName = settings.value("db/file", "qeystore.db").value<QString>();
    QString dotDbFile = dbPath + QDir::separator() + dbFileName;

    // open database file
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dotDbFile);
    if(!db.open()){
        qDebug() << "Could not open database: " << db.lastError().text();
    }

    // ask user for password
    QString pwd = QInputDialog::getText(this,"Store password","Please enter the store password:",QLineEdit::Password);

    // Set password
    QSqlQuery query = db.exec(QString("PRAGMA KEY='%1'").arg(pwd));
    if(query.lastError().isValid()){
        qDebug() << "Could not set password: " << query.lastError().text();
    }

    // create schema if it doesn't exist
    if(!db.tables().contains("passwords")){
        qDebug() << "Creating  table `passwords`...";
        query = db.exec("CREATE TABLE passwords(url text, username text, password test);");
        if(query.lastError().isValid()){
            qDebug() << "Could not create table: " << query.lastError().text();
        }
    }
    qDebug() << "Database file " << dotDbFile << " created/opened";

    // init table view
    QSqlQueryModel *m = new QSqlQueryModel;
    m->setQuery("SELECT * FROM passwords;");
    ui->tableView->setModel(m);

    // save settings
    settings.setValue("db/path",dbPath);
    settings.setValue("db/file",dbFileName);

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

void MainWindow::tableViewClicked(const QModelIndex &idx){
    qDebug() << "Row " << idx.row() << " clicked";
    QString pwd = ((QSqlQueryModel*)ui->tableView->model())->record(idx.row()).field("password").value().toString();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(pwd);
    clipboard->setText(pwd,QClipboard::Selection);
    qDebug() << "Password copied to clipboard.";
}

void MainWindow::changePassword() {
    // ask user for password
    QString pwd = QInputDialog::getText(this,"New Store password","Please enter the new store password:",QLineEdit::Password);
    QString pwd2 = QInputDialog::getText(this,"New Store password (reentry)","Please enter the new store password again:",QLineEdit::Password);
    if(pwd == pwd2){
        // Set password
        QSqlQuery query = db.exec(QString("PRAGMA REKEY='%1'").arg(pwd));
        if(query.lastError().isValid()){
            qDebug() << "Could not set password: " << query.lastError().text();
        }
    }
    else{
        QMessageBox::warning(this,"Passwords did not match!","Passwords did not match! Password unchanged");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
