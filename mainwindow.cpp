#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    path("/home/ieg/qt/qtProjects/MenuExample/settings.json")
{
    ui->setupUi(this);
    ui->lstMenu->installEventFilter(this);

    //QTimer::singleShot(0, this, SLOT(init()));
    init();
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
    folderTrace.clear();
    prevObj.clear();
    jsonSetLine.clear();
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

                if((obj[s].toArray().at(i).toObject()["type"].toString()).compare("boolean") == 0)
                {
                    if(obj[s].toArray().at(i).toObject()["value"].toBool())
                    {
                        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
                        item->setCheckState(Qt::Checked);
                    }
                    else
                    {
                        item->setCheckState(Qt::Unchecked);
                    }
                }

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
                    folderTrace.append(selectItem);

                    return false;
                }
                else if (currentObj[selectItem].isArray())
                {
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
                    folderTrace.removeLast();
                }
            }

            return false;
        }
    }
    else
    {
        return QMainWindow::eventFilter(obj, event);
    }

    return false;
}

void MainWindow::setJsonSetLine()
{
    jsonSetLine.clear();
    foreach (QString s, folderTrace) {
        jsonSetLine.append(s).append(".");
    }
}

void MainWindow::valueChange(QJsonObject obj, QString array_name, int index)
{
    QJsonArray array = obj[array_name].toArray();

    if(array.at(index).toObject()["type"].toString().compare("boolean") == 0)
    {
        setJsonSetLine();

        if(array.at(index).toObject()["value"].toBool() == true)
        {
            modifyJsonValue(jDoc, QString("%1_options[%2].value").arg(jsonSetLine).arg(index),false);
        }
        else if (array.at(index).toObject()["value"].toBool() == false)
        {
            modifyJsonValue(jDoc, QString("%1_options[%2].value").arg(jsonSetLine).arg(index),true);
        }

        updateJsonAndMenu();
    }
    else if (array.at(index).toObject()["type"].toString().compare("string") == 0)
    {
        setJsonSetLine();
        modifyJsonValue(jDoc, QString("%1_options[%2].value").arg(jsonSetLine).arg(index),
                        editLineDialog(array.at(index).toObject()["value"].toString()));
        updateJsonAndMenu();
    }
    else if (array.at(index).toObject()["type"].toString().compare("enum") == 0)
    {
        setJsonSetLine();
        int ix;

        modifyJsonValue(jDoc, QString("%1_options[%2].value").arg(jsonSetLine).arg(index),
                        editComboBoxDialog(array.at(index).toObject()["values"].toString(),
                        (int)array.at(index).toObject()["value_index"].toDouble(), ix));
        modifyJsonValue(jDoc, QString("%1_options[%2].value_index").arg(jsonSetLine).arg(index),ix);
        updateJsonAndMenu();
    }
    else
    {

    }
}

void MainWindow::updateJsonAndMenu()
{
    QFile f(path);
    if(!f.open(QIODevice::WriteOnly | QIODevice::Text | QFile::Truncate))
        return;

    f.write(QJsonDocument(jDoc).toJson());
    f.close();


    readJson(readFile(path));
    updateListMenu();
}

void MainWindow::modifyJsonValue(QJsonValue& destValue, const QString& path, const QJsonValue& newValue)
{
    const int indexOfDot = path.indexOf('.');
    const QString dotPropertyName = path.left(indexOfDot);
    const QString dotSubPath = indexOfDot > 0 ? path.mid(indexOfDot + 1) : QString();

    const int indexOfSquareBracketOpen = path.indexOf('[');
    const int indexOfSquareBracketClose = path.indexOf(']');

    const int arrayIndex = path.mid(indexOfSquareBracketOpen + 1, indexOfSquareBracketClose - indexOfSquareBracketOpen - 1).toInt();

    const QString squareBracketPropertyName = path.left(indexOfSquareBracketOpen);
    const QString squareBracketSubPath = indexOfSquareBracketClose > 0 ? (path.mid(indexOfSquareBracketClose + 1)[0] == '.' ? path.mid(indexOfSquareBracketClose + 2) : path.mid(indexOfSquareBracketClose + 1)) : QString();

    // determine what is first in path. dot or bracket
    bool useDot = true;
    if (indexOfDot >= 0) // there is a dot in path
    {
        if (indexOfSquareBracketOpen >= 0) // there is squarebracket in path
        {
            if (indexOfDot > indexOfSquareBracketOpen)
                useDot = false;
            else
                useDot = true;
        }
        else
            useDot = true;
    }
    else
    {
        if (indexOfSquareBracketOpen >= 0)
            useDot = false;
        else
            useDot = true; // acutally, id doesn't matter, both dot and square bracket don't exist
    }

    QString usedPropertyName = useDot ? dotPropertyName : squareBracketPropertyName;
    QString usedSubPath = useDot ? dotSubPath : squareBracketSubPath;

    QJsonValue subValue;
    if (destValue.isArray())
        subValue = destValue.toArray()[usedPropertyName.toInt()];
    else if (destValue.isObject())
        subValue = destValue.toObject()[usedPropertyName];
    else
        qDebug() << "oh, what should i do now with the following value?! " << destValue;

    if(usedSubPath.isEmpty())
    {
        subValue = newValue;
    }
    else
    {
        if (subValue.isArray())
        {
            QJsonArray arr = subValue.toArray();
            QJsonValue arrEntry = arr[arrayIndex];
            modifyJsonValue(arrEntry,usedSubPath,newValue);
            arr[arrayIndex] = arrEntry;
            subValue = arr;
        }
        else if (subValue.isObject())
            modifyJsonValue(subValue,usedSubPath,newValue);
        else
            subValue = newValue;
    }

    if (destValue.isArray())
    {
        QJsonArray arr = destValue.toArray();
        arr[arrayIndex] = subValue;
        destValue = arr;
    }
    else if (destValue.isObject())
    {
        QJsonObject obj = destValue.toObject();
        obj[usedPropertyName] = subValue;
        destValue = obj;
    }
    else
        destValue = newValue;
}

void MainWindow::modifyJsonValue(QJsonDocument& doc, const QString& path, const QJsonValue& newValue)
{
    QJsonValue val;
    if (doc.isArray())
        val = doc.array();
    else
        val = doc.object();

    modifyJsonValue(val,path,newValue);

    if (val.isArray())
        doc = QJsonDocument(val.toArray());
    else
        doc = QJsonDocument(val.toObject());
}

void MainWindow::init()
{
    QString jData = readFile(path);

    readJson(jData);
    updateListMenu();
}

QString MainWindow::editLineDialog(QString str)
{
    QDialog dlg(this);
    QLabel *label = new QLabel("Edit setting:");
    QLineEdit *editline = new QLineEdit;
    editline->setText(str);
    connect(editline, SIGNAL(returnPressed()), &dlg, SLOT(close()));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(label, 0, 0);
    layout->addWidget(editline, 1, 0);

    dlg.setLayout(layout);
    dlg.exec();

    qDebug()<<editline->text();
    return editline->text();
}

QString MainWindow::editComboBoxDialog(QString str,int index, int &i)
{
    QStringList items=str.split(",");

    QDialog dlg(this);
    QLabel *label = new QLabel("Edit setting:");
    QComboBox *cbox = new QComboBox;
    cbox->addItems(items);
    cbox->setCurrentIndex(index);
    cbox->setEditable(true);
    cbox->lineEdit()->setReadOnly(true);
    connect(cbox->lineEdit(), SIGNAL(returnPressed()), &dlg, SLOT(close()));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(label, 0, 0);
    layout->addWidget(cbox, 1, 0);

    dlg.setLayout(layout);
    dlg.exec();

    qDebug()<<cbox->itemText(cbox->currentIndex());
    i=cbox->currentIndex();
    return cbox->itemText(cbox->currentIndex());
}












