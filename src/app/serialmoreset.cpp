#include "serialmoreset.h"
#include "ui_serialmoreset.h"
SerialMoreSet *SerialMoreSet::dialog = nullptr;
SerialMoreSet::SerialMoreSet(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SerialMoreSet)
{
    ui->setupUi(this);
}

SerialMoreSet *SerialMoreSet::getDialog()
{
    if(dialog == nullptr)
        dialog = new SerialMoreSet;
    return dialog;
}

SerialMoreSet::~SerialMoreSet()
{
    delete ui;
}

void SerialMoreSet::getConfig(int &data, int &stop, int &parity, int &flow)
{
    int index = ui->cbData->currentIndex();
    switch (index) {
    case 0:
        data = 5;
        break;
    case 1:
        data = 6;
        break;
    case 2:
        data = 7;
        break;
    case 3:
        data = 8;
        break;
    default:
        data = 8;
        break;
    }
    index = ui->cbStop->currentIndex();
    switch (index) {
    case 0:
        stop = 1;
        break;
    case 1:
        stop = 3;
        break;
    case 2:
        stop = 2;
        break;
    default:
        stop = 1;
        break;
    }
    index = ui->cbParity->currentIndex();
    switch (index) {
    case 0:
        parity = 0;
        break;
    case 1:
        parity = 2;
        break;
    case 2:
        parity = 3;
        break;
    case 3:
        parity = 4;
        break;
    case 4:
        parity = 5;
        break;
    default:
        parity = 0;
        break;
    }
    index = ui->cbFlow->currentIndex();
    switch (index) {
    case 0:
        flow = 0;
        break;
    case 1:
        flow = 1;
        break;
    case 2:
        flow = 2;
        break;
    default:
        flow = 0;
        break;
    }
}
