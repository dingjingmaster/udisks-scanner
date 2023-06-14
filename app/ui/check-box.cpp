//
// Created by dingjing on 23-4-26.
//

#include "check-box.h"

CheckBox::CheckBox(QWidget *parent) : QCheckBox (parent)
{

}

CheckBox::CheckBox(QString label, QWidget *parent)
    : QCheckBox (parent)
{
    setText (label);
}
