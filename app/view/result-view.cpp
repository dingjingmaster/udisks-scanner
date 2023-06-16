//
// Created by dingjing on 23-6-15.
//

#include "result-view.h"

ResultView::ResultView(QWidget *parent)
    : QTableView(parent)
{
    setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::NoSelection);
}
