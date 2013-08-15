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
#include <QMouseEvent>

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

    // logic that asks for store password and tries to establish, if that was correct
    while(42){
        bool ok = false;
        // ask user for password
        QString pwd = QInputDialog::getText(this,"Store password","Please enter the store password:",QLineEdit::Password,"",&ok);
        if(!ok) exit(-1);

        // Set password
        QSqlQuery query = db.exec(QString("PRAGMA KEY='%1'").arg(pwd));
        if(query.lastError().isValid()){
            qDebug() << "Could not set password: " << query.lastError().text();
        }


        // create schema if it doesn't exist
        if(!db.tables().contains("passwords")){
            qDebug() << "Creating  table `passwords`...";
            query = db.exec("CREATE TABLE passwords(url text PRIMARY KEY NOT NULL, username text, password test);");
            if(query.lastError().isValid()){
                qDebug() << "Could not create table: " << query.lastError().text();
                continue; // user seems to have supplied the wrong password
            }
        }
        break;
    }
    qDebug() << "Database file " << dotDbFile << " created/opened";

    // init table view
    QSqlQueryModel *m = new QSqlQueryModel;
    m->setQuery("SELECT * FROM passwords;");
    ui->tableView->setModel(m);

    // Hide password column
    ui->tableView->hideColumn(2);

    // Adapt column width to column contents
    ui->tableView->resizeColumnsToContents();

    // Install CTRL-Click event filter
    ui->tableView->viewport()->installEventFilter(this);

    // save settings
    settings.setValue("db/path",dbPath);
    settings.setValue("db/file",dbFileName);

    return db;
}

void MainWindow::refreshTableView(){
    ((QSqlQueryModel*)ui->tableView->model())->setQuery("SELECT * FROM passwords;");

    // Adapt column width to column contents
    ui->tableView->resizeColumnsToContents();
}

void MainWindow::writeEntryToDb(QString url, QString username, QString password){
    QSqlQuery q;
    q.prepare("INSERT INTO passwords(url, username, password) VALUES(:url, :username, :password);");
    q.bindValue(":url", url);
    q.bindValue(":username",username);
    q.bindValue(":password",password);
    if(!q.exec()){
        qDebug() << "Could not insert entry: " << q.lastError().text();
    }
    refreshTableView();
}

void MainWindow::newEntry(){
    Ui::NewEntry *ne = new Ui::NewEntry();
    QDialog d;
    ne->setupUi(&d);
    if(d.exec()){
        writeEntryToDb(ne->le_URL->text(),ne->le_Username->text(),ne->le_Password->text());
    }
}

void MainWindow::tableViewClicked(const QModelIndex &idx){
    if(!editFlag){
        qDebug() << "Row " << idx.row() << " clicked";
        QString pwd = ((QSqlQueryModel*)ui->tableView->model())->record(idx.row()).field("password").value().toString();
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(pwd);
        clipboard->setText(pwd,QClipboard::Selection);
        qDebug() << "Password copied to clipboard.";
    }
    else{
        Ui::NewEntry *ne = new Ui::NewEntry;
        QDialog d;
        ne->setupUi(&d);
        QSqlRecord rec = ((QSqlQueryModel*)ui->tableView->model())->record(idx.row());
        ne->le_URL->setText(rec.field("url").value().toString());
        ne->le_Username->setText(rec.field("username").value().toString());
        ne->le_Password->setText(rec.field("password").value().toString());
        if(d.exec()){
            writeEntryToDb(ne->le_URL->text(),ne->le_Username->text(),ne->le_Password->text());
        }
        editFlag = false;
    }
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

bool MainWindow::eventFilter(QObject *, QEvent *e){
    if(e->type() == QEvent::MouseButtonRelease){
        QMouseEvent *me = static_cast<QMouseEvent*>(e);
        if(me->modifiers() == Qt::ControlModifier){
            qDebug() << "Ctrl+Left Click";
            editFlag = true;
        }
    }
    return false;
}

MainWindow::~MainWindow()
{
    delete ui;
}
