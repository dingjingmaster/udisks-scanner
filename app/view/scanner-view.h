#ifndef SCANNERVIEW_H
#define SCANNERVIEW_H

#include <QTableView>
#include <QMouseEvent>

class ScannerView : public QTableView
{
    Q_OBJECT
public:
    explicit ScannerView(QWidget *parent = nullptr);

protected:
    void paintEvent (QPaintEvent* ev) override;
    void mouseMoveEvent (QMouseEvent* event) override;
    void mouseReleaseEvent (QMouseEvent* event) override;

Q_SIGNALS:
    void taskStart();
    void taskPause();
    void taskStop();
    void taskDelete();
    void taskDetail(const QString& taskID);
    void updateView ();

private:
    float            mCurPos{};
};

#endif // SCANNERVIEW_H
