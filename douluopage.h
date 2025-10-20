#ifndef DOULUOPAGE_H
#define DOULUOPAGE_H

#include <QWidget>

namespace Ui { class DouluoPage; }

class DouluoPage : public QWidget
{
    Q_OBJECT
public:
    explicit DouluoPage(QWidget *parent = nullptr);
    ~DouluoPage();

private slots:
    void onVirtualRealmClicked();

private:
    Ui::DouluoPage *ui;
};

#endif // DOULUOPAGE_H



