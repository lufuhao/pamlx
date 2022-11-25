//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#include "configurationdialog.h"
#include "mainwindow.h"
#include <QCompleter>
#include <QFileDialog>
#include <QFileSystemModel>

ConfigurationDialog::ConfigurationDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

#ifdef Q_OS_MAC
    setWindowTitle(QApplication::translate("ConfigurationDialog", "Preferences", 0, QApplication::UnicodeUTF8));

    QFont font;
    font.setPointSize(13);
    setFont(font);
#endif

#if defined(Q_OS_WIN32)
    label_5->setText("Example:   C:\\Frogram Files\\paml4.9");
#elif defined(Q_OS_MAC)
    label_5->setText("Example:   /Applications/paml4.9");
#else
    label_5->setText("Example:   /usr/local/paml4.9");
#endif

    QCompleter* completer = new QCompleter(this);
    completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
#ifdef Q_OS_WIN32
    completer->setCaseSensitivity(Qt::CaseInsensitive);
#endif
    QFileSystemModel* fileSystemModel = new QFileSystemModel(completer);
    fileSystemModel->setRootPath("");
    QDir::Filters filter = fileSystemModel->filter();
    fileSystemModel->setFilter( filter & ~QDir::Files );
    completer->setModel(fileSystemModel);
    comboBox->setCompleter(completer);

    comboBox->setFont(this->font());
    comboBox->lineEdit()->setDragEnabled(true);

    appPath = QApplication::applicationDirPath();
#ifdef Q_OS_MAC
    QDir dir(appPath);
    dir.cd("../../..");
    appPath = dir.absolutePath();
#endif

    MainWindow* mainWindow = qobject_cast<MainWindow*>(parent);
    if(mainWindow) {

        comboBox->lineEdit()->setText( QDir::toNativeSeparators(mainWindow->pamlPath) );

        fontComboBox->setCurrentFont( mainWindow->outputFont );

        QString text = QString::number( mainWindow->outputFont.pointSize() );
        int index = comboBox_3->findText(text);
        if(index == -1) {
            comboBox_3->addItem(text);
            index = comboBox_3->findText(text);
        }
        comboBox_3->setCurrentIndex(index);

        checkBox->setChecked( mainWindow->outputFont.bold() );

        checkBox_2->setChecked( mainWindow->outputFont.italic() );
    }
}

void ConfigurationDialog::accept()
{
    QDialog::accept();

    MainWindow* mainWindow = qobject_cast<MainWindow*>(this->parent());
    if(mainWindow) {

        QString pamlPath = comboBox->lineEdit()->text();
        if( pamlPath.endsWith('/') || pamlPath.endsWith('\\') )
            pamlPath.remove( pamlPath.size()-1 , 1);
        mainWindow->pamlPath = QDir::fromNativeSeparators(QDir::cleanPath(pamlPath));

        if( QDir::isAbsolutePath(mainWindow->pamlPath) )
            mainWindow->absolutePAMLPath = mainWindow->pamlPath;
        else
            mainWindow->absolutePAMLPath = QDir::cleanPath( appPath + "/" + mainWindow->pamlPath );

        mainWindow->outputFont = fontComboBox->currentFont();
    }
}

void ConfigurationDialog::on_toolButton_clicked()
{
    QString path = QDir::cleanPath( comboBox->lineEdit()->text() );

    if( !QDir::isAbsolutePath(path) )
        path = QDir::cleanPath( appPath + "/" + path );

    QString directoryName = QFileDialog::getExistingDirectory(this, tr("PAML Path"), path);
    if(directoryName.isEmpty()) return;

    comboBox->lineEdit()->setText( QDir::toNativeSeparators(directoryName) );
}

void ConfigurationDialog::on_pushButton_clicked()
{
    MainWindow* mainWindow = qobject_cast<MainWindow*>(this->parent());
    if(mainWindow) {

        fontComboBox->setCurrentFont( mainWindow->defaultOutputFont );

        QString text = QString::number( mainWindow->defaultOutputFont.pointSize() );
        int index = comboBox_3->findText(text);
        if(index == -1) {
            comboBox_3->addItem(text);
            index = comboBox_3->findText(text);
        }
        comboBox_3->setCurrentIndex(index);

        checkBox->setChecked( mainWindow->defaultOutputFont.bold() );

        checkBox_2->setChecked( mainWindow->defaultOutputFont.italic() );
    }
}

void ConfigurationDialog::on_comboBox_editTextChanged(const QString& text)
{
    QString path = QDir::cleanPath(text);

    if( !QDir::isAbsolutePath(path) )
        path = QDir::cleanPath( appPath + "/" + path );

    if( !QDir(path).exists() )
        comboBox->setStyleSheet("color: red");
    else
        comboBox->setStyleSheet("");
}

void ConfigurationDialog::on_fontComboBox_currentFontChanged(const QFont& font)
{
    textEdit->setFont(font);
    textEdit->setText("<p align=\"center\"><br/><br/>Sample text</p>");
}

void ConfigurationDialog::on_comboBox_3_currentIndexChanged(const QString& text)
{
    QFont font = fontComboBox->currentFont();
    int pointSize = text.toInt();
    if( font.pointSize() != pointSize ) {
        font.setPointSize(pointSize);
        fontComboBox->setCurrentFont(font);
    }
}

void ConfigurationDialog::on_checkBox_clicked(bool checked)
{
    QFont font = fontComboBox->currentFont();
    font.setBold(checked);
    fontComboBox->setCurrentFont(font);
}

void ConfigurationDialog::on_checkBox_2_clicked(bool checked)
{
    QFont font = fontComboBox->currentFont();
    font.setItalic(checked);
    fontComboBox->setCurrentFont(font);
}
