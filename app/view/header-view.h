#ifndef HEADERVIEW_H
#define HEADERVIEW_H

#include <QHeaderView>
#include <QObject>

class HeaderView : public QHeaderView
{
    Q_OBJECT
public:
    HeaderView(Qt::Orientation orientation, QWidget* parent=nullptr);

    bool isChecked () const;
    void setChecked (bool);

Q_SIGNALS:
    void checkBoxClicked (bool state);

public:
    void paintSection (QPainter* p, const QRect& rect, int logicalIndex) const override;

protected:
    void mousePressEvent (QMouseEvent* e) override;

private:
    void redrawCheckBox();

private:
    bool                mChecked = false;
};

#endif // HEADERVIEW_H
