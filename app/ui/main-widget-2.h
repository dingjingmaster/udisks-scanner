//
// Created by dingjing on 23-6-13.
//

#ifndef UDISKS_SCANNER_MAIN_WIDGET_2_H
#define UDISKS_SCANNER_MAIN_WIDGET_2_H
#include <QWidget>
#include <QVBoxLayout>


class ResultView;
class ResultModel;
class MainWidget2 : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget2(QWidget* parent=nullptr);

Q_SIGNALS:
    void showTaskList();

private:
    QVBoxLayout*            mMainLayout = nullptr;
    ResultView*             mView = nullptr;
    ResultModel*            mModel = nullptr;

};


#endif //UDISKS_SCANNER_MAIN_WIDGET_2_H
