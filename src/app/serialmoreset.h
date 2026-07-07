#ifndef SERIALMORESET_H
#define SERIALMORESET_H

#include <QDialog>

namespace Ui {
class SerialMoreSet;
}

class SerialMoreSet : public QDialog
{
    Q_OBJECT

protected:
    explicit SerialMoreSet(QWidget *parent = nullptr);

public:
    static SerialMoreSet *getDialog();
    ~SerialMoreSet();
    void getConfig(int &data, int &stop, int &parity, int &flow);;

private:
    static SerialMoreSet *dialog;
    Ui::SerialMoreSet *ui;
};

#endif // SERIALMORESET_H
