#ifndef SCANNERTASKITEM_H
#define SCANNERTASKITEM_H

#include <QObject>

class ScannerTaskItem : public QObject
{
    Q_OBJECT
public:
    // 未知 -- 未开始、扫描中、已停止、已完成、已暂停、扫描发生错误
    // -- 任务状态：0 - 未开始，1 - 扫描中，2 - 已停止，3 - 已完成, 4 - 已暂停, 5 - 继续扫描, 13 - 扫描发生错误
    enum Status { Unknow = -1, NoBegin = 0, Scanning, Stop, Finish, Suspended = 4, Continue = 5, Error = 13 }; Q_ENUM(Status)
public:
    explicit ScannerTaskItem(QObject *parent = nullptr);
    ScannerTaskItem(QString id, QString name, QStringList policy, QString scanPath, qint64 startTime, Status status, float scanProgress, QObject* p = nullptr);

    void setID (QString id);
    void setName (QString name);
    void setPolicy (QString policy);
    void setPolicy (QStringList policy);
    void setScanPath (QString scanPath);
    void setScanOutPath (QString scanOutPath);
    void setStartTime (qint64 startTime);
    void setAllFile (qint64 f);
    void setFinishedFile (qint64 f);
    void setStatus (int status);
    void setStatus (Status status);
    void setScanProgress(float scanProgress);
    void setIsChecked(bool c);

    [[nodiscard]] QString getID ();
    [[nodiscard]] QString getName ();
    [[nodiscard]] QStringList getPolicy ();
    [[nodiscard]] QString getScanPath ();
    [[nodiscard]] QString getScanOutPath ();
    [[nodiscard]] qint64 getStartTime () const;
    [[nodiscard]] QString getStartTimeStr () const;
    [[nodiscard]] Status getStatus ();
    [[nodiscard]] int getFinishedFile();
    [[nodiscard]] int getAllFile();
    [[nodiscard]] float getScanProgress();
    [[nodiscard]] bool getIsChecked() const;

private:
    QString                 mID;
    QString                 mName;
    QStringList             mPolicy;
    QStringList             mScanPath;
    QStringList             mScanOutPath;
    qint64                  mStartTime;
    Status                  mStatus;

    qint64                  mAllFile;
    qint64                  mFinishedFile;
    float                   mScanProgress;

    bool                    mIsChecked = false;
};

#endif // SCANNERTASKITEM_H
