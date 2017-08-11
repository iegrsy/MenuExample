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
            item->setIcon(QIcon("/home/ieg/Resimler/icons/006-folder.png"));
            item->setText(s);

            ui->lstMenu->addItem(item);
        }
        else if(obj[s].isArray())
        {
            //            ui->lstMenu->addItem(s);
            for (int i=0; i<obj[s].toArray().count() ; i++)
            {
                QListWidgetItem *item = new QListWidgetItem;
                item->setIcon(QIcon("/home/ieg/Resimler/icons/009-settings.png"));
                item->setText(obj[s].toArray().at(i).toObject()["name"].toString());

                ui->lstMenu->addItem(item);
            }
        }
    }
    if(prevObj.count() > 0)
    {
        QListWidgetItem *item = new QListWidgetItem;
        item->setIcon(QIcon("/home/ieg/Resimler/icons/001-arrows.png"));
        item->setText("back_menu");

        ui->lstMenu->addItem(item);
    }
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

//void MainWindow::optionsClick(QString)
//{

//}

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

                    return false;
                }
                else if (currentObj[selectItem].isArray())
                {
                    qDebug()<<currentObj[selectItem];
                    return false;
                }
                else
                {
                    foreach (QString s, currentObj.keys()) {
                        if(currentObj[s].isArray() && s.compare("_options") == 0){
                            for(int i = 0; i < currentObj[s].toArray().count(); i++)
                            {
                                QString temp = currentObj[s].toArray().at(i)
                                        .toObject()["name"].toString();

                                if(selectItem.compare(temp) == 0)
                                {
                                    valueChange(currentObj, s, i);
                                }
                            }
                        }
                    }
                }
                if (selectItem.compare("back_menu") == 0)
                {
                    backMenu();
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

void MainWindow::valueChange(QJsonObject obj, QString array_name, int index)
{
    QJsonArray array = obj[array_name].toArray();

    //    qDebug()<<array.at(index).toObject()["name"];
    //    qDebug()<<array.at(index).toObject()["type"];
    //    qDebug()<<array.at(index).toObject()["value"];

    if(array.at(index).toObject()["type"].toString().compare("boolean") == 0)
    {
        qDebug()<<"type: bool";
    }
    else if (array.at(index).toObject()["type"].toString().compare("string") == 0)
    {
        qDebug()<<"type: string";
    }
    else if (array.at(index).toObject()["type"].toString().compare("enum") == 0)
    {
        qDebug()<<"type: enum";
    }
    else
    {

    }


}






















