//
// Created by dingjing on 23-9-27.
//

#include "software-view.h"

#include <QDebug>
#include <QScrollBar>
#include <QHeaderView>
#include <QFontMetrics>
#include <QApplication>


SoftwareView::SoftwareView(QTableView *parent)
    : QTableView (parent)
{
    setMinimumHeight (260);
    setMouseTracking(true);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::NoSelection);

    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}
