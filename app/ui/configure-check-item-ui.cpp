//
// Created by dingjing on 23-10-11.
//

#include "configure-check-item-ui.h"

#include <QLabel>
#include <QGridLayout>


ConfigureCheckItemUI::ConfigureCheckItemUI(
    const QString &name,
    const QString &category,
    const QString &level,
    const QString &checkMethod,
    const QString &repairMethod,
    QWidget *parent) : QWidget (parent)
{
    auto layout = new QGridLayout;

    auto nameLabel = new QLabel(name);
    layout->addWidget (nameLabel, 1, 1, 1, 10,  Qt::AlignLeft | Qt::AlignVCenter);

    {
        auto label = new QLabel("类别:");
        auto value = new QLabel(category);
        layout->addWidget (label, 2,  1, 1, 2,  Qt::AlignRight | Qt::AlignVCenter);
        layout->addWidget (value, 2,  3, 1, 10, Qt::AlignLeft  | Qt::AlignVCenter);
    }

    {
        auto label = new QLabel("等级:");
        auto value = new QLabel(level);
        layout->addWidget (label, 3,  1, 1, 2,  Qt::AlignRight | Qt::AlignVCenter);
        layout->addWidget (value, 3,  3, 1, 10, Qt::AlignLeft  | Qt::AlignVCenter);
    }

    {
        auto label = new QLabel("检查方法:");
        auto value = new QLabel(checkMethod);
        layout->addWidget (label, 4,  1, 1, 2,  Qt::AlignRight | Qt::AlignVCenter);
        layout->addWidget (value, 4,  3, 1, 10, Qt::AlignLeft  | Qt::AlignVCenter);
    }

    {
        auto label = new QLabel("修复建议:");
        auto value = new QLabel(repairMethod);
        layout->addWidget (label, 5,  1, 1, 2,  Qt::AlignRight | Qt::AlignVCenter);
        layout->addWidget (value, 5,  3, 1, 10, Qt::AlignLeft  | Qt::AlignVCenter);
    }

    setLayout (layout);
}
