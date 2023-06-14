//
// Created by dingjing on 23-5-6.
//

#ifndef ESAFE_DSM_MESSAGE_BOX_H
#define ESAFE_DSM_MESSAGE_BOX_H
#include <QMessageBox>
#include <QVBoxLayout>

class MessageBox : public QDialog
{
    Q_OBJECT
public:
    explicit MessageBox(const QString& title, const QString& text, QWidget* p= nullptr);

    QPushButton* addButton (const QString& str);

protected:

private:
    QVBoxLayout*            mMainLayout = nullptr;
    QHBoxLayout*            mBtnLayout = nullptr;
};


#endif //ESAFE_DSM_MESSAGE_BOX_H
