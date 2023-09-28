//
// Created by dingjing on 2/7/23.
//

#include "main-window.h"

#include "log.h"
#include "main-header.h"
#include "message-box.h"
#include "push-button.h"
#include "main-widget-11.h"
#include "main-widget-12.h"
#include "main-widget-21.h"
#include "singleton-app-gui.h"

#include <QScreen>
#include <QLayout>
#include <QApplication>


MainWindow::MainWindow (QWidget *parent)
    : QWidget (parent)
{
    setMouseTracking (true);
    setObjectName ("window-main");
    setContentsMargins (0, 0, 0, 0);
    setMinimumSize (mWidth, mHeight);
    setWindowFlag (Qt::FramelessWindowHint);

    mDirection = NONE;

    mMainLayout = new QVBoxLayout;
    auto llB = new QHBoxLayout;                     // nav btn
    auto layout2 = new QVBoxLayout;                 // 敏感数据检查
    auto layout3 = new QVBoxLayout;                 // 脆弱性检查
    layout2->setContentsMargins(2, 2, 2, 2);
    mMainLayout->setContentsMargins (0, 0, 0, 0);

    auto minGan = new PushButton;
    auto cuiRuo = new PushButton;
    minGan->setText ("敏感数据检查");
    cuiRuo->setText ("脆弱性检查");
    minGan->setEnable(true);
    cuiRuo->setEnable(true);

    llB->addWidget (minGan);
    llB->addWidget (cuiRuo);

    mHeader = new MainHeader;
    mHeader->setTitle ("重要数据安全检查");
    mMainLayout->addWidget(mHeader);

    mHeader->showMin();
    mHeader->showMax();

    mMainLayout->addItem (llB);

    mWindow11 = new MainWidget11;
    mWindow12 = new MainWidget12;

    layout2->addWidget (mWindow11);
    layout2->addWidget (mWindow12);

    mWindow12->hide();

    mWindow21 = new MainWidget21;
    layout3->addWidget (mWindow21);

    mMainLayout->addItem (layout2);
    mMainLayout->addItem (layout3);

    setLayout (mMainLayout);

    connect (mHeader, &MainHeader::windowClose, this, [=] () {
        QApplication::exit(0);
    });

    connect (mHeader, &MainHeader::windowMin, this, [=] () {
        setWindowState (Qt::WindowMinimized);
    });

    connect (mHeader, &MainHeader::windowMax, this, [=] () {
        if (isMaximized()) {
            setWindowState (Qt::WindowNoState);
        }
        else {
            setWindowState (Qt::WindowMaximized);
        }
    });

    connect (this, &MainWindow::resizeWidgets, this, [=] () {

    });

    connect (mWindow11, &MainWidget11::taskDetail, this, [=] (const QString& id) {
        mWindow11->hide();
        mWindow12->showResult(id);
    });

    connect (mWindow12, &MainWidget12::showTaskList, this, [=] () {
        mWindow12->hideResult();
        mWindow11->show();
    });

    connect (minGan, &PushButton::clicked, this, [=] () {
        mWindow21->hide();
        if (mWindow11->isHidden()) {
            Q_EMIT mWindow12->showTaskList();
        }
        minGan->setType (PushButton::Type1);
        cuiRuo->setType (PushButton::Type4);
    });

    connect (cuiRuo, &PushButton::clicked, this, [=] () {
        mWindow11->hide();
        mWindow12->hideResult();
        mWindow21->show();

        minGan->setType (PushButton::Type4);
        cuiRuo->setType (PushButton::Type1);
    });

    Q_EMIT minGan->clicked ();
}

void MainWindow::mouseMoveEvent(QMouseEvent* e)
{
    QPoint globalPos = e->globalPos();
    QRect rect = this->rect();
    QPoint tl = mapToGlobal (rect.topLeft());
    QPoint rb = mapToGlobal (rect.bottomRight());

    qreal dpiRatio = qApp->devicePixelRatio ();
    if (!mIsPress) {
        region (globalPos);
    }

    if (!(Qt::LeftButton & e->buttons())) {
        return;
    }

    {
        if (NONE != mDirection) {
            QRect rMove (tl, rb);
            switch (mDirection) {
                case LEFT: {
                    if (rb.x () - globalPos.x () <= this->minimumWidth ()) {
                        rMove.setX (tl.x ());
                    }
                    else {
                        rMove.setX (globalPos.x ());
                    }
                    break;
                }
                case RIGHT: {
                    rMove.setWidth (globalPos.x () - tl.x ());
                    break;
                }
                case UP: {
                    if (rb.y () - globalPos.y () <= this->minimumHeight ()) {
                        rMove.setY (tl.y ());
                    }
                    else {
                        rMove.setY (globalPos.y ());
                    }
                    break;
                }
                case DOWN: {
                    rMove.setHeight (globalPos.y () - tl.y ());
                    break;
                }
                case LEFT_TOP: {
                    if (rb.x () - globalPos.x () <= this->minimumWidth ()) {
                        rMove.setX (tl.x ());
                    }
                    else {
                        rMove.setX (globalPos.x ());
                    }

                    if (rb.y () - globalPos.y () <= this->minimumHeight ()) {
                        rMove.setY (tl.y ());
                    }
                    else {
                        rMove.setY (globalPos.y ());
                    }
                    break;
                }
                case RIGHT_TOP: {
                    rMove.setWidth (globalPos.x () - tl.x ());
                    rMove.setY (globalPos.y ());
                    break;
                }
                case LEFT_BOTTOM: {
                    rMove.setX (globalPos.x ());
                    rMove.setHeight (globalPos.y () - tl.y ());
                    break;
                }
                case RIGHT_BOTTOM: {
                    rMove.setWidth (globalPos.x () - tl.x ());
                    rMove.setHeight (globalPos.y () - tl.y ());
                    break;
                }
                default: {
                    break;
                }
            }
            this->setGeometry(rMove);
            return;
        }
    }

    if (mDrag) {
//        if (QX11Info::isPlatformX11 ()) {
//            Display *display = QX11Info::display ();
//            Atom netMoveResize = XInternAtom (display, "_NET_WM_MOVERESIZE", False);
//            XEvent xEvent;
//            const auto pos = QCursor::pos ();
//
//            memset (&xEvent, 0, sizeof (XEvent));
//            xEvent.xclient.type = ClientMessage;
//            xEvent.xclient.message_type = netMoveResize;
//            xEvent.xclient.display = display;
//            xEvent.xclient.window = this->winId ();
//            xEvent.xclient.format = 32;
//            xEvent.xclient.data.l[0] = pos.x () * dpiRatio;
//            xEvent.xclient.data.l[1] = pos.y () * dpiRatio;
//            xEvent.xclient.data.l[2] = 8;
//            xEvent.xclient.data.l[3] = Button1;
//            xEvent.xclient.data.l[4] = 0;
//
//            XUngrabPointer (display, CurrentTime);
//            XSendEvent (display, QX11Info::appRootWindow (QX11Info::appScreen ()), False,
//                        SubstructureNotifyMask | SubstructureRedirectMask, &xEvent);
//            //XFlush(display);
//            XEvent xevent;
//            memset (&xevent, 0, sizeof (XEvent));
//
//            xevent.type = ButtonRelease;
//            xevent.xbutton.button = Button1;
//            xevent.xbutton.window = this->winId ();
//            xevent.xbutton.x = e->pos ().x () * dpiRatio;
//            xevent.xbutton.y = e->pos ().y () * dpiRatio;
//            xevent.xbutton.x_root = pos.x () * dpiRatio;
//            xevent.xbutton.y_root = pos.y () * dpiRatio;
//            xevent.xbutton.display = display;
//
//            XSendEvent (display, this->effectiveWinId (), False, ButtonReleaseMask, &xevent);
//            XFlush (display);
//
//            if (e->source () == Qt::MouseEventSynthesizedByQt) {
//                if (!MainWindow::mouseGrabber ()) {
//                    this->grabMouse ();
//                    this->releaseMouse ();
//                }
//            }
//            mDrag = false;
//        }
//        else {
            move ((QCursor::pos () - mOffset) * dpiRatio);
//        }
    }
}

void MainWindow::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent (e);
    Q_EMIT resizeWidgets();
}

void MainWindow::mousePressEvent(QMouseEvent* e)
{
    QWidget::mousePressEvent (e);

    if (e->isAccepted()) {
        return;
    }

    switch (e->button()) {
        case Qt::LeftButton: {
            mDrag = true;
            mIsPress = true;
            mOffset = mapFromGlobal (QCursor::pos());
            if (NONE != mDirection) {
                mouseGrabber();
            }
            else {
                mDragPos = e->globalPos() - frameGeometry().topLeft();
            }
            break;
        }
        case Qt::RightButton:
        default:{
            mDrag = false;
            mIsPress = false;
            QWidget::mousePressEvent (e);
        }
    }
}

void MainWindow::region(const QPoint &cursorGlobalPoint)
{
    QRect rect = this->rect();
    QPoint tl = mapToGlobal(rect.topLeft());
    QPoint rb = mapToGlobal(rect.bottomRight());
    int x = cursorGlobalPoint.x();
    int y = cursorGlobalPoint.y();

//    qDebug() << "region -- tl: " << tl << " rb: " << rb << " x: " << x << " b: " << y;

    if (tl.x() + 3 >= x && x >= tl.x() - 3 && tl.y() + 3 >= y && y >= tl.y() - 3) {
        mDirection = LEFT_TOP;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));  // 设置鼠标形状
    }
    else if (x >= rb.x() - 3 && x <= rb.x() + 3 && y >= rb.y() - 3 && y <= rb.y() + 3) {
        mDirection = RIGHT_BOTTOM;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));
    }
    else if (x <= tl.x() + 3 && x >= tl.x() - 3 && y >= rb.y() - 3 && y <= rb.y() + 3) {
        mDirection = LEFT_BOTTOM;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if (x <= rb.x() + 3 && x >= rb.x() - 3 && y >= tl.y() + 3 && y <= tl.y() - 3) {
        mDirection = RIGHT_TOP;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if (x <= tl.x() + 3 && x >= tl.x() - 3) {
        mDirection = LEFT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if (x <= rb.x() + 3 && x >= rb.x() - 3) {
        mDirection = RIGHT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if (y >= tl.y() - 3 && y <= tl.y() + 3) {
        mDirection = UP;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    }
    else if (y <= rb.y() + 3 && y >= rb.y() - 3) {
        mDirection = DOWN;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    }
    else {
        mDirection = NONE;
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton
        || e->button() == Qt::RightButton) {
        mDrag = false;
        mIsPress = false;
        if (NONE != mDirection) {
            releaseMouse();
            setCursor (QCursor(Qt::ArrowCursor));
        }
    }
    else {
        QWidget::mouseReleaseEvent (e);
    }
}

