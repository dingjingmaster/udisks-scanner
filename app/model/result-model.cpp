//
// Created by dingjing on 23-6-15.
//

#include "result-model.h"

#include <QObject>

ResultModel::ResultModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

void ResultModel::clearData()
{

}

QModelIndex ResultModel::getIndexByItem(const ResultItem *it, int column)
{
    return QModelIndex ();
}

void ResultModel::test()
{

}

void ResultModel::resetModel()
{

}

void ResultModel::addItem(ResultItem *item)
{

}

void ResultModel::delItem(ResultItem *item)
{

}

void ResultModel::updateItem(ResultItem *item)
{

}

void ResultModel::onScrollbarMoved(float ratio)
{

}

int ResultModel::rowCount(const QModelIndex &parent) const
{
    return 0;
}

int ResultModel::columnCount(const QModelIndex &parent) const
{
    return 0;
}

QVariant ResultModel::data(const QModelIndex &index, int role) const
{
    return QVariant ();
}

QVariant ResultModel::headerData(int section, Qt::Orientation orentation, int role) const
{
    return QAbstractItemModel::headerData (section, orentation, role);
}

QModelIndex ResultModel::index(int row, int column, const QModelIndex &parent) const
{
    return QAbstractTableModel::index (row, column, parent);
}

Qt::ItemFlags ResultModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags (index);
}

bool ResultModel::insertRows(int row, int count, const QModelIndex &parent)
{
    return QAbstractItemModel::insertRows (row, count, parent);
}

bool ResultModel::removeRows(int row, int count, const QModelIndex &parent)
{
    return QAbstractItemModel::removeRows (row, count, parent);
}

ResultModel::~ResultModel()
= default;
