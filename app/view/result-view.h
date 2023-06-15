//
// Created by dingjing on 23-6-15.
//

#ifndef UDISKS_SCANNER_RESULT_VIEW_H
#define UDISKS_SCANNER_RESULT_VIEW_H
#include <QTableView>
#include <QMouseEvent>

class ResultView : public QTableView
{
    Q_OBJECT
public:
    explicit ResultView(QWidget *parent = nullptr);

};


#endif //UDISKS_SCANNER_RESULT_VIEW_H
