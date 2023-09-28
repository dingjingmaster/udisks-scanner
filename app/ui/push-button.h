#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QWidget>

class QLabel;
class QVBoxLayout;
class PushButton : public QWidget
{
    Q_OBJECT
public:
    enum Type { Type1, Type2, Type3, Type4 };
public:
    explicit PushButton(QWidget *parent = nullptr, Type t=Type1);

    void setText (QString text);
    bool isEnable ();
    void setEnable();
    void setType(Type type);
    
public Q_SLOTS:
    void setEnable(bool);

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

Q_SIGNALS:
    void clicked ();
    void enable(bool);
    void doubleClicked ();

private:
    const int           mMaxWidth = 120;
    const int           mLabelMargin = 6;
    
    bool                mIsEnable = false;

    QLabel*             mLabel;
    QVBoxLayout*        mLayout;

    QString             mText;

    Type                mType;

    // private
    QWidget*            mPLine = nullptr;
};

#endif // PUSHBUTTON_H
