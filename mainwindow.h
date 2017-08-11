#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QStringList>
#include <QListWidgetItem>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>

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

    void modifyJsonValue(QJsonValue &destValue, const QString &path, const QJsonValue &newValue);
    void modifyJsonValue(QJsonDocument &doc, const QString &path, const QJsonValue &newValue);
    void updateJsonAndMenu();
    void setJsonSetLine();
    QString editLineDialog(QString val);
    bool editBoolDiaolg(bool val);
    QString editComboBoxDialog(QString str, int index, int &i);
protected slots:
    void init();
private:
    Ui::MainWindow *ui;
    QString path;
    QString jsonSetLine;

    QJsonDocument jDoc;
    QJsonObject currentObj;
    QStringList folderTrace;
    QList<QJsonObject> prevObj;
};

#endif // MAINWINDOW_H
