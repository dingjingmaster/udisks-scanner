//
// Created by dingjing on 23-4-26.
//

#ifndef ESAFE_DSM_CHECK_BOX_H
#define ESAFE_DSM_CHECK_BOX_H
#include <QCheckBox>

class CheckBox : public QCheckBox
{
    Q_OBJECT
public:
    explicit CheckBox(QWidget* parent = nullptr);
    explicit CheckBox(QString label, QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *e) override {};
    void mouseReleaseEvent(QMouseEvent *e) override {};
};


#endif //ESAFE_DSM_CHECK_BOX_H
