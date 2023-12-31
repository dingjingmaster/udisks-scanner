//
// Created by dingjing on 2/7/23.
//

#ifndef ESAFE_DSM_MAIN_WINDOW_H
#define ESAFE_DSM_MAIN_WINDOW_H
#include <QWidget>

class MainHeader;
class MainWidget11;
class MainWidget12;
class MainWidget21;
class MainWindow : public QWidget
{
    Q_OBJECT
    enum Direction {UP = 0, DOWN, LEFT, RIGHT, LEFT_TOP, LEFT_BOTTOM, RIGHT_BOTTOM, RIGHT_TOP, NONE};
public:
    explicit MainWindow (QWidget* parent = nullptr);

Q_SIGNALS:
    void resizeWidgets ();

protected:
    void resizeEvent (QResizeEvent*) override;
    void mouseMoveEvent (QMouseEvent*) override;
    void mousePressEvent (QMouseEvent*) override;
    void mouseReleaseEvent (QMouseEvent*) override;

private:
    void region (const QPoint& cursorGlobalPoint);

private:
    constexpr static int            mWidth = 1200;
    constexpr static int            mHeight = 700;
    QLayout*                        mMainLayout = nullptr;

    MainHeader*                     mHeader = nullptr;
    MainWidget11*                   mWindow11 = nullptr;
    MainWidget12*                   mWindow12 = nullptr;
    MainWidget21*                   mWindow21 = nullptr;

    QPoint                          mOffset;
    Direction                       mDirection;
    bool                            mDrag = false;
    bool                            mIsPress = false;
    QPoint                          mDragPos;
    QStringList                     mOperationFiles;
};



#endif //ESAFE_DSM_MAIN_WINDOW_H
