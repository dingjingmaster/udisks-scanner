//
// Created by dingjing on 2/7/23.
//

#include "main-window.h"

#include "log.h"
#include "main-header.h"
#include "message-box.h"
#include "main-widget-1.h"
#include "main-widget-2.h"
#include "singleton-app-gui.h"

#include <QScreen>
#include <QLayout>
#include <QX11Info>
#include <X11/Xlib.h>
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
    auto layout2 = new QVBoxLayout;
    layout2->setContentsMargins(2, 2, 2, 2);
    mMainLayout->setContentsMargins (0, 0, 0, 0);

    mHeader = new MainHeader;
    mHeader->setTitle ("数据安全检查工具——单机版");
    mMainLayout->addWidget(mHeader);

    mHeader->showMin();
    mHeader->showMax();

    mWindow1 = new MainWidget1;
    mWindow2 = new MainWidget2;

    layout2->addWidget (mWindow1);
    layout2->addWidget (mWindow2);

    mWindow2->hide();

    mMainLayout->addItem (layout2);

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
    else {
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
        if (QX11Info::isPlatformX11 ()) {
            Display *display = QX11Info::display ();
            Atom netMoveResize = XInternAtom (display, "_NET_WM_MOVERESIZE", False);
            XEvent xEvent;
            const auto pos = QCursor::pos ();

            memset (&xEvent, 0, sizeof (XEvent));
            xEvent.xclient.type = ClientMessage;
            xEvent.xclient.message_type = netMoveResize;
            xEvent.xclient.display = display;
            xEvent.xclient.window = this->winId ();
            xEvent.xclient.format = 32;
            xEvent.xclient.data.l[0] = pos.x () * dpiRatio;
            xEvent.xclient.data.l[1] = pos.y () * dpiRatio;
            xEvent.xclient.data.l[2] = 8;
            xEvent.xclient.data.l[3] = Button1;
            xEvent.xclient.data.l[4] = 0;

            XUngrabPointer (display, CurrentTime);
            XSendEvent (display, QX11Info::appRootWindow (QX11Info::appScreen ()), False,
                        SubstructureNotifyMask | SubstructureRedirectMask, &xEvent);
            //XFlush(display);
            XEvent xevent;
            memset (&xevent, 0, sizeof (XEvent));

            xevent.type = ButtonRelease;
            xevent.xbutton.button = Button1;
            xevent.xbutton.window = this->winId ();
            xevent.xbutton.x = e->pos ().x () * dpiRatio;
            xevent.xbutton.y = e->pos ().y () * dpiRatio;
            xevent.xbutton.x_root = pos.x () * dpiRatio;
            xevent.xbutton.y_root = pos.y () * dpiRatio;
            xevent.xbutton.display = display;

            XSendEvent (display, this->effectiveWinId (), False, ButtonReleaseMask, &xevent);
            XFlush (display);

            if (e->source () == Qt::MouseEventSynthesizedByQt) {
                if (!MainWindow::mouseGrabber ()) {
                    this->grabMouse ();
                    this->releaseMouse ();
                }
            }
            mDrag = false;
        }
        else {
            move ((QCursor::pos () - mOffset) * dpiRatio);
        }
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

