//
// Created by dingjing on 23-6-15.
//

#ifndef UDISKS_SCANNER_RESULT_ITEM_H
#define UDISKS_SCANNER_RESULT_ITEM_H
#include <QObject>

class ResultItem : public QObject
{
    Q_OBJECT
public:
    ResultItem(QString id, QString fileName, QString filePath, qint64 fileTime, QObject*parent=nullptr);

    void setID (QString id);
    void setFileName (QString fileName);
    void setFilePath (QString filePath);
    void setFileTime (qint64 fileTime);

    QString getID();
    QString getFileName();
    QString getFilePath();
    qint64 getFileTime();
    QString getFileTimeStr();

private:
    QString                 mID;
    QString                 mFileName;
    QString                 mFilePath;
    qint64                  mFileTime;
};


#endif //UDISKS_SCANNER_RESULT_ITEM_H
