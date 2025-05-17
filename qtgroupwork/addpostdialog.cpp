#include "addpostdialog.h"
#include "ui_addpostdialog.h"

AddPostDialog::AddPostDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddPostDialog)
{
    ui->setupUi(this);
}

AddPostDialog::~AddPostDialog()
{
    delete ui;
}


