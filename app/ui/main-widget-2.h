//
// Created by dingjing on 23-6-13.
//

#ifndef UDISKS_SCANNER_MAIN_WIDGET_2_H
#define UDISKS_SCANNER_MAIN_WIDGET_2_H
#include <QWidget>
#include <QVBoxLayout>


class QLabel;
class ResultView;
class ResultModel;
class MainWidget2 : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget2(QWidget* parent=nullptr);
    void showResult (const QString& taskID);
    void hideResult ();

protected:
    void resizeEvent(QResizeEvent *event) override;

Q_SIGNALS:
    void showTaskList();

private:
    QString                 mTaskID;
    const int               mItemIdxSize = 90;
    const int               mItemNameSize = 200;
    const int               mItemTimeSize = 200;

    QVBoxLayout*            mMainLayout = nullptr;
    ResultView*             mView = nullptr;
    ResultModel*            mModel = nullptr;

    QLabel*                 mDW = nullptr;
    QLabel*                 mTN = nullptr;
    QLabel*                 mCL = nullptr;
    QLabel*                 mSP = nullptr;
    QLabel*                 mSNP = nullptr;
    QLabel*                 mST = nullptr;

};


#endif //UDISKS_SCANNER_MAIN_WIDGET_2_H
