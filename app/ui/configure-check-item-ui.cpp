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
    layout->addWidget (nameLabel, 0, 0, 12, 12, Qt::AlignLeft | Qt::AlignVCenter);

    {
        auto label = new QLabel("类别:");
        auto value = new QLabel(category);
        layout->addWidget (label, 1,  0, 1, 3, Qt::AlignRight | Qt::AlignVCenter);
        layout->addWidget (value, 1,  2, 1, 8, Qt::AlignLeft  | Qt::AlignVCenter);
    }

    {
        auto label = new QLabel("等级:");
        auto value = new QLabel(level);
        layout->addWidget (label, 2,  0, 1, 3, Qt::AlignRight | Qt::AlignVCenter);
        layout->addWidget (value, 2,  2, 1, 8, Qt::AlignLeft  | Qt::AlignVCenter);
    }

    {
        auto label = new QLabel("检查方法:");
        auto value = new QLabel(checkMethod);
        layout->addWidget (label, 3,  0, 1, 3, Qt::AlignRight | Qt::AlignVCenter);
        layout->addWidget (value, 3,  2, 1, 8, Qt::AlignLeft  | Qt::AlignVCenter);
    }

    {
        auto label = new QLabel("修复建议:");
        auto value = new QLabel(repairMethod);
        layout->addWidget (label, 4,  0, 1, 3, Qt::AlignRight | Qt::AlignVCenter);
        layout->addWidget (value, 4,  2, 1, 8, Qt::AlignLeft  | Qt::AlignVCenter);
    }

    setLayout (layout);
}
