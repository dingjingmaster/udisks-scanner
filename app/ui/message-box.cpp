//
// Created by dingjing on 23-5-6.
//

#include "message-box.h"

#include <QLabel>
#include <QPushButton>


QPushButton *MessageBox::addButton(const QString &str)
{
    auto* btn = new QPushButton(str, this);

    mBtnLayout->addWidget (btn);

    return btn;
}

MessageBox::MessageBox(const QString &title, const QString &text, QWidget *p)
    : QDialog (p)
{
    setMinimumSize(380, 200);

    setWindowTitle (title);

    mMainLayout = new QVBoxLayout;

    auto l1 = new QVBoxLayout;
    mBtnLayout = new QHBoxLayout;
    mBtnLayout->setSpacing (8);
    mBtnLayout->setAlignment (Qt::AlignCenter);

    auto w = new QLabel;
    w->setText (text);
    l1->addWidget (w);

    mMainLayout->addItem (l1);
    mMainLayout->addStretch();
    mMainLayout->addItem (mBtnLayout);

    setLayout (mMainLayout);
}

