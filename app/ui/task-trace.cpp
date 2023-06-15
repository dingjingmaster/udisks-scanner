//
// Created by dingjing on 23-6-15.
//

#include "task-trace.h"

#include <QLabel>
#include <QApplication>


TaskTrace::TaskTrace(QWidget *parent)
    : QWidget (parent)
{
    setContentsMargins (0, 0, 0, 0);

    setFixedWidth (mFixedWidth);
    setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto font1 = qApp->font();
    font1.setPointSizeF (font1.pointSizeF() + 3);

    auto font2 = qApp->font();
    font2.setPointSizeF (font2.pointSizeF() - 2);

    mMainLayout = new QVBoxLayout;

    auto label1 = new QLabel;
    label1->setFont (font1);
    label1->setText ("任务跟踪");

    auto p = palette();
    p.setColor (QPalette::Background, Qt::red);

    setAutoFillBackground (true);
    setPalette (p);

    mMainLayout->addWidget (label1);
    mMainLayout->addStretch ();

    setLayout (mMainLayout);
}
