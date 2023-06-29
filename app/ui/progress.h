//
// Created by dingjing on 6/28/23.
//

#ifndef DSIP_SIT_PROGRESS_H
#define DSIP_SIT_PROGRESS_H

#include <QColor>
#include <QWidget>
#include <QtWidgets/QLabel>
#include <QGraphicsBlurEffect>
#include <QtWidgets/QVBoxLayout>

class QProgressIndicator;

class Progress : public QWidget
{
    Q_OBJECT
public:
    explicit Progress (QWidget* parent = nullptr);

private:
    void newParent ();

    void updateLabelPosition();

Q_SIGNALS:
    void updateProcess (int cur, int total);

protected:
    bool event (QEvent* ev) override;
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter (QObject* obj, QEvent* ev) override;

private:
    const int                   mLabelWidth = 400;
    const int                   mLabelHeight = 80;
    QLabel*                     mInfo;
    QProgressIndicator*         mProcess;
    QVBoxLayout*                mMainLayout{};
    QWidget*                    mMainWidget{};
};

class QProgressIndicator : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(int delay READ animationDelay WRITE setAnimationDelay)
    Q_PROPERTY(bool displayedWhenStopped READ isDisplayedWhenStopped WRITE setDisplayedWhenStopped)
public:
    QProgressIndicator(QWidget* parent = 0);

    int animationDelay() const { return mDelay; }

    bool isAnimated () const;


    bool isDisplayedWhenStopped() const;

    const QColor & color() const { return mColor; }

    virtual QSize sizeHint() const;
    int heightForWidth(int w) const;
public Q_SLOTS:

    void startAnimation();

    void stopAnimation();

    void setAnimationDelay(int delay);

    void setDisplayedWhenStopped(bool state);

    void setColor(const QColor & color);
protected:
    virtual void timerEvent(QTimerEvent * event);
    virtual void paintEvent(QPaintEvent * event);
private:
    int                     mAngle;
    int                     mDelay;
    int                     mTimerId;
    QColor                  mColor;
    bool                    mDisplayedWhenStopped;
};

#endif //DSIP_SIT_PROGRESS_H
