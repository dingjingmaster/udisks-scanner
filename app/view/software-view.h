//
// Created by dingjing on 23-9-27.
//

#ifndef UDISKS_SCANNER_SOFTWARE_VIEW_H
#define UDISKS_SCANNER_SOFTWARE_VIEW_H
#include <QTableView>

class SoftwareView : public QTableView
{
    Q_OBJECT
public:
    explicit SoftwareView(QTableView* parent=nullptr);

};


#endif //UDISKS_SCANNER_SOFTWARE_VIEW_H
