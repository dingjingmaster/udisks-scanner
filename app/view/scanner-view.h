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
    void taskStart(const QString& taskID);
    void taskPause(const QString& taskID);
    void taskStop(const QString& taskID);
    void taskDelete(const QString& taskID);
    void taskResume(const QString& taskID);
    void taskDetail(const QString& taskID);
    void updateView ();

private:
    float            mCurPos{};
};

#endif // SCANNERVIEW_H
