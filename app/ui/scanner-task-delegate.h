//
// Created by dingjing on 23-6-14.
//

#ifndef UDISKS_SCANNER_SCANNER_TASK_DELEGATE_H
#define UDISKS_SCANNER_SCANNER_TASK_DELEGATE_H
#include <QSet>
#include <QRect>
#include <QPainter>
#include <QStyledItemDelegate>

class ScannerTaskDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ScannerTaskDelegate (QObject* parent = nullptr);

    void paint (QPainter* p, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

public:

private:
    // column TaskStatus
    const int               mStatusMargin = 5;
    const int               mStatusHigh = 32;

    qreal                   mFontSize = 0;

    QSet<QRect>             mStatusValidButtons;
};


#endif //UDISKS_SCANNER_SCANNER_TASK_DELEGATE_H
