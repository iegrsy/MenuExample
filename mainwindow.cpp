#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lstMenu->installEventFilter(this);

    QString path = "/home/ieg/qt/qtProjects/MenuExample/settings.json";
    QString jData = readFile(path);

    readJson(jData);
    updateListMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::readFile(QString path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
        return 0;

    QString jdata = file.readAll().data();
    file.close();

    return jdata;
}

void MainWindow::readJson(QString str)
{
    if(str.isEmpty())
        return;

    jDoc = QJsonDocument::fromJson(str.toUtf8());
    currentObj = jDoc.object();
}

void MainWindow::showObj(QJsonObject obj)
{
    ui->lstMenu->clear();
    foreach (QString s, obj.keys())
    {
        if(obj[s].isObject())
        {
            QListWidgetItem *item = new QListWidgetItem;
            item->setIcon(QIcon("/home/ieg/Resimler/icon.png"));
            item->setText(s);

            ui->lstMenu->addItem(item);
        }
        else if(obj[s].isArray())
        {
            ui->lstMenu->addItem(s);
            //            for (int i=0; i<obj[s].toArray().count() ; i++)
            //            {
            //                ui->lstMenu->addItem(obj[s].toArray().at(i).toObject()["name"].toString());
            //            }
        }
    }
    if(prevObj.count() > 0)
        ui->lstMenu->addItem("back_menu");
    ui->lstMenu->setCurrentRow(0);
}

void MainWindow::updateListMenu()
{
    showObj(currentObj);
}

void MainWindow::intoObject(QJsonObject obj){

    prevObj.append(currentObj);

    currentObj = obj;
    updateListMenu();
}

void MainWindow::backMenu()
{
    if(!prevObj.isEmpty()){
        currentObj = prevObj.last();
        prevObj.removeLast();
        showObj(currentObj);
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->lstMenu)
    {
        QListWidget *lstMenu = static_cast<QListWidget*>(obj);

        //Capture key event
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if(keyEvent->key() == Qt::Key_Return)
            {
                QString selectItem = lstMenu->selectedItems().first()->text();
                if(currentObj[selectItem].isObject())
                {
                    intoObject(currentObj[selectItem].toObject());
                }
                else if (currentObj[selectItem].isArray())
                {
                    qDebug()<<selectItem;
                }
                if (selectItem.compare("back_menu") == 0)
                {
                    backMenu();
                    qDebug()<<selectItem;
                }
            }
            return false;
        }
    }
    else
    {
        return QMainWindow::eventFilter(obj, event);
    }
}
