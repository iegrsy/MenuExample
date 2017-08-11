#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QStringList>
#include <QListWidgetItem>

#include <QJsonParseError>
#include <QDragEnterEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString readFile(QString path);
    void readJson(QString str);
    void showObj(QJsonObject obj);
    void updateListMenu();
    bool eventFilter(QObject *obj, QEvent *event);
    void intoObject(QJsonObject obj);
    void backMenu();
    void valueChange(QJsonObject obj, QString array_name, int index);
private:
    Ui::MainWindow *ui;
    QJsonDocument jDoc;
    QJsonObject currentObj;
    QList<QJsonObject> prevObj;
};

#endif // MAINWINDOW_H
