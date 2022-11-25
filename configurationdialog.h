//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#include "ui_configurationdialog.h"

class ConfigurationDialog : public QDialog, private Ui::ConfigurationDialog
{
    Q_OBJECT

public:
    explicit ConfigurationDialog(QWidget *parent = 0);

public slots:
    void accept();

private slots:
    void on_toolButton_clicked();
    void on_pushButton_clicked();
    void on_comboBox_editTextChanged(const QString& text);
    void on_fontComboBox_currentFontChanged(const QFont& font);
    void on_comboBox_3_currentIndexChanged(const QString& text);
    void on_checkBox_clicked(bool checked);
    void on_checkBox_2_clicked(bool checked);

private:
    QString appPath;
};

#endif // CONFIGURATIONDIALOG_H
