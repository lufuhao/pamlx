//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#include "mainwindow.h"
#include "mcmctreeengine.h"
#include "basemlengine.h"
#include "codemlengine.h"
#include "pampengine.h"
#include "yn00engine.h"
#include "evolverengine.h"
#include "chi2dialog.h"
#include "codonfrequencydialog.h"
#include "aminoacidfrequencydialog.h"
#include "configurationdialog.h"
#include "pamlhistorydialog.h"
#include "opendialog.h"
#include "outputwindow.h"
#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFileSystemModel>
#include <QCloseEvent>
#include <QClipboard>
#include <QtWidgets/QMessageBox>
#include <QtPrintSupport/QPrinter>
#include <QtWidgets/QFontDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QtPrintSupport/QPrintDialog>
#include <QUrl>
#include <typeinfo>
#include <QtWidgets/QButtonGroup>
#include <QMimeData>

static const char* strList[] = {"", "MCMCTree", "BaseML", "CodeML", "PAMP", "YN00", "Evolver"};
static const double precision = 1e-12;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
#if defined(Q_OS_MAC)
    defaultOutputFont("Courier New", 12),
    defaultPrintFont("Courier New", 14),
#elif defined(Q_OS_LINUX)
    defaultOutputFont("Liberation Mono", 10),
    defaultPrintFont("Sans Serif", 12),
#else
    defaultOutputFont("Courier New", 8),
    defaultPrintFont("Courier New", 9),
#endif
    mcmcTreeEngine(NULL),
    basemlEngine(NULL),
    codemlEngine(NULL),
    pampEngine(NULL),
    yn00Engine(NULL),
    evolverEngine(NULL),
    mcmcTreeOutput(NULL),
    basemlOutput(NULL),
    codemlOutput(NULL),
    pampOutput(NULL),
    yn00Output(NULL),
    evolverOutput(NULL),
    currentDocument(NULL),
    currentDocumentType(Invalid),
    chi2Dialog(NULL),
    codonFrequencyDialog(NULL),
    aminoAcidFrequencyDialog(NULL),
    focusEdit(NULL),
    actionGroup(NULL),
    buttonGroup(NULL)
{
    setupUi(this);

    setAcceptDrops(true);
    setWindowTitle(QApplication::applicationName());

    //Menu
    QAction* actionBefore = menuRecent_Files->actions().first();
    for(int i=MaxRecentFiles-1; i>=0; i--) {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setData(i);
        recentFileActions[i]->setVisible(false);
        recentFileActions[i]->setStatusTip(tr("Open this file"));
        if(i < MaxRecentFiles-1)
            actionBefore = recentFileActions[i+1];
        menuRecent_Files->insertAction(actionBefore, recentFileActions[i]);
        QObject::connect(recentFileActions[i], SIGNAL(triggered()), this, SLOT(onActionOpenRecentFile()));
    }

    for(unsigned int i=0; i<MaxOutputWindows && i+1<sizeof(strList)/sizeof(char*); i++) {
        outputWindowActions[i] = new QAction(this);
        outputWindowActions[i]->setText(tr("&%1 %2 Output").arg(i+1).arg(strList[i+1]));
        outputWindowActions[i]->setStatusTip(tr("Show or hide the output window"));
        outputWindowActions[i]->setCheckable(true);
        outputWindowActions[i]->setData(i);
        menuWindow->addAction(outputWindowActions[i]);
        QObject::connect(outputWindowActions[i], SIGNAL(triggered()), this, SLOT(onActionOutputWindow()));
    }

    actionOpen->setShortcut(QKeySequence::Open);
    actionClose->setShortcut(QKeySequence::Close);
    actionSave->setShortcut(QKeySequence::Save);
    actionSave_As->setShortcut(QKeySequence::SaveAs);
    actionPrint->setShortcut(QKeySequence::Print);

    actionUndo->setShortcut(QKeySequence::Undo);
    actionRedo->setShortcut(QKeySequence::Redo);
    actionCut->setShortcut(QKeySequence::Cut);
    actionCopy->setShortcut(QKeySequence::Copy);
    actionPaste->setShortcut(QKeySequence::Paste);
    actionDelete->setShortcut(QKeySequence::Delete);
    actionSelect_All->setShortcut(QKeySequence::SelectAll);

    actionText_View->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));

    actionPAML_User_Guide->setShortcut(QKeySequence::HelpContents);

    installEventFilter(this);

#ifdef Q_OS_MAC
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/empty.png"), QSize(), QIcon::Normal, QIcon::Off);
    actionClose->setIcon(icon);
    actionSave_As->setIcon(icon);
    actionSet_Printer_Font->setIcon(icon);
    actionPrint_Preview->setIcon(icon);
    menuRecent_Files->setIcon(icon);
    actionSelect_All->setIcon(icon);

    actionConfiguration->setText(QApplication::translate("MainWindow", "&Preferences...", 0, QApplication::UnicodeUTF8));
#endif

    //Toolbar
    mainToolBar->setWindowTitle(tr("Toolbar"));
    mainToolBar->addAction(actionStart_Page);
    mainToolBar->addSeparator();
    mainToolBar->addAction(actionMCMCTree);
    mainToolBar->addAction(actionBaseml);
    mainToolBar->addAction(actionCodeml);
    mainToolBar->addAction(actionPAMP);
    mainToolBar->addAction(actionYN00);
    mainToolBar->setAllowedAreas(Qt::AllToolBarAreas);
    mainToolBar->setFloatable(false);

#ifdef Q_OS_MAC
    QFont font1;
    font1.setFamily(QString::fromUtf8("Georgia"));
    font1.setPointSize(14);
    font1.setBold(true);
    font1.setWeight(75);
    mainToolBar->setFont(font1);
#endif

    actionGroup = new QActionGroup(this);
    actionGroup->addAction(actionStart_Page);
    actionGroup->addAction(actionMCMCTree);
    actionGroup->addAction(actionBaseml);
    actionGroup->addAction(actionCodeml);
    actionGroup->addAction(actionPAMP);
    actionGroup->addAction(actionYN00);
    actionGroup->addAction(actionEvolver);

    buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(radioButton);
    buttonGroup->addButton(radioButton_2);

    //Stacked widget
#ifdef Q_OS_MAC
    QFont font;
    font.setPointSize(13);
    stackedWidget->setFont(font);
#endif
    QFont mainWidgetFont = stackedWidget->font();

    //Start page
    actionStart_Page->trigger();

    commandLinkButton->installEventFilter(this);
    commandLinkButton_2->installEventFilter(this);
    commandLinkButton_3->installEventFilter(this);
    commandLinkButton_4->installEventFilter(this);
    commandLinkButton_5->installEventFilter(this);

#ifdef Q_OS_MAC
    label_72->setText(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Lucida Grande'; font-size:13pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'SimSun'; font-size:9pt;\"></p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'SimSun'; font-size:9pt;\"></p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'SimSun'; font-size:9pt;\"></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Chalkduster'; font-size:28pt; font-weight:600;\">   </span><img src=\":/pamlxlogo.png\" style=\"vertical-align: middle;\" /><span style=\" font-family:'Chalkduster'; font-size:28pt; font-weight:600;\">   Phylogenetic Analysis by Maximum Likelihood          </span></p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'SimSun'; font-size:10pt; font-weight:600;\"></p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'SimSun'; font-size:10pt; font-weight:600;\"></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"http://abacus.gene.ucl.ac.uk/\"><span style=\" font-family:'Nanum Pen Script'; font-size:36pt; font-weight:600; text-decoration: none; color:#000000;\">Ziheng Yang</span></a></p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'SimSun'; font-size:9pt;\"></p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'SimSun'; font-size:9pt;\"></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"http://abacus.gene.ucl.ac.uk/software/paml.html\"><span style=\" font-family:'Herculanum'; font-size:20pt; text-decoration: none; color:#000000;\">PAML Software Website</span></a></p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'SimSun'; font-size:20pt; font-weight:600;\"></p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'SimSun'; font-size:9pt;\"></p></body></html>", 0, QApplication::UnicodeUTF8));

    QFont font2;
    font2.setFamily(QString::fromUtf8("Comic Sans MS"));
    font2.setPointSize(14);
    font2.setBold(true);
    font2.setItalic(false);
    font2.setWeight(75);
    commandLinkButton->setFont(font2);
    commandLinkButton_2->setFont(font2);
    commandLinkButton_3->setFont(font2);
    commandLinkButton_4->setFont(font2);
    commandLinkButton_5->setFont(font2);

    QFont font3;
    font3.setFamily(QString::fromUtf8("Comic Sans MS"));
    font3.setPointSize(14);
    font3.setBold(false);
    font3.setItalic(false);
    font3.setWeight(50);
    commandLinkButton_6->setFont(font3);
    commandLinkButton_7->setFont(font3);

    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/arrow.png"), QSize(), QIcon::Normal, QIcon::Off);
    commandLinkButton->setIcon(icon1);
    commandLinkButton_2->setIcon(icon1);
    commandLinkButton_3->setIcon(icon1);
    commandLinkButton_4->setIcon(icon1);
    commandLinkButton_5->setIcon(icon1);
    commandLinkButton_6->setIcon(icon1);
    commandLinkButton_7->setIcon(icon1);
#endif


    //mcmctree
    QCompleter* completer_20 = new QCompleter(this);
    completer_20->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
#ifdef Q_OS_WIN32
    completer_20->setCaseSensitivity(Qt::CaseInsensitive);
#endif
    QFileSystemModel* fileSystemModel_20 = new QFileSystemModel(completer_20);
    fileSystemModel_20->setRootPath("");
    completer_20->setModel(fileSystemModel_20);
    comboBox_20->setCompleter(completer_20);

    QCompleter* completer_21 = new QCompleter(this);
    completer_21->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
#ifdef Q_OS_WIN32
    completer_21->setCaseSensitivity(Qt::CaseInsensitive);
#endif
    QFileSystemModel* fileSystemModel_21 = new QFileSystemModel(completer_21);
    fileSystemModel_21->setRootPath("");
    completer_21->setModel(fileSystemModel_21);
    comboBox_21->setCompleter(completer_21);

    QCompleter* completer_22 = new QCompleter(this);
    completer_22->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
#ifdef Q_OS_WIN32
    completer_22->setCaseSensitivity(Qt::CaseInsensitive);
#endif
    QFileSystemModel* fileSystemModel_22 = new QFileSystemModel(completer_22);
    fileSystemModel_22->setRootPath("");
    QDir::Filters filter_22 = fileSystemModel_22->filter();
    fileSystemModel_22->setFilter( filter_22 & ~QDir::Files );
    completer_22->setModel(fileSystemModel_22);
    comboBox_22->setCompleter(completer_22);

    comboBox_20->setFont(mainWidgetFont);
    comboBox_21->setFont(mainWidgetFont);
    comboBox_22->setFont(mainWidgetFont);

    comboBox_20->lineEdit()->setDragEnabled(true);
    comboBox_21->lineEdit()->setDragEnabled(true);
    comboBox_22->lineEdit()->setDragEnabled(true);

#if defined(Q_OS_WIN32)
    label->setText(label->text() + "            ");
    label_24->setText(label_24->text() + "  ");
    gridLayout_63->setContentsMargins(-1, 13, -1, 7);
#elif defined(Q_OS_MAC)
    label->setText(label->text() + "         ");
    label_24->setText(label_24->text() + " ");
    gridLayout_63->setContentsMargins(-1, 30, -1, 15);
#endif


    //baseml
    QCompleter* completer_17 = new QCompleter(this);
    completer_17->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
#ifdef Q_OS_WIN32
    completer_17->setCaseSensitivity(Qt::CaseInsensitive);
#endif
    QFileSystemModel* fileSystemModel_17 = new QFileSystemModel(completer_17);
    fileSystemModel_17->setRootPath("");
    completer_17->setModel(fileSystemModel_17);
    comboBox_17->setCompleter(completer_17);

    QCompleter* completer_18 = new QCompleter(this);
    completer_18->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
#ifdef Q_OS_WIN32
    completer_18->setCaseSensitivity(Qt::CaseInsensitive);
#endif
    QFileSystemModel* fileSystemModel_18 = new QFileSystemModel(completer_18);
    fileSystemModel_18->setRootPath("");
    completer_18->setModel(fileSystemModel_18);
    comboBox_18->setCompleter(completer_18);

    QCompleter* completer_19 = new QCompleter(this);
    completer_19->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
#ifdef Q_OS_WIN32
    completer_19->setCaseSensitivity(Qt::CaseInsensitive);
#endif
    QFileSystemModel* fileSystemModel_19 = new QFileSystemModel(completer_19);
    fileSystemModel_19->setRootPath("");
    QDir::Filters filter_19 = fileSystemModel_19->filter();
    fileSystemModel_19->setFilter( filter_19 & ~QDir::Files );
    completer_19->setModel(fileSystemModel_19);
    comboBox_19->setCompleter(completer_19);

    comboBox_17->setFont(mainWidgetFont);
    comboBox_18->setFont(mainWidgetFont);
    comboBox_19->setFont(mainWidgetFont);

    comboBox_17->lineEdit()->setDragEnabled(true);
    comboBox_18->lineEdit()->setDragEnabled(true);
    comboBox_19->lineEdit()->setDragEnabled(true);

#if defined(Q_OS_WIN32)
    label_34->setText(label_34->text() + "            ");
    gridLayout_65->setContentsMargins(-1, 9, -1, -1);
#elif defined(Q_OS_MAC)
    label_34->setText(label_34->text() + "         ");
    gridLayout_65->setContentsMargins(-1, 18, -1, -1);
#endif


    //codeml
    QCompleter* completer_23 = new QCompleter(this);
    completer_23->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
#ifdef Q_OS_WIN32
    completer_23->setCaseSensitivity(Qt::CaseInsensitive);
#endif
    QFileSystemModel* fileSystemModel_23 = new QFileSystemModel(completer_23);
    fileSystemModel_23->setRootPath("");
    completer_23->setModel(fileSystemModel_23);
    comboBox_23->setCompleter(completer_23);

    QCompleter* completer_24 = new QCompleter(this);
    completer_24->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
#ifdef Q_OS_WIN32
    completer_24->setCaseSensitivity(Qt::CaseInsensitive);
#endif
    QFileSystemModel* fileSystemModel_24 = new QFileSystemModel(completer_24);
    fileSystemModel_24->setRootPath("");
    completer_24->setModel(fileSystemModel_24);
    comboBox_24->setCompleter(completer_24);

    QCompleter* completer_25 = new QCompleter(this);
    completer_25->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
#ifdef Q_OS_WIN32
    completer_25->setCaseSensitivity(Qt::CaseInsensitive);
#endif
    QFileSystemModel* fileSystemModel_25 = new QFileSystemModel(completer_25);
    fileSystemModel_25->setRootPath("");
    QDir::Filters filter_25 = fileSystemModel_25->filter();
    fileSystemModel_25->setFilter( filter_25 & ~QDir::Files );
    completer_25->setModel(fileSystemModel_25);
    comboBox_25->setCompleter(completer_25);

    QCompleter* completer_45 = new QCompleter(this);
    completer_45->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
#ifdef Q_OS_WIN32
    completer_45->setCaseSensitivity(Qt::CaseInsensitive);
#endif
    QFileSystemModel* fileSystemModel_45 = new QFileSystemModel(completer_45);
    fileSystemModel_45->setRootPath("");
    completer_45->setModel(fileSystemModel_45);
    comboBox_45->setCompleter(completer_45);

    comboBox_23->setFont(mainWidgetFont);
    comboBox_24->setFont(mainWidgetFont);
    comboBox_25->setFont(mainWidgetFont);
    comboBox_45->setFont(mainWidgetFont);

    comboBox_23->lineEdit()->setDragEnabled(true);
    comboBox_24->lineEdit()->setDragEnabled(true);
    comboBox_25->lineEdit()->setDragEnabled(true);
    comboBox_45->lineEdit()->setDragEnabled(true);

#if defined(Q_OS_WIN32)
    label_39->setText(label_39->text() + "            ");
    gridLayout_62->setContentsMargins(-1, 9, -1, -1);
    checkBox_22->setText(checkBox_22->text() + "      ");
    checkBox_23->setText(checkBox_23->text() + "      ");
    checkBox_25->setText(checkBox_25->text() + "      ");
#elif defined(Q_OS_MAC)
    label_39->setText(label_39->text() + "         ");
    gridLayout_62->setContentsMargins(-1, 18, -1, -1);
    checkBox_22->setText(checkBox_22->text() + "           ");
    checkBox_23->setText(checkBox_23->text() + "           ");
    checkBox_25->setText(checkBox_25->text() + "           ");
#elif defined(Q_OS_LINUX)
    checkBox_22->setText(checkBox_22->text() + "   ");
    checkBox_23->setText(checkBox_23->text() + "   ");
    checkBox_25->setText(checkBox_25->text() + "   ");
#endif


    //pamp
    QCompleter* completer_27 = new QCompleter(this);
    completer_27->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
#ifdef Q_OS_WIN32
    completer_27->setCaseSensitivity(Qt::CaseInsensitive);
#endif
    QFileSystemModel* fileSystemModel_27 = new QFileSystemModel(completer_27);
    fileSystemModel_27->setRootPath("");
    completer_27->setModel(fileSystemModel_27);
    comboBox_27->setCompleter(completer_27);

    QCompleter* completer_28 = new QCompleter(this);
    completer_28->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
#ifdef Q_OS_WIN32
    completer_28->setCaseSensitivity(Qt::CaseInsensitive);
#endif
    QFileSystemModel* fileSystemModel_28 = new QFileSystemModel(completer_28);
    fileSystemModel_28->setRootPath("");
    completer_28->setModel(fileSystemModel_28);
    comboBox_28->setCompleter(completer_28);

    QCompleter* completer_29 = new QCompleter(this);
    completer_29->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
#ifdef Q_OS_WIN32
    completer_29->setCaseSensitivity(Qt::CaseInsensitive);
#endif
    QFileSystemModel* fileSystemModel_29 = new QFileSystemModel(completer_29);
    fileSystemModel_29->setRootPath("");
    QDir::Filters filter_29 = fileSystemModel_29->filter();
    fileSystemModel_29->setFilter( filter_29 & ~QDir::Files );
    completer_29->setModel(fileSystemModel_29);
    comboBox_29->setCompleter(completer_29);

    comboBox_27->setFont(mainWidgetFont);
    comboBox_28->setFont(mainWidgetFont);
    comboBox_29->setFont(mainWidgetFont);

    comboBox_27->lineEdit()->setDragEnabled(true);
    comboBox_28->lineEdit()->setDragEnabled(true);
    comboBox_29->lineEdit()->setDragEnabled(true);

#if defined(Q_OS_WIN32)
    label_53->setText(label_53->text() + "            ");
#elif defined(Q_OS_MAC)
    label_53->setText(label_53->text() + "         ");
#endif


    //yn00
    QCompleter* completer_33 = new QCompleter(this);
    completer_33->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
#ifdef Q_OS_WIN32
    completer_33->setCaseSensitivity(Qt::CaseInsensitive);
#endif
    QFileSystemModel* fileSystemModel_33 = new QFileSystemModel(completer_33);
    fileSystemModel_33->setRootPath("");
    completer_33->setModel(fileSystemModel_33);
    comboBox_33->setCompleter(completer_33);

    QCompleter* completer_35 = new QCompleter(this);
    completer_35->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
#ifdef Q_OS_WIN32
    completer_35->setCaseSensitivity(Qt::CaseInsensitive);
#endif
    QFileSystemModel* fileSystemModel_35 = new QFileSystemModel(completer_35);
    fileSystemModel_35->setRootPath("");
    QDir::Filters filter_35 = fileSystemModel_35->filter();
    fileSystemModel_35->setFilter( filter_35 & ~QDir::Files );
    completer_35->setModel(fileSystemModel_35);
    comboBox_35->setCompleter(completer_35);

    comboBox_33->setFont(mainWidgetFont);
    comboBox_35->setFont(mainWidgetFont);

    comboBox_33->lineEdit()->setDragEnabled(true);
    comboBox_35->lineEdit()->setDragEnabled(true);

#if defined(Q_OS_WIN32)
    label_59->setText(label_59->text() + "            ");
#elif defined(Q_OS_MAC)
    label_59->setText(label_59->text() + "         ");
#endif


    //evolver
    QCompleter* completer_10 = new QCompleter(this);
    completer_10->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
#ifdef Q_OS_WIN32
    completer_10->setCaseSensitivity(Qt::CaseInsensitive);
#endif
    QFileSystemModel* fileSystemModel_10 = new QFileSystemModel(completer_10);
    fileSystemModel_10->setRootPath("");
    QDir::Filters filter_10 = fileSystemModel_10->filter();
    fileSystemModel_10->setFilter( filter_10 & ~QDir::Files );
    completer_10->setModel(fileSystemModel_10);
    comboBox_10->setCompleter(completer_10);

    QCompleter* completer_11 = new QCompleter(this);
    completer_11->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
#ifdef Q_OS_WIN32
    completer_11->setCaseSensitivity(Qt::CaseInsensitive);
#endif
    QFileSystemModel* fileSystemModel_11 = new QFileSystemModel(completer_11);
    fileSystemModel_11->setRootPath("");
    completer_11->setModel(fileSystemModel_11);
    comboBox_11->setCompleter(completer_11);

    comboBox_10->setFont(mainWidgetFont);
    comboBox_11->setFont(mainWidgetFont);

    comboBox_10->lineEdit()->setDragEnabled(true);
    comboBox_11->lineEdit()->setDragEnabled(true);


    loadSettings();

    updateEnginesPath();
    updateOutputFont();
    updateRecentFileActions();
}

MainWindow::~MainWindow()
{
    focusEdit = NULL;

    delete chi2Dialog;
    chi2Dialog = NULL;

    delete codonFrequencyDialog;
    codonFrequencyDialog = NULL;

    delete aminoAcidFrequencyDialog;
    aminoAcidFrequencyDialog = NULL;

    delete mcmcTreeOutput;
    mcmcTreeOutput = NULL;

    delete basemlOutput;
    basemlOutput = NULL;

    delete codemlOutput;
    codemlOutput = NULL;

    delete pampOutput;
    pampOutput = NULL;

    delete yn00Output;
    yn00Output = NULL;

    delete evolverOutput;
    evolverOutput = NULL;

    delete mcmcTreeEngine;
    mcmcTreeEngine = NULL;

    delete basemlEngine;
    basemlEngine = NULL;

    delete codemlEngine;
    codemlEngine = NULL;

    delete pampEngine;
    pampEngine = NULL;

    delete yn00Engine;
    yn00Engine = NULL;

    delete evolverEngine;
    evolverEngine = NULL;

    currentDocument = NULL;
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if( event->type() == QEvent::ShortcutOverride ) {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        if(key) {
            if( key->matches(QKeySequence::Open) )
                actionOpen->setEnabled( !actionText_View->isChecked() );
            else if( key->matches(QKeySequence::Close) )
                actionClose->setEnabled( currentDocument && !currentDocument->title().isEmpty() && !actionText_View->isChecked() );
            else if( key->matches(QKeySequence::Save) )
                actionSave->setEnabled( currentDocument && currentDocument->isModified() );
            else if( key->matches(QKeySequence::SaveAs) )
                actionSave_As->setEnabled( currentDocument );
            else if( key->matches(QKeySequence::Print) )
                actionPrint->setEnabled( currentDocument && !currentDocument->title().isEmpty() );
            else if( key->modifiers() == Qt::ControlModifier && key->key() == Qt::Key_T )
                actionText_View->setEnabled( currentDocument && !currentDocument->title().isEmpty() );
        }
    }
    else if( event->type() == QEvent::DragEnter && (watched == commandLinkButton || watched == commandLinkButton_2 || watched == commandLinkButton_3 || watched == commandLinkButton_4 || watched == commandLinkButton_5) ) {
        QDragEnterEvent* aDragEnterEvent = static_cast<QDragEnterEvent*>(event);
        if( aDragEnterEvent && aDragEnterEvent->mimeData()->hasFormat("text/uri-list") ) {
            aDragEnterEvent->acceptProposedAction();
            return true;
        }
    }
    else if( event->type() == QEvent::Drop && (watched == commandLinkButton || watched == commandLinkButton_2 || watched == commandLinkButton_3 || watched == commandLinkButton_4 || watched == commandLinkButton_5) ) {
        QCommandLinkButton* button = qobject_cast<QCommandLinkButton*>(watched);
        QDropEvent* aDropEvent = static_cast<QDropEvent*>(event);
        if(button && aDropEvent) {
            button->click();
            dropEvent(aDropEvent);
        }
        return true;
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    ControlFileType oldDocumentType = currentDocumentType;
    if(currentDocument) {
        if( !saveModifiedFile(currentDocument) ) {
            event->ignore();
            return;
        }
    }

    if(mcmcTreeEngine && oldDocumentType != MCMCTreeCtl) {
        if( !saveModifiedFile(mcmcTreeEngine) ) {
            event->ignore();
            return;
        }
    }

    if(basemlEngine && oldDocumentType != BaseMLCtl) {
        if( !saveModifiedFile(basemlEngine) ) {
            event->ignore();
            return;
        }
    }

    if(codemlEngine && oldDocumentType != CodeMLCtl) {
        if( !saveModifiedFile(codemlEngine) ) {
            event->ignore();
            return;
        }
    }

    if(pampEngine && oldDocumentType != PAMPCtl) {
        if( !saveModifiedFile(pampEngine) ) {
            event->ignore();
            return;
        }
    }

    if(yn00Engine && currentDocumentType != YN00Ctl) {
        if( !saveModifiedFile(yn00Engine) ) {
            event->ignore();
            return;
        }
    }

    if(evolverEngine && !(oldDocumentType >= MCbaseDat && oldDocumentType <= MCaaDat)) {
        if( !saveModifiedFile(evolverEngine) ) {
            event->ignore();
            return;
        }
    }

    delete mcmcTreeOutput;
    mcmcTreeOutput = NULL;

    delete basemlOutput;
    basemlOutput = NULL;

    delete codemlOutput;
    codemlOutput = NULL;

    delete pampOutput;
    pampOutput = NULL;

    delete yn00Output;
    yn00Output = NULL;

    delete evolverOutput;
    evolverOutput = NULL;

    delete chi2Dialog;
    chi2Dialog = NULL;

    saveSettings();
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if( currentDocument && event->mimeData()->hasFormat("text/uri-list") )
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent* event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty()) return;

    QString fileName = urls.first().toLocalFile();
    if(fileName.isEmpty()) return;

    loadDocumentFile(fileName, currentDocumentType);
    event->acceptProposedAction();
}

void MainWindow::loadSettings()
{
    QString settingsFileName = QApplication::applicationDirPath();
    settingsFileName += "/pamlx.conf";
    QFile settingsFile(settingsFileName);

    if( settingsFile.open(QIODevice::ReadOnly) ) {
        QDataStream settingsIn(&settingsFile);
        settingsIn.setVersion(QDataStream::Qt_4_0);

        const quint32 magicNumber = 0x30A6FF4E;
        quint32 storedMagicNumber;
        settingsIn >> storedMagicNumber;

        if(storedMagicNumber == magicNumber) {
            QByteArray geometry;
            qint32 toolBarArea;
            bool toolbarVisible;
            bool statusBarVisible;

            settingsIn >> pamlPath
                       >> outputFont
                       >> printFont
                       >> recentFiles
                       >> lastFileOpenPath
                       >> geometry
                       >> toolBarArea
                       >> toolbarVisible
                       >> statusBarVisible;

            restoreGeometry(geometry);
            addToolBar(Qt::ToolBarArea(toolBarArea), mainToolBar);
            mainToolBar->setVisible(toolbarVisible);
            QMainWindow::statusBar()->setVisible(statusBarVisible);

            if( QDir::isAbsolutePath(pamlPath) ) {
                absolutePAMLPath = pamlPath;
            }
            else {
                QString appPath = QApplication::applicationDirPath();
#ifdef Q_OS_MAC
                QDir dir(appPath);
                dir.cd("../../..");
                appPath = dir.absolutePath();
#endif
                absolutePAMLPath = QDir::cleanPath( appPath + "/" + pamlPath );
            }

            return;
        }
    }

//DefaultSettings
    pamlPath = QApplication::applicationDirPath();
#ifdef Q_OS_MAC
    QDir dir(pamlPath);
    dir.cd("../../..");
    pamlPath = dir.absolutePath();
#endif
    outputFont = defaultOutputFont;
    printFont = defaultPrintFont;
    lastFileOpenPath = "";
    absolutePAMLPath = pamlPath;
}

void MainWindow::saveSettings()
{
    QString settingsFileName = QApplication::applicationDirPath();
    settingsFileName += "/pamlx.conf";
    QFile settingsFile(settingsFileName);

    if( settingsFile.open(QIODevice::WriteOnly) ) {
        QDataStream settingsOut(&settingsFile);
        settingsOut.setVersion(QDataStream::Qt_4_0);

        const quint32 magicNumber = 0x30A6FF4E;
        settingsOut << magicNumber
                    << pamlPath
                    << outputFont
                    << printFont
                    << recentFiles
                    << lastFileOpenPath
                    << saveGeometry()
                    << qint32(toolBarArea(mainToolBar))
                    << mainToolBar->isVisible()
                    << QMainWindow::statusBar()->isVisible();
    }
}

void MainWindow::loadDocumentFile(const QString& fileName, ControlFileType type)
{
    switch(type)
    {
    case MCMCTreeCtl:
        actionMCMCTree->trigger();
        break;
    case BaseMLCtl:
        actionBaseml->trigger();
        break;
    case CodeMLCtl:
        actionCodeml->trigger();
        break;
    case PAMPCtl:
        actionPAMP->trigger();
        break;
    case YN00Ctl:
        actionYN00->trigger();
        break;
    case MCbaseDat:
    case MCcodonDat:
    case MCaaDat:
        actionEvolver->trigger();
        break;
    default:
        return;
    }

    //if( !fileName.isEmpty() && fileName == currentDocument->fileName() ) return;

    if( !saveModifiedFile(currentDocument) ) return;

    ControlFileType oldDocumentType = currentDocumentType;
    if(type >= MCbaseDat)
        tabWidget->setCurrentIndex( int(type - MCbaseDat) );

    if(fileName.isEmpty()) {
        //NemDocument
        if( currentDocument->onNewDocument() ) {
            updateUi();
            updateUiTitle();
        }
        else {
            if(oldDocumentType != currentDocumentType)
                tabWidget->setCurrentIndex( int(oldDocumentType - MCbaseDat) );
        }
    }
    else {
        //OpenDocument
        if( currentDocument->onOpenDocument(fileName) ) {
            updateUi();
            updateUiTitle();
            addToRecentFiles(fileName, type);
        }
        else {
            if(oldDocumentType != currentDocumentType)
                tabWidget->setCurrentIndex( int(oldDocumentType - MCbaseDat) );
        }
    }
}

bool MainWindow::saveDocumentFile(const QString& fileName)
{
    bool hadDoc = !currentDocument->title().isEmpty();

    bool ret = currentDocument->onSaveDocument(fileName);

    if(ret) {
        if(!hadDoc) updateUi();
        updateUiTitle();

        if(currentDocumentType != Invalid)
            addToRecentFiles(fileName, currentDocumentType);
    }

    return ret;
}

void MainWindow::addToRecentFiles(const QString& fileName, ControlFileType type)
{
    if(fileName.isEmpty()) return;

    Pair pair(fileName, type);
    recentFiles.removeAll(pair);
    recentFiles.prepend(pair);
    updateRecentFileActions();
}

void MainWindow::removeFromRecentFiles(const QString& fileName, ControlFileType type)
{
    if(fileName.isEmpty()) return;

    Pair pair(fileName, type);
    if(recentFiles.removeAll(pair) > 0)
        updateRecentFileActions();
}

MainWindow::ControlFileType MainWindow::getDocumentType(Document* document)
{
    ControlFileType type;

    if(!document)
        type = Invalid;
    else if( typeid(*document) == typeid(MCMCTreeEngine) )
        type = MCMCTreeCtl;
    else if( typeid(*document) == typeid(BasemlEngine) )
        type = BaseMLCtl;
    else if( typeid(*document) == typeid(CodemlEngine) )
        type = CodeMLCtl;
    else if( typeid(*document) == typeid(PampEngine) )
        type = PAMPCtl;
    else if( typeid(*document) == typeid(YN00Engine) )
        type = YN00Ctl;
    else if( typeid(*document) == typeid(EvolverEngine) )
        type = ControlFileType(MCbaseDat + int(evolverEngine->fileType()));
    else
        type = Invalid;

    return type;
}

bool MainWindow::saveModifiedFile(Document* document, bool saveInBackground)
{
    bool retVal = true;

    if( document && document->isModified() && !document->title().isEmpty() ) {
        Document* oldDocument = currentDocument;
        ControlFileType oldDocumentType = currentDocumentType;

        if(document != currentDocument && !saveInBackground) {
            ControlFileType documentType = getDocumentType(document);
            switch(documentType)
            {
            case MCMCTreeCtl:
                actionMCMCTree->trigger();
                break;
            case BaseMLCtl:
                actionBaseml->trigger();
                break;
            case CodeMLCtl:
                actionCodeml->trigger();
                break;
            case PAMPCtl:
                actionPAMP->trigger();
                break;
            case YN00Ctl:
                actionYN00->trigger();
                break;
            case MCbaseDat:
            case MCcodonDat:
            case MCaaDat:
                actionEvolver->trigger();
                break;
            default:
                saveInBackground = true;
            }
        }

        QString name = document->fileName();
        if(name.isEmpty())
            name = document->title();

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setWindowTitle(tr("Save Changes"));
        msgBox.setText(tr("Save changes to:"));
        msgBox.setInformativeText(QDir::toNativeSeparators(name));
        msgBox.addButton(QMessageBox::Yes);
        msgBox.addButton(QMessageBox::No);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();

        if(ret == QMessageBox::Yes) {
            bool ret2;

            if(document != currentDocument && saveInBackground) {
                currentDocument = document;
                currentDocumentType = getDocumentType(document);

                ret2 = on_actionSave_triggered();

                currentDocument = oldDocument;
                currentDocumentType = oldDocumentType;

                if(ret2) updateUiTitle();
            }
            else
                ret2 = on_actionSave_triggered();

            if(!ret2) retVal = false;
        }
        else if(ret == QMessageBox::Cancel) {
            retVal = false;
        }

        if(oldDocument != currentDocument && !saveInBackground && !retVal) {
            switch(oldDocumentType)
            {
            case MCMCTreeCtl:
                actionMCMCTree->trigger();
                break;
            case BaseMLCtl:
                actionBaseml->trigger();
                break;
            case CodeMLCtl:
                actionCodeml->trigger();
                break;
            case PAMPCtl:
                actionPAMP->trigger();
                break;
            case YN00Ctl:
                actionYN00->trigger();
                break;
            case MCbaseDat:
            case MCcodonDat:
            case MCaaDat:
                actionEvolver->trigger();
                break;
            case Invalid:
                actionStart_Page->trigger();
                break;
            default:;
            }
        }
    }

    return retVal;
}

void MainWindow::setModifiedFlag()
{
    if( currentDocument && !currentDocument->isModified() && !currentDocument->title().isEmpty() ) {
        currentDocument->setModified(true);
        setWindowModified(true);
    }
}

void MainWindow::updateEnginesPath()
{
    if(mcmcTreeEngine) {
        mcmcTreeEngine->setPath(absolutePAMLPath);
        if( mcmcTreeEngine->state() == Engine::NotRunning ) {
            groupBox_8->setHidden( mcmcTreeEngine->compareVersion("4.9") >= 0 );
            pushButton->setEnabled( mcmcTreeEngine->isStatusOk() );
        }
    }

    if(basemlEngine) {
        basemlEngine->setPath(absolutePAMLPath);
        if( basemlEngine->state() == Engine::NotRunning )
            pushButton_5->setEnabled( basemlEngine->isStatusOk() );
    }

    if(codemlEngine) {
        codemlEngine->setPath(absolutePAMLPath);
        if( codemlEngine->state() == Engine::NotRunning )
            pushButton_7->setEnabled( codemlEngine->isStatusOk() );
    }

    if(pampEngine) {
        pampEngine->setPath(absolutePAMLPath);
        if( pampEngine->state() == Engine::NotRunning )
            pushButton_11->setEnabled( pampEngine->isStatusOk() );
    }

    if(yn00Engine) {
        yn00Engine->setPath(absolutePAMLPath);
        if( yn00Engine->state() == Engine::NotRunning )
            pushButton_13->setEnabled( yn00Engine->isStatusOk() );
    }

    if(evolverEngine) {
        evolverEngine->setPath(absolutePAMLPath);
        if( evolverEngine->state() == Engine::NotRunning )
            pushButton_8->setEnabled( evolverEngine->isStatusOk() );
    }

    if(chi2Dialog)
        chi2Dialog->setPath(absolutePAMLPath);
}

void MainWindow::updateOutputFont()
{
    //QTextEdit font
    if(mcmcTreeOutput)
        mcmcTreeOutput->textEdit->setCurrentFont(outputFont);
    if(basemlOutput)
        basemlOutput->textEdit->setCurrentFont(outputFont);
    if(codemlOutput)
        codemlOutput->textEdit->setCurrentFont(outputFont);
    if(pampOutput)
        pampOutput->textEdit->setCurrentFont(outputFont);
    if(yn00Output)
        yn00Output->textEdit->setCurrentFont(outputFont);
    if(evolverOutput)
        evolverOutput->textEdit->setCurrentFont(outputFont);
}

void MainWindow::updateRecentFileActions()
{
    int i=0;
    int size = recentFiles.size();

    for(; i<MaxRecentFiles && i<size; i++) {
        QString text = tr("&%1 %2").arg(i+1).arg(QDir::toNativeSeparators(recentFiles[i].fileName));
        recentFileActions[i]->setText(text);
        recentFileActions[i]->setVisible(true);
    }

    for(; i<MaxRecentFiles; i++) {
        recentFileActions[i]->setVisible(false);
    }
}

void MainWindow::updateUiTitle()
{
    QString appName = QApplication::applicationName();

    int index = stackedWidget->currentIndex();
    if(index < 0 || index >= int(sizeof(strList)/sizeof(char*))) index = 0;
    QString appTitle = strList[index];

    QString fileTitle;
    if(currentDocument)
        fileTitle = currentDocument->title();

    if(appTitle.isEmpty()) {
        setWindowTitle(appName);
        setWindowModified(false);
    }
    else if(fileTitle.isEmpty()) {
        setWindowTitle( tr("%1  -  %2").arg(appName).arg(appTitle) );
        setWindowModified(false);
    }
    else if(!actionText_View->isChecked()) {
        setWindowTitle( tr("%1  -  %2  -  [%3[*]]").arg(appName).arg(appTitle).arg(fileTitle) );
        setWindowModified( currentDocument->isModified() );
    }
    else {
        setWindowTitle( tr("%1  -  %2  -  [%3[*]] - Text View (ReadOnly)").arg(appName).arg(appTitle).arg(fileTitle) );
        setWindowModified( currentDocument->isModified() );
    }
}

void MainWindow::updateUi()
{
    int index = stackedWidget->currentIndex();

    switch(index)
    {
    case MCMCTree:
        {
        if(!mcmcTreeEngine) return;

        lineEdit_5->setText( QString::number(mcmcTreeEngine->nEdit_seed) );
        comboBox_20->lineEdit()->setText( mcmcTreeEngine->strEdit_seqfile );
        comboBox_21->lineEdit()->setText( mcmcTreeEngine->strEdit_treefile );
        lineEdit_3->setText( mcmcTreeEngine->strEdit_outfile_Name );
        comboBox_22->lineEdit()->setText( mcmcTreeEngine->strEdit_outfile_Location );
        comboBox->setCurrentIndex( mcmcTreeEngine->nComb_usedata );
        spinBox_7->setValue( mcmcTreeEngine->nEdit_ndata );
        comboBox_2->setCurrentIndex( mcmcTreeEngine->nComb_model );
        comboBox_3->setCurrentIndex( mcmcTreeEngine->nComb_clock - 1 );
        doubleSpinBox_19->setValue( mcmcTreeEngine->nEdit_TipDate );
        doubleSpinBox_20->setValue( mcmcTreeEngine->nEdit_TipDate_TimeUnit );
        lineEdit_6->setText( mcmcTreeEngine->strEdit_RootAge );
        doubleSpinBox_17->setValue( mcmcTreeEngine->nEdit_alpha );
        spinBox_2->setValue( mcmcTreeEngine->nEdit_ncatG );
        checkBox_2->setChecked( mcmcTreeEngine->bButt_cleandata );
        doubleSpinBox_7->setValue( mcmcTreeEngine->nEdit_BDparas[0] );
        doubleSpinBox_8->setValue( mcmcTreeEngine->nEdit_BDparas[1] );
        doubleSpinBox_13->setValue( mcmcTreeEngine->nEdit_BDparas[2] );
        doubleSpinBox_18->setValue( mcmcTreeEngine->nEdit_BDparas[3] );
        doubleSpinBox->setValue( mcmcTreeEngine->nEdit_kappa_gamma[0] );
        doubleSpinBox_2->setValue( mcmcTreeEngine->nEdit_kappa_gamma[1] );
        doubleSpinBox_3->setValue( mcmcTreeEngine->nEdit_alpha_gamma[0] );
        doubleSpinBox_4->setValue( mcmcTreeEngine->nEdit_alpha_gamma[1] );
        comboBox_50->setCurrentIndex( mcmcTreeEngine->nComb_rgeneprior );
        doubleSpinBox_5->setValue( mcmcTreeEngine->nEdit_rgene_para[0] );
        doubleSpinBox_6->setValue( mcmcTreeEngine->nEdit_rgene_para[1] );
        doubleSpinBox_27->setValue( mcmcTreeEngine->nEdit_rgene_para[2] );
        doubleSpinBox_9->setValue( mcmcTreeEngine->nEdit_sigma2_para[0] );
        doubleSpinBox_10->setValue( mcmcTreeEngine->nEdit_sigma2_para[1] );
        doubleSpinBox_28->setValue( mcmcTreeEngine->nEdit_sigma2_para[2] );
        checkBox_3->setChecked( mcmcTreeEngine->bButt_print );
        spinBox_3->setValue( mcmcTreeEngine->nEdit_burnin );
        spinBox_4->setValue( mcmcTreeEngine->nEdit_sampfreq );
        spinBox_5->setValue( mcmcTreeEngine->nEdit_nsample );
        if( mcmcTreeEngine->compareVersion("4.9") < 0 ) {
            checkBox->setChecked( mcmcTreeEngine->bButt_autoAdjustFinetune );
            doubleSpinBox_11->setValue( mcmcTreeEngine->nEdit_finetune[0] );
            doubleSpinBox_12->setValue( mcmcTreeEngine->nEdit_finetune[1] );
            doubleSpinBox_14->setValue( mcmcTreeEngine->nEdit_finetune[2] );
            doubleSpinBox_15->setValue( mcmcTreeEngine->nEdit_finetune[3] );
            doubleSpinBox_16->setValue( mcmcTreeEngine->nEdit_finetune[4] );
        }
        }
        break;

    case BaseML:
        {
        if(!basemlEngine) return;

        comboBox_17->lineEdit()->setText( basemlEngine->strEdit_seqfile );
        comboBox_18->lineEdit()->setText( basemlEngine->strEdit_treefile );
        lineEdit_9->setText( basemlEngine->strEdit_outfile_Name );
        comboBox_19->lineEdit()->setText( basemlEngine->strEdit_outfile_Location );
        checkBox_5->setChecked( basemlEngine->bButt_cleandata );
        comboBox_16->setCurrentIndex( basemlEngine->nComb_runmode );
        comboBox_15->setCurrentIndex( basemlEngine->nComb_method );
        comboBox_6->setCurrentIndex( basemlEngine->nComb_clock );
        doubleSpinBox_21->setValue( basemlEngine->nEdit_TipDate );
        doubleSpinBox_22->setValue( basemlEngine->nEdit_TipDate_TimeUnit );
        comboBox_4->setCurrentIndex( basemlEngine->nComb_Mgene );
        comboBox_14->setCurrentIndex( basemlEngine->nComb_nhomo );
        checkBox_9->setChecked( basemlEngine->bButt_getSE );
        checkBox_10->setChecked( basemlEngine->bButt_RateAncestor );
        comboBox_5->setCurrentIndex( basemlEngine->nComb_model );
        checkBox_7->setChecked( basemlEngine->bButt_fix_kappa );
        doubleSpinBox_36->setValue( basemlEngine->nEdit_kappa );
        checkBox_8->setChecked( basemlEngine->bButt_fix_alpha );
        doubleSpinBox_23->setValue( basemlEngine->nEdit_alpha );
        checkBox_6->setChecked( basemlEngine->bButt_Malpha );
        spinBox_6->setValue( basemlEngine->nEdit_ncatG );
        checkBox_13->setChecked( basemlEngine->bButt_fix_rho );
        doubleSpinBox_24->setValue( basemlEngine->nEdit_rho );
        comboBox_13->setCurrentIndex( basemlEngine->nComb_nparK );
        spinBox_8->setValue( basemlEngine->nEdit_ndata );
        lineEdit_12->setText( basemlEngine->strEdit_Small_Diff );
        comboBox_26->setCurrentIndex( basemlEngine->nComb_icode + 1 );
        comboBox_8->setCurrentIndex( basemlEngine->nComb_fix_blength + 1 );
        }
        break;

    case CodeML:
        {
        if(!codemlEngine) return;

        comboBox_23->lineEdit()->setText( codemlEngine->strEdit_seqfile );
        comboBox_24->lineEdit()->setText( codemlEngine->strEdit_treefile );
        lineEdit_10->setText( codemlEngine->strEdit_outfile_Name );
        comboBox_25->lineEdit()->setText( codemlEngine->strEdit_outfile_Location );
        comboBox_31->setCurrentIndex( codemlEngine->nComb_seqtype - 1 );
        radioButton_2->setChecked(true);
        checkBox_25->setChecked( codemlEngine->bButt_cleandata );
        int i = codemlEngine->nComb_runmode;
        if(i == -2) i = 6;
        else if(i == -3) i = 7;
        else if(i >= 6) i = -1;
        comboBox_38->setCurrentIndex(i);
        doubleSpinBox_33->setValue( codemlEngine->nEdit_t_gamma[0] );
        doubleSpinBox_34->setValue( codemlEngine->nEdit_t_gamma[1] );
        doubleSpinBox_38->setValue( codemlEngine->nEdit_w_gamma[0] );
        doubleSpinBox_42->setValue( codemlEngine->nEdit_w_gamma[1] );
        comboBox_44->setCurrentIndex( codemlEngine->nComb_method );
        comboBox_41->setCurrentIndex( codemlEngine->nComb_clock );
        checkBox_22->setChecked( codemlEngine->bButt_getSE );
        checkBox_23->setChecked( codemlEngine->bButt_RateAncestor );
        comboBox_42->setCurrentIndex( codemlEngine->nComb_CodonFreq );
        comboBox_47->setCurrentIndex( codemlEngine->nComb_estFreq );
        bool ok;
        int num = comboBox_39->count();
        for(i=0; i<num; i++)
            if( comboBox_39->itemData(i).toInt(&ok) == codemlEngine->nComb_model && ok ) {
                comboBox_39->setCurrentIndex(i);
                break;
            }
        if(i == num) comboBox_39->setCurrentIndex(-1);
        i = codemlEngine->nComb_aaDist;
        if(i < -6 || (i > 7 && i < 11)) i = -1;
        else if( i >= -6 && i < 0) i = 6 - i;
        else if(i == 7) i = 13;
        else if(i >= 11) i += 3;
        comboBox_43->setCurrentIndex(i);
        comboBox_45->lineEdit()->setText( codemlEngine->strEdit_aaRatefile );
        num = listWidget->count();
        for(int i=0,j=0; i<num && j<CodemlEngine::maxNSsitesModels; j++) {
            CodemlEngine::Trilean t = codemlEngine->triList_nsmodels[j];
            if(t != CodemlEngine::Null) {
                listWidget->item(i)->setCheckState( (bool(t))? Qt::Checked : Qt::Unchecked );
                i++;
            }
        }
        comboBox_46->setCurrentIndex( codemlEngine->nComb_icode );
        num = comboBox_40->count();
        for(i=0; i<num; i++)
            if( comboBox_40->itemData(i).toInt(&ok) == codemlEngine->nComb_Mgene && ok ) {
                comboBox_40->setCurrentIndex(i);
                break;
            }
        if(i == num) comboBox_40->setCurrentIndex(-1);
        checkBox_19->setChecked( codemlEngine->bButt_fix_kappa );
        doubleSpinBox_41->setValue( codemlEngine->nEdit_kappa );
        checkBox_26->setChecked( codemlEngine->bButt_fix_omega );
        doubleSpinBox_26->setValue( codemlEngine->nEdit_omega );
        checkBox_20->setChecked( codemlEngine->bButt_fix_alpha );
        doubleSpinBox_25->setValue( codemlEngine->nEdit_alpha );
        checkBox_21->setChecked( codemlEngine->bButt_Malpha );
        spinBox_11->setValue( codemlEngine->nEdit_ncatG );
        spinBox_16->setValue( codemlEngine->nEdit_ndata );
        lineEdit_14->setText( codemlEngine->strEdit_Small_Diff );
        comboBox_49->setCurrentIndex( codemlEngine->nComb_fix_blength + 1 );
        }
        break;

    case PAMP:
        {
        if(!pampEngine) return;

        comboBox_27->lineEdit()->setText( pampEngine->strEdit_seqfile );
        comboBox_28->lineEdit()->setText( pampEngine->strEdit_treefile );
        lineEdit_4->setText( pampEngine->strEdit_outfile_Name );
        comboBox_29->lineEdit()->setText( pampEngine->strEdit_outfile_Location );
        int i = pampEngine->nComb_seqtype;
        if(i > 1) i--;
        comboBox_32->setCurrentIndex(i);
        comboBox_30->setCurrentIndex( pampEngine->nComb_nhomo );
        spinBox_9->setValue( pampEngine->nEdit_ncatG );
        }
        break;

    case YN00:
        {
        if(!yn00Engine) return;

        comboBox_33->lineEdit()->setText( yn00Engine->strEdit_seqfile );
        lineEdit_7->setText( yn00Engine->strEdit_outfile_Name );
        comboBox_35->lineEdit()->setText( yn00Engine->strEdit_outfile_Location );
        comboBox_34->setCurrentIndex( yn00Engine->nComb_verbose );
        int i = yn00Engine->nComb_noisy;
        if(i > 4 && i < 9) i = 4;
        else if(i >= 9) i = 5;
        comboBox_36->setCurrentIndex(i);
        comboBox_48->setCurrentIndex( yn00Engine->nComb_icode );
        spinBox_10->setValue( yn00Engine->nEdit_ndata );
        checkBox_4->setChecked( yn00Engine->bButt_weighting );
        checkBox_11->setChecked( yn00Engine->bButt_commonkappa );
        checkBox_12->setChecked( yn00Engine->bButt_commonf3x4 );
        }
        break;

    case Evolver:
        {
        if(!evolverEngine) return;

        comboBox_10->lineEdit()->setText( evolverEngine->strEdit_Output_Location );
        comboBox_9->setCurrentIndex( evolverEngine->nComb_outFormat );

        EvolverEngine::Type type = evolverEngine->fileType();
        tabWidget->setCurrentIndex(int(type));

        if( !evolverEngine->title().isEmpty() ) {
            tabWidget->setTabEnabled(0, 0 == int(type));
            tabWidget->setTabEnabled(1, 1 == int(type));
            tabWidget->setTabEnabled(2, 2 == int(type));
        }
        else {
            tabWidget->setTabEnabled(0, true);
            tabWidget->setTabEnabled(1, true);
            tabWidget->setTabEnabled(2, true);
        }

        switch(type)
        {
        case EvolverEngine::Nucleotide:
            {
            lineEdit_18->setText( QString::number(evolverEngine->nEdit_seed) );
            spinBox_14->setValue( evolverEngine->nEdit_seqs );
            spinBox_13->setValue( evolverEngine->nEdit_sites );
            spinBox_15->setValue( evolverEngine->nEdit_replicates );
            lineEdit_25->setText( QString::number(evolverEngine->nEdit_treeLength) );
            lineEdit_20->setText( evolverEngine->strEdit_tree );
            comboBox_7->setCurrentIndex( evolverEngine->nComb_model );
            lineEdit_19->setText( evolverEngine->strEdit_parameters );
            doubleSpinBox_35->setValue( evolverEngine->nEdit_alpha );
            spinBox_12->setValue( evolverEngine->nEdit_ncatG );
            doubleSpinBox_47->setValue( evolverEngine->nEdit_baseFrequencies[0] );
            doubleSpinBox_48->setValue( evolverEngine->nEdit_baseFrequencies[1] );
            doubleSpinBox_49->setValue( evolverEngine->nEdit_baseFrequencies[2] );
            doubleSpinBox_50->setValue( evolverEngine->nEdit_baseFrequencies[3] );
            }
            break;

        case EvolverEngine::Codon:
            {
            lineEdit_33->setText( QString::number(evolverEngine->nEdit_seed) );
            spinBox_21->setValue( evolverEngine->nEdit_seqs );
            spinBox_22->setValue( evolverEngine->nEdit_sites );
            spinBox_20->setValue( evolverEngine->nEdit_replicates );
            lineEdit_32->setText( QString::number(evolverEngine->nEdit_treeLength) );
            lineEdit_34->setText( evolverEngine->strEdit_tree );
            doubleSpinBox_39->setValue( evolverEngine->nEdit_omega );
            doubleSpinBox_40->setValue( evolverEngine->nEdit_kappa );

            for(int i=0; i<64; i++)
                codonFrequencyDialog->tableWidget->item(i/4, i%4)->setText( QString::number(evolverEngine->nEdit_codonFrequencies[i]) );
            }
            break;

        case EvolverEngine::AminoAcid:
            {
            lineEdit_24->setText( QString::number(evolverEngine->nEdit_seed) );
            spinBox_24->setValue( evolverEngine->nEdit_seqs );
            spinBox_25->setValue( evolverEngine->nEdit_sites );
            spinBox_23->setValue( evolverEngine->nEdit_replicates );
            lineEdit_28->setText( QString::number(evolverEngine->nEdit_treeLength) );
            lineEdit_29->setText( evolverEngine->strEdit_tree );
            doubleSpinBox_37->setValue( evolverEngine->nEdit_alpha );
            spinBox_30->setValue( evolverEngine->nEdit_ncatG );
            comboBox_12->setCurrentIndex( evolverEngine->nComb_model );
            comboBox_11->lineEdit()->setText( evolverEngine->strEdit_aminoAcidSbstRtFile );

            for(int i=0; i<20; i++)
                aminoAcidFrequencyDialog->tableWidget->item(i, 1)->setText( QString::number(evolverEngine->nEdit_aminoAcidFrequencies[i]) );
            }
            break;

        default:;
        }
        }
        break;

    default:;
    }
}

void MainWindow::updateEngine()
{
    int index = stackedWidget->currentIndex();

    switch(index)
    {
    case MCMCTree:
        {
        if(!mcmcTreeEngine) return;

        mcmcTreeEngine->nEdit_seed = lineEdit_5->text().toInt();
        mcmcTreeEngine->strEdit_seqfile = comboBox_20->lineEdit()->text();
        mcmcTreeEngine->strEdit_treefile = comboBox_21->lineEdit()->text();
        mcmcTreeEngine->strEdit_outfile_Name = lineEdit_3->text();
        mcmcTreeEngine->strEdit_outfile_Location = comboBox_22->lineEdit()->text();
        mcmcTreeEngine->nComb_usedata = comboBox->currentIndex();
        mcmcTreeEngine->nEdit_ndata = spinBox_7->value();
        mcmcTreeEngine->nComb_model = comboBox_2->currentIndex();
        mcmcTreeEngine->nComb_clock = comboBox_3->currentIndex() + 1;
        mcmcTreeEngine->nEdit_TipDate = doubleSpinBox_19->value();
        mcmcTreeEngine->nEdit_TipDate_TimeUnit = doubleSpinBox_20->value();
        mcmcTreeEngine->strEdit_RootAge = lineEdit_6->text();
        mcmcTreeEngine->nEdit_alpha = doubleSpinBox_17->value();
        mcmcTreeEngine->nEdit_ncatG = spinBox_2->value();
        mcmcTreeEngine->bButt_cleandata = checkBox_2->isChecked();
        mcmcTreeEngine->nEdit_BDparas[0] = doubleSpinBox_7->value();
        mcmcTreeEngine->nEdit_BDparas[1] = doubleSpinBox_8->value();
        mcmcTreeEngine->nEdit_BDparas[2] = doubleSpinBox_13->value();
        mcmcTreeEngine->nEdit_BDparas[3] = doubleSpinBox_18->value();
        mcmcTreeEngine->nEdit_kappa_gamma[0] = doubleSpinBox->value();
        mcmcTreeEngine->nEdit_kappa_gamma[1] = doubleSpinBox_2->value();
        mcmcTreeEngine->nEdit_alpha_gamma[0] = doubleSpinBox_3->value();
        mcmcTreeEngine->nEdit_alpha_gamma[1] = doubleSpinBox_4->value();
        mcmcTreeEngine->nComb_rgeneprior = comboBox_50->currentIndex();
        mcmcTreeEngine->nEdit_rgene_para[0] = doubleSpinBox_5->value();
        mcmcTreeEngine->nEdit_rgene_para[1] = doubleSpinBox_6->value();
        mcmcTreeEngine->nEdit_rgene_para[2] = doubleSpinBox_27->value();
        mcmcTreeEngine->nEdit_sigma2_para[0] = doubleSpinBox_9->value();
        mcmcTreeEngine->nEdit_sigma2_para[1] = doubleSpinBox_10->value();
        mcmcTreeEngine->nEdit_sigma2_para[2] = doubleSpinBox_28->value();
        mcmcTreeEngine->bButt_print = checkBox_3->isChecked();
        mcmcTreeEngine->nEdit_burnin = spinBox_3->value();
        mcmcTreeEngine->nEdit_sampfreq = spinBox_4->value();
        mcmcTreeEngine->nEdit_nsample = spinBox_5->value();
        if( mcmcTreeEngine->compareVersion("4.9") < 0 ) {
            mcmcTreeEngine->bButt_autoAdjustFinetune = checkBox->isChecked();
            mcmcTreeEngine->nEdit_finetune[0] = doubleSpinBox_11->value();
            mcmcTreeEngine->nEdit_finetune[1] = doubleSpinBox_12->value();
            mcmcTreeEngine->nEdit_finetune[2] = doubleSpinBox_14->value();
            mcmcTreeEngine->nEdit_finetune[3] = doubleSpinBox_15->value();
            mcmcTreeEngine->nEdit_finetune[4] = doubleSpinBox_16->value();
        }
        }
        break;

    case BaseML:
        {
        if(!basemlEngine) return;

        basemlEngine->strEdit_seqfile = comboBox_17->lineEdit()->text();
        basemlEngine->strEdit_treefile = comboBox_18->lineEdit()->text();
        basemlEngine->strEdit_outfile_Name = lineEdit_9->text();
        basemlEngine->strEdit_outfile_Location = comboBox_19->lineEdit()->text();
        basemlEngine->bButt_cleandata = checkBox_5->isChecked();
        basemlEngine->nComb_runmode = comboBox_16->currentIndex();
        basemlEngine->nComb_method = comboBox_15->currentIndex();
        basemlEngine->nComb_clock = comboBox_6->currentIndex();
        basemlEngine->nEdit_TipDate = doubleSpinBox_21->value();
        basemlEngine->nEdit_TipDate_TimeUnit = doubleSpinBox_22->value();
        basemlEngine->nComb_Mgene = comboBox_4->currentIndex();
        basemlEngine->nComb_nhomo = comboBox_14->currentIndex();
        basemlEngine->bButt_getSE = checkBox_9->isChecked();
        basemlEngine->bButt_RateAncestor = checkBox_10->isChecked();
        basemlEngine->nComb_model = comboBox_5->currentIndex();
        basemlEngine->bButt_fix_kappa = checkBox_7->isChecked();
        basemlEngine->nEdit_kappa = doubleSpinBox_36->value();
        basemlEngine->bButt_fix_alpha = checkBox_8->isChecked();
        basemlEngine->nEdit_alpha = doubleSpinBox_23->value();
        basemlEngine->bButt_Malpha = checkBox_6->isChecked();
        basemlEngine->nEdit_ncatG = spinBox_6->value();
        basemlEngine->bButt_fix_rho = checkBox_13->isChecked();
        basemlEngine->nEdit_rho = doubleSpinBox_24->value();
        basemlEngine->nComb_nparK = comboBox_13->currentIndex();
        basemlEngine->nEdit_ndata = spinBox_8->value();
        basemlEngine->strEdit_Small_Diff = lineEdit_12->text();
        basemlEngine->nComb_icode = comboBox_26->currentIndex() - 1;
        basemlEngine->nComb_fix_blength = comboBox_8->currentIndex() - 1;
        }
        break;

    case CodeML:
        {
        if(!codemlEngine) return;

        codemlEngine->strEdit_seqfile = comboBox_23->lineEdit()->text();
        codemlEngine->strEdit_treefile = comboBox_24->lineEdit()->text();
        codemlEngine->strEdit_outfile_Name = lineEdit_10->text();
        codemlEngine->strEdit_outfile_Location = comboBox_25->lineEdit()->text();
        codemlEngine->nComb_seqtype = comboBox_31->currentIndex() + 1;
        codemlEngine->bButt_cleandata = checkBox_25->isChecked();
        int i = comboBox_38->currentIndex();
        if(i == 6) i = -2;
        else if(i == 7) i = -3;
        codemlEngine->nComb_runmode = i;
        codemlEngine->nEdit_t_gamma[0] = doubleSpinBox_33->value();
        codemlEngine->nEdit_t_gamma[1] = doubleSpinBox_34->value();
        codemlEngine->nEdit_w_gamma[0] = doubleSpinBox_38->value();
        codemlEngine->nEdit_w_gamma[1] = doubleSpinBox_42->value();
        codemlEngine->nComb_method = comboBox_44->currentIndex();
        codemlEngine->nComb_clock = comboBox_41->currentIndex();
        codemlEngine->bButt_getSE = checkBox_22->isChecked();
        codemlEngine->bButt_RateAncestor = checkBox_23->isChecked();
        codemlEngine->nComb_CodonFreq = comboBox_42->currentIndex();
        codemlEngine->nComb_estFreq = comboBox_47->currentIndex();
        i = comboBox_39->currentIndex();
        QVariant variant = comboBox_39->itemData(i);
        if( i == -1 || variant.type() != QVariant::Invalid )
            codemlEngine->nComb_model = (i == -1)? -1 : variant.toInt();
        i = comboBox_43->currentIndex();
        if(i == -1) i = -7;
        else if(i > 6 && i < 13) i = 6 - i;
        else if(i == 13) i = 7;
        else if(i > 13) i -= 3;
        codemlEngine->nComb_aaDist = i;
        codemlEngine->strEdit_aaRatefile = comboBox_45->lineEdit()->text();
        int num = listWidget->count();
        for(int i=0,j=0; i<num && j<CodemlEngine::maxNSsitesModels; j++) {
            CodemlEngine::Trilean& t = codemlEngine->triList_nsmodels[j];
            if(t != CodemlEngine::Null) {
                t = (bool(listWidget->item(i)->checkState()))? CodemlEngine::True : CodemlEngine::False;
                i++;
            }
        }
        codemlEngine->nComb_icode = comboBox_46->currentIndex();
        i = comboBox_40->currentIndex();
        variant = comboBox_40->itemData(i);
        if( i == -1 || variant.type() != QVariant::Invalid )
            codemlEngine->nComb_Mgene = (i == -1)? -1 : variant.toInt();
        codemlEngine->bButt_fix_kappa = checkBox_19->isChecked();
        codemlEngine->nEdit_kappa = doubleSpinBox_41->value();
        codemlEngine->bButt_fix_omega = checkBox_26->isChecked();
        codemlEngine->nEdit_omega = doubleSpinBox_26->value();
        codemlEngine->bButt_fix_alpha = checkBox_20->isChecked();
        codemlEngine->nEdit_alpha = doubleSpinBox_25->value();
        codemlEngine->bButt_Malpha = checkBox_21->isChecked();
        codemlEngine->nEdit_ncatG = spinBox_11->value();
        codemlEngine->nEdit_ndata = spinBox_16->value();
        codemlEngine->strEdit_Small_Diff = lineEdit_14->text();
        codemlEngine->nComb_fix_blength = comboBox_49->currentIndex() - 1;
        }
        break;

    case PAMP:
        {
        if(!pampEngine) return;

        pampEngine->strEdit_seqfile = comboBox_27->lineEdit()->text();
        pampEngine->strEdit_treefile = comboBox_28->lineEdit()->text();
        pampEngine->strEdit_outfile_Name = lineEdit_4->text();
        pampEngine->strEdit_outfile_Location = comboBox_29->lineEdit()->text();
        int i = comboBox_32->currentIndex();
        if(i > 0) i++;
        pampEngine->nComb_seqtype = i;
        pampEngine->nComb_nhomo = comboBox_30->currentIndex();
        pampEngine->nEdit_ncatG = spinBox_9->value();
        }
        break;

    case YN00:
        {
        if(!yn00Engine) return;

        yn00Engine->strEdit_seqfile = comboBox_33->lineEdit()->text();
        yn00Engine->strEdit_outfile_Name = lineEdit_7->text();
        yn00Engine->strEdit_outfile_Location = comboBox_35->lineEdit()->text();
        yn00Engine->nComb_verbose = comboBox_34->currentIndex();
        int i = comboBox_36->currentIndex();
        if(i == 5) i = 9;
        yn00Engine->nComb_noisy = i;
        yn00Engine->nComb_icode = comboBox_48->currentIndex();
        yn00Engine->nEdit_ndata = spinBox_10->value();
        yn00Engine->bButt_weighting = checkBox_4->isChecked();
        yn00Engine->bButt_commonkappa = checkBox_11->isChecked();
        yn00Engine->bButt_commonf3x4 = checkBox_12->isChecked();
        }
        break;

    case Evolver:
        {
        if(!evolverEngine) return;

        evolverEngine->strEdit_Output_Location = comboBox_10->lineEdit()->text();
        evolverEngine->nComb_outFormat = comboBox_9->currentIndex();

        EvolverEngine::Type type = EvolverEngine::Type(tabWidget->currentIndex());
        evolverEngine->setFileType(type);

        switch(type)
        {
        case EvolverEngine::Nucleotide:
            {
            evolverEngine->nEdit_seed = lineEdit_18->text().toInt();
            evolverEngine->nEdit_seqs = spinBox_14->value();
            evolverEngine->nEdit_sites = spinBox_13->value();
            evolverEngine->nEdit_replicates = spinBox_15->value();
            evolverEngine->nEdit_treeLength = lineEdit_25->text().toDouble();
            evolverEngine->strEdit_tree = lineEdit_20->text();
            evolverEngine->nComb_model = comboBox_7->currentIndex();
            evolverEngine->strEdit_parameters = lineEdit_19->text();
            evolverEngine->nEdit_alpha = doubleSpinBox_35->value();
            evolverEngine->nEdit_ncatG = spinBox_12->value();
            evolverEngine->nEdit_baseFrequencies[0] = doubleSpinBox_47->value();
            evolverEngine->nEdit_baseFrequencies[1] = doubleSpinBox_48->value();
            evolverEngine->nEdit_baseFrequencies[2] = doubleSpinBox_49->value();
            evolverEngine->nEdit_baseFrequencies[3] = doubleSpinBox_50->value();
            }
            break;

        case EvolverEngine::Codon:
            {
            evolverEngine->nEdit_seed = lineEdit_33->text().toInt();
            evolverEngine->nEdit_seqs = spinBox_21->value();
            evolverEngine->nEdit_sites = spinBox_22->value();
            evolverEngine->nEdit_replicates = spinBox_20->value();
            evolverEngine->nEdit_treeLength = lineEdit_32->text().toDouble();
            evolverEngine->strEdit_tree = lineEdit_34->text();
            evolverEngine->nEdit_omega = doubleSpinBox_39->value();
            evolverEngine->nEdit_kappa = doubleSpinBox_40->value();

            for(int i=0; i<64; i++)
                evolverEngine->nEdit_codonFrequencies[i] = codonFrequencyDialog->tableWidget->item(i/4, i%4)->text().toDouble();
            }
            break;

        case EvolverEngine::AminoAcid:
            {
            evolverEngine->nEdit_seed = lineEdit_24->text().toInt();
            evolverEngine->nEdit_seqs = spinBox_24->value();
            evolverEngine->nEdit_sites = spinBox_25->value();
            evolverEngine->nEdit_replicates = spinBox_23->value();
            evolverEngine->nEdit_treeLength = lineEdit_28->text().toDouble();
            evolverEngine->strEdit_tree = lineEdit_29->text();
            evolverEngine->nEdit_alpha = doubleSpinBox_37->value();
            evolverEngine->nEdit_ncatG = spinBox_30->value();
            evolverEngine->nComb_model = comboBox_12->currentIndex();
            evolverEngine->strEdit_aminoAcidSbstRtFile = comboBox_11->lineEdit()->text();

            for(int i=0; i<20; i++)
                evolverEngine->nEdit_aminoAcidFrequencies[i] = aminoAcidFrequencyDialog->tableWidget->item(i, 1)->text().toDouble();
            }
            break;

        default:;
        }
        }
        break;

    default:;
    }
}

void MainWindow::showWarningMessageWhenPathFailed(const QString &aPath, bool isExecutable)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(tr("Warning"));
    QString msg = (isExecutable)? tr("<p>Failed&nbsp;to&nbsp;find&nbsp;executable&nbsp;file&nbsp;\"%1\"&nbsp;in&nbsp;the&nbsp;folder:<br>%2</p>") : tr("<p>Failed&nbsp;to&nbsp;find&nbsp;\"%1\"&nbsp;in&nbsp;the&nbsp;folder:<br>%2</p>");
    QFileInfo fileInfo(aPath);
    msgBox.setText(msg.arg(fileInfo.fileName()).arg(QDir::toNativeSeparators(fileInfo.path())));
#ifdef Q_OS_MAC
    msgBox.setInformativeText(tr("<p>Please&nbsp;set&nbsp;correct&nbsp;paml&nbsp;path&nbsp;in&nbsp;<i>%1&nbsp;&rarr;&nbsp;Preferences...</i></p>").arg(QApplication::applicationName()));
#else
    msgBox.setInformativeText(tr("<p>Please&nbsp;set&nbsp;correct&nbsp;paml&nbsp;path&nbsp;in&nbsp;<i>Tools&nbsp;&rarr;&nbsp;Configuration...</i></p>"));
#endif
    msgBox.exec();
}

void MainWindow::onMCMCTreeEngineStateChanged(Engine::EngineState newState)
{
    switch(newState)
    {
    case Engine::NotRunning:
        {
            groupBox_8->setHidden( mcmcTreeEngine->compareVersion("4.9") >= 0 );
            pushButton->setEnabled( mcmcTreeEngine->isStatusOk() );
            pushButton_2->setEnabled(false);
        }
        break;
    case Engine::Running:
        {
        pushButton_2->setEnabled(true);
        }
        break;
    default:;
    }
}

void MainWindow::onBasemlEngineStateChanged(Engine::EngineState newState)
{
    switch(newState)
    {
    case Engine::NotRunning:
        {
            pushButton_5->setEnabled( basemlEngine->isStatusOk() );
            pushButton_6->setEnabled(false);
        }
        break;
    case Engine::Running:
        {
        pushButton_6->setEnabled(true);
        }
        break;
    default:;
    }
}

void MainWindow::onCodemlEngineStateChanged(Engine::EngineState newState)
{
    switch(newState)
    {
    case Engine::NotRunning:
        {
            pushButton_7->setEnabled( codemlEngine->isStatusOk() );
            pushButton_10->setEnabled(false);
        }
        break;
    case Engine::Running:
        {
        pushButton_10->setEnabled(true);
        }
        break;
    default:;
    }
}

void MainWindow::onPampEngineStateChanged(Engine::EngineState newState)
{
    switch(newState)
    {
    case Engine::NotRunning:
        {
            pushButton_11->setEnabled( pampEngine->isStatusOk() );
            pushButton_12->setEnabled(false);
        }
        break;
    case Engine::Running:
        {
        pushButton_12->setEnabled(true);
        }
        break;
    default:;
    }
}

void MainWindow::onYN00EngineStateChanged(Engine::EngineState newState)
{
    switch(newState)
    {
    case Engine::NotRunning:
        {
            pushButton_13->setEnabled( yn00Engine->isStatusOk() );
            pushButton_14->setEnabled(false);
        }
        break;
    case Engine::Running:
        {
        pushButton_14->setEnabled(true);
        }
        break;
    default:;
    }
}

void MainWindow::onEvolverEngineStateChanged(Engine::EngineState newState)
{
    switch(newState)
    {
    case Engine::NotRunning:
        {
            pushButton_8->setEnabled( evolverEngine->isStatusOk() );
            pushButton_9->setEnabled(false);
        }
        break;
    case Engine::Running:
        {
        pushButton_9->setEnabled(true);
        }
        break;
    default:;
    }
}

void MainWindow::print(QPrinter* printer)
{
    currentDocument->refreshFileContent();

#ifndef QT_NO_PRINTER
    QTextEdit* textEdit = new QTextEdit;

    textEdit->setCurrentFont(printFont);
    textEdit->setText( currentDocument->fileContent() );
    textEdit->print(printer);

    delete textEdit;
#endif
}

void MainWindow::on_stackedWidget_currentChanged(int index)
{
    actionText_View->setChecked(false);

    switch(index)
    {
    case MCMCTree:
        {
        if(!mcmcTreeEngine) {
            setWindowTitle( tr("%1  -  %2").arg(QApplication::applicationName()).arg(strList[MCMCTree]) );

            mcmcTreeEngine = new MCMCTreeEngine(absolutePAMLPath, NULL, 5*1000, 12*3600*1000, this);
            QObject::connect(mcmcTreeEngine, SIGNAL(stateChanged(Engine::EngineState)), this, SLOT(onMCMCTreeEngineStateChanged(Engine::EngineState)));

            groupBox_8->setHidden( mcmcTreeEngine->compareVersion("4.9") >= 0 );

            updateUi();

            if( !mcmcTreeEngine->isStatusOk() ) {
                if( mcmcTreeEngine->state() == Engine::NotRunning )
                    pushButton->setEnabled(false);

                showWarningMessageWhenPathFailed( mcmcTreeEngine->path(), true );
            }
        }

        currentDocument = mcmcTreeEngine;
        currentDocumentType = MCMCTreeCtl;
        }
        break;

    case BaseML:
        {
        if(!basemlEngine) {
            setWindowTitle( tr("%1  -  %2").arg(QApplication::applicationName()).arg(strList[BaseML]) );

            basemlEngine = new BasemlEngine(absolutePAMLPath, NULL, 5*1000, 12*3600*1000, this);
            QObject::connect(basemlEngine, SIGNAL(stateChanged(Engine::EngineState)), this, SLOT(onBasemlEngineStateChanged(Engine::EngineState)));

            updateUi();

            if( !basemlEngine->isStatusOk() ) {
                if( basemlEngine->state() == Engine::NotRunning )
                    pushButton_5->setEnabled(false);

                showWarningMessageWhenPathFailed( basemlEngine->path(), true );
            }
        }

        currentDocument = basemlEngine;
        currentDocumentType = BaseMLCtl;
        }
        break;

    case CodeML:
        {
        if(!codemlEngine) {
            setWindowTitle( tr("%1  -  %2").arg(QApplication::applicationName()).arg(strList[CodeML]) );

            codemlEngine = new CodemlEngine(absolutePAMLPath, NULL, 5*1000, 12*3600*1000, this);
            QObject::connect(codemlEngine, SIGNAL(stateChanged(Engine::EngineState)), this, SLOT(onCodemlEngineStateChanged(Engine::EngineState)));

            updateUi();

            if( !codemlEngine->isStatusOk() ) {
                if( codemlEngine->state() == Engine::NotRunning )
                    pushButton_7->setEnabled(false);

                showWarningMessageWhenPathFailed( codemlEngine->path(), true );
            }
        }

        currentDocument = codemlEngine;
        currentDocumentType = CodeMLCtl;
        }
        break;

    case PAMP:
        {
        if(!pampEngine) {
            setWindowTitle( tr("%1  -  %2").arg(QApplication::applicationName()).arg(strList[PAMP]) );

            pampEngine = new PampEngine(absolutePAMLPath, NULL, 5*1000, 12*3600*1000, this);
            QObject::connect(pampEngine, SIGNAL(stateChanged(Engine::EngineState)), this, SLOT(onPampEngineStateChanged(Engine::EngineState)));

            updateUi();

            if( !pampEngine->isStatusOk() ) {
                if( pampEngine->state() == Engine::NotRunning )
                    pushButton_11->setEnabled(false);

                showWarningMessageWhenPathFailed( pampEngine->path(), true );
            }
        }

        currentDocument = pampEngine;
        currentDocumentType = PAMPCtl;
        }
        break;

    case YN00:
        {
        if(!yn00Engine) {
            setWindowTitle( tr("%1  -  %2").arg(QApplication::applicationName()).arg(strList[YN00]) );

            yn00Engine = new YN00Engine(absolutePAMLPath, NULL, 5*1000, 12*3600*1000, this);
            QObject::connect(yn00Engine, SIGNAL(stateChanged(Engine::EngineState)), this, SLOT(onYN00EngineStateChanged(Engine::EngineState)));

            updateUi();

            if( !yn00Engine->isStatusOk() ) {
                if( yn00Engine->state() == Engine::NotRunning )
                    pushButton_13->setEnabled(false);

                showWarningMessageWhenPathFailed( yn00Engine->path(), true );
            }
        }

        currentDocument = yn00Engine;
        currentDocumentType = YN00Ctl;
        }
        break;

    case Evolver:
        {
        if(!codonFrequencyDialog)
            codonFrequencyDialog = new CodonFrequencyDialog(this);

        if(!aminoAcidFrequencyDialog)
            aminoAcidFrequencyDialog = new AminoAcidFrequencyDialog(this);

        if(!evolverEngine) {
            setWindowTitle( tr("%1  -  %2").arg(QApplication::applicationName()).arg(strList[Evolver]) );

            evolverEngine = new EvolverEngine(absolutePAMLPath, EvolverEngine::Nucleotide, NULL, 5*1000, 12*3600*1000, this);
            QObject::connect(evolverEngine, SIGNAL(stateChanged(Engine::EngineState)), this, SLOT(onEvolverEngineStateChanged(Engine::EngineState)));

            evolverEngine->setFileType(EvolverEngine::AminoAcid);
            updateUi();
            evolverEngine->setFileType(EvolverEngine::Codon);
            updateUi();
            evolverEngine->setFileType(EvolverEngine::Nucleotide);
            updateUi();

            if( !evolverEngine->isStatusOk() ) {
                if( evolverEngine->state() == Engine::NotRunning )
                    pushButton_8->setEnabled(false);

                showWarningMessageWhenPathFailed( evolverEngine->path(), true );
            }
        }

        currentDocument = evolverEngine;
        currentDocumentType = ControlFileType(MCbaseDat + int(evolverEngine->fileType()));
        }
        break;

    default:
        currentDocument = NULL;
        currentDocumentType = Invalid;
    }

    updateUiTitle();
}


//----- Menu -----

void MainWindow::on_menuFile_aboutToShow()
{
    menuNew->setEnabled( !actionText_View->isChecked() );
    actionOpen->setEnabled( !actionText_View->isChecked() );
    actionClose->setEnabled( currentDocument && !currentDocument->title().isEmpty() && !actionText_View->isChecked() );
    actionSave->setEnabled( currentDocument && currentDocument->isModified() );
    actionSave_As->setEnabled( currentDocument );
    actionSet_Printer_Font->setEnabled( currentDocument && !currentDocument->title().isEmpty() );
    actionPrint_Preview->setEnabled( currentDocument && !currentDocument->title().isEmpty() );
    actionPrint->setEnabled( currentDocument && !currentDocument->title().isEmpty() );
    menuRecent_Files->setEnabled( !recentFiles.isEmpty() && !actionText_View->isChecked() );
}

void MainWindow::on_actionMCMCTree_ctl_triggered()
{
    loadDocumentFile(QString(), MCMCTreeCtl);
}

void MainWindow::on_actionBaseml_ctl_triggered()
{
    loadDocumentFile(QString(), BaseMLCtl);
}

void MainWindow::on_actionCodeml_ctl_triggered()
{
    loadDocumentFile(QString(), CodeMLCtl);
}

void MainWindow::on_actionPamp_ctl_triggered()
{
    loadDocumentFile(QString(), PAMPCtl);
}

void MainWindow::on_actionYN00_ctl_triggered()
{
    loadDocumentFile(QString(), YN00Ctl);
}

void MainWindow::on_actionMCbase_dat_triggered()
{
    loadDocumentFile(QString(), MCbaseDat);
}

void MainWindow::on_actionMCcodon_dat_triggered()
{
    loadDocumentFile(QString(), MCcodonDat);
}

void MainWindow::on_actionMCaa_dat_triggered()
{
    loadDocumentFile(QString(), MCaaDat);
}

void MainWindow::on_actionOpen_triggered()
{
    ControlFileType type = currentDocumentType;

    OpenDialog openDialog(type, this);
    if( openDialog.exec() == QDialog::Rejected ) return;

    QString dialogTitle;

    switch(type)
    {
    case MCMCTreeCtl:
        dialogTitle = tr("Open %1 Control File").arg(strList[MCMCTree]);
        break;
    case BaseMLCtl:
        dialogTitle = tr("Open %1 Control File").arg(strList[BaseML]);
        break;
    case CodeMLCtl:
        dialogTitle = tr("Open %1 Control File").arg(strList[CodeML]);
        break;
    case PAMPCtl:
        dialogTitle = tr("Open %1 Control File").arg(strList[PAMP]);
        break;
    case YN00Ctl:
        dialogTitle = tr("Open %1 Control File").arg(strList[YN00]);
        break;
    case MCbaseDat:
        dialogTitle = tr("Open %1 File").arg("MCbase.dat");
        break;
    case MCcodonDat:
        dialogTitle = tr("Open %1 File").arg("MCcodon.dat");
        break;
    case MCaaDat:
        dialogTitle = tr("Open %1 File").arg("MCaa.dat");
        break;
    default:
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this, dialogTitle, lastFileOpenPath);
    if(fileName.isEmpty()) return;

    lastFileOpenPath = QFileInfo(fileName).absolutePath();
    loadDocumentFile(fileName, type);
}

void MainWindow::on_actionClose_triggered()
{
    if( !saveModifiedFile(currentDocument) ) return;

    if( currentDocument->onCloseDocument() ) {
        updateUi();
        actionStart_Page->trigger();
    }
}

bool MainWindow::on_actionSave_triggered()
{
    if( currentDocument->fileName().isEmpty() )
        return on_actionSave_As_triggered();
    else
        return saveDocumentFile( currentDocument->fileName() );
}

bool MainWindow::on_actionSave_As_triggered()
{
    QString path = currentDocument->fileName();
    if(path.isEmpty())
        path = currentDocument->title();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), path);
    if(fileName.isEmpty()) return false;

    return saveDocumentFile(fileName);
}

void MainWindow::on_actionSet_Printer_Font_triggered()
{
    printFont = QFontDialog::getFont(0, printFont, this);
}

void MainWindow::on_actionPrint_Preview_triggered()
{
#ifndef QT_NO_PRINTER
    QPrinter printer;
    QPrintPreviewDialog printPreviewDialog(&printer, this);
    QObject::connect(&printPreviewDialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(print(QPrinter*)));

    printPreviewDialog.exec();
#endif
}

void MainWindow::on_actionPrint_triggered()
{
#ifndef QT_NO_PRINTER
    QPrinter printer;
    QPrintDialog printDialog(&printer, this);
    if( printDialog.exec() == QDialog::Accepted ) {
        print(&printer);
    }
#endif
}

void MainWindow::onActionOpenRecentFile()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if(action) {
        int i = action->data().toInt();
        if( QFileInfo(recentFiles[i].fileName).isFile() )
            loadDocumentFile(recentFiles[i].fileName, recentFiles[i].fileType);
        else {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setWindowTitle(tr("Failed to Open File"));
            msgBox.setText(tr("The file \"%1\" cannot be opened.").arg(QDir::toNativeSeparators(recentFiles[i].fileName)));
            msgBox.setInformativeText(tr("Do you want to remove the reference to it from <i>Recent Files</i> list?"));
            msgBox.addButton(QMessageBox::Yes);
            msgBox.addButton(QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);
            if( msgBox.exec() == QMessageBox::Yes )
                removeFromRecentFiles(recentFiles[i].fileName, recentFiles[i].fileType);
        }
    }
}

void MainWindow::on_actionClear_History_triggered()
{
    recentFiles.clear();
    updateRecentFileActions();
}

void MainWindow::on_menuEdit_aboutToShow()
{
    QComboBox* aComboBox;
    QLineEdit* focusLineEdit;
    QTextEdit* focusTextEdit;
    bool result;

    focusLineEdit = qobject_cast<QLineEdit*>( QApplication::focusWidget() );
    if(focusLineEdit) {
        goto UpdateEditMenuforLineEdit;
    }

    aComboBox = qobject_cast<QComboBox*>( QApplication::focusWidget() );
    if( aComboBox && aComboBox->isEditable() ) {
        focusLineEdit = aComboBox->lineEdit();
        goto UpdateEditMenuforLineEdit;
    }

    focusTextEdit = qobject_cast<QTextEdit*>( QApplication::focusWidget() );
    if(focusTextEdit) {
        goto UpdateEditMenuforTextEdit;
    }

    actionUndo->setEnabled(false);
    actionRedo->setEnabled(false);
    actionCut->setEnabled(false);
    actionCopy->setEnabled(false);
    actionPaste->setEnabled(false);
    actionDelete->setEnabled(false);
    actionSelect_All->setEnabled(false);

    return;


UpdateEditMenuforLineEdit:

    result = focusLineEdit->hasSelectedText();

    actionUndo->setEnabled( focusLineEdit->isUndoAvailable() );
    actionRedo->setEnabled( focusLineEdit->isRedoAvailable() );
    actionCut->setEnabled(result);
    actionCopy->setEnabled(result);
    actionPaste->setEnabled( !QApplication::clipboard()->text().isEmpty() );
    actionDelete->setEnabled(result);
    actionSelect_All->setEnabled( !focusLineEdit->text().isEmpty() );

    focusEdit = focusLineEdit;

    return;


UpdateEditMenuforTextEdit:

    actionUndo->setEnabled(false);
    actionRedo->setEnabled(false);
    actionCut->setEnabled(false);
    actionCopy->setEnabled( !focusTextEdit->textCursor().selectedText().isEmpty() );
    actionPaste->setEnabled(false);
    actionDelete->setEnabled(false);
    actionSelect_All->setEnabled( !focusTextEdit->toPlainText().isEmpty() );

    focusEdit = focusTextEdit;

    return;
}

void MainWindow::on_actionUndo_triggered()
{
    QLineEdit* focusLineEdit = qobject_cast<QLineEdit*>(focusEdit);
    if(focusLineEdit)
        focusLineEdit->undo();
}

void MainWindow::on_actionRedo_triggered()
{
    QLineEdit* focusLineEdit = qobject_cast<QLineEdit*>(focusEdit);
    if(focusLineEdit)
        focusLineEdit->redo();
}

void MainWindow::on_actionCut_triggered()
{
    QLineEdit* focusLineEdit = qobject_cast<QLineEdit*>(focusEdit);
    if(focusLineEdit)
        focusLineEdit->cut();
}

void MainWindow::on_actionCopy_triggered()
{
    QLineEdit* focusLineEdit = qobject_cast<QLineEdit*>(focusEdit);
    if(focusLineEdit) {
        focusLineEdit->copy();
        return;
    }

    QTextEdit* focusTextEdit = qobject_cast<QTextEdit*>(focusEdit);
    if(focusTextEdit) {
        focusTextEdit->copy();
        return;
    }
}

void MainWindow::on_actionPaste_triggered()
{
    QLineEdit* focusLineEdit = qobject_cast<QLineEdit*>(focusEdit);
    if(focusLineEdit)
        focusLineEdit->paste();
}

void MainWindow::on_actionDelete_triggered()
{
    QLineEdit* focusLineEdit = qobject_cast<QLineEdit*>(focusEdit);
    if(focusLineEdit)
        focusLineEdit->del();
}

void MainWindow::on_actionSelect_All_triggered()
{
    QLineEdit* focusLineEdit = qobject_cast<QLineEdit*>(focusEdit);
    if(focusLineEdit) {
        focusLineEdit->selectAll();
        return;
    }

    QTextEdit* focusTextEdit = qobject_cast<QTextEdit*>(focusEdit);
    if(focusTextEdit) {
        focusTextEdit->selectAll();
        return;
    }
}

void MainWindow::on_menuView_aboutToShow()
{
    actionText_View->setEnabled( currentDocument && !currentDocument->title().isEmpty() );
    actionStatus_Bar->setChecked( QMainWindow::statusBar()->isVisible() );
}

void MainWindow::on_actionStart_Page_triggered()
{
    stackedWidget->setCurrentIndex(StartPage);
}

void MainWindow::on_actionText_View_toggled(bool checked)
{
    if(checked) {
        stackedWidget_2->setCurrentIndex(1);
        currentDocument->refreshFileContent();
        textEdit->setCurrentFont(outputFont);
        textEdit->setText( currentDocument->fileContent() );
    }
    else
        stackedWidget_2->setCurrentIndex(0);

    updateUiTitle();
}

void MainWindow::on_actionMCMCTree_triggered()
{
    stackedWidget->setCurrentIndex(MCMCTree);
}

void MainWindow::on_actionBaseml_triggered()
{
    stackedWidget->setCurrentIndex(BaseML);
}

void MainWindow::on_actionCodeml_triggered()
{
    stackedWidget->setCurrentIndex(CodeML);
}

void MainWindow::on_actionPAMP_triggered()
{
    stackedWidget->setCurrentIndex(PAMP);
}

void MainWindow::on_actionYN00_triggered()
{
    stackedWidget->setCurrentIndex(YN00);
}

void MainWindow::on_menuTools_aboutToShow()
{
    actionChi2->setChecked( chi2Dialog && chi2Dialog->isVisible() );
}

void MainWindow::on_actionEvolver_triggered()
{
    stackedWidget->setCurrentIndex(Evolver);
}

void MainWindow::on_actionChi2_triggered()
{
    if(!chi2Dialog)
        chi2Dialog = new Chi2Dialog(absolutePAMLPath);

    bool flag = chi2Dialog->isVisible();

    chi2Dialog->show();

    if( chi2Dialog->isMinimized() )
        chi2Dialog->showNormal();
    if( !chi2Dialog->isActiveWindow() )
        chi2Dialog->activateWindow();

    if( !chi2Dialog->isStatusOk() && !flag )
        showWarningMessageWhenPathFailed( chi2Dialog->path(), true );
}

void MainWindow::on_actionConfiguration_triggered()
{
    ConfigurationDialog configurationDialog(this);
    if( configurationDialog.exec() == QDialog::Accepted ) {
        saveSettings();

        updateEnginesPath();
        updateOutputFont();
    }
}

void MainWindow::on_menuWindow_aboutToShow()
{
        outputWindowActions[0]->setEnabled( mcmcTreeOutput );
        outputWindowActions[0]->setChecked( mcmcTreeOutput && mcmcTreeOutput->isVisible() );

        outputWindowActions[1]->setEnabled( basemlOutput );
        outputWindowActions[1]->setChecked( basemlOutput && basemlOutput->isVisible() );

        outputWindowActions[2]->setEnabled( codemlOutput );
        outputWindowActions[2]->setChecked( codemlOutput && codemlOutput->isVisible() );

        outputWindowActions[3]->setEnabled( pampOutput );
        outputWindowActions[3]->setChecked( pampOutput && pampOutput->isVisible() );

        outputWindowActions[4]->setEnabled( yn00Output );
        outputWindowActions[4]->setChecked( yn00Output && yn00Output->isVisible() );

        outputWindowActions[5]->setEnabled( evolverOutput );
        outputWindowActions[5]->setChecked( evolverOutput && evolverOutput->isVisible() );
}

void MainWindow::onActionOutputWindow()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if(action) {
        OutputWindow* output;
        int index = action->data().toInt();

        switch(index)
        {
        case 0:
            output = mcmcTreeOutput;
            break;
        case 1:
            output = basemlOutput;
            break;
        case 2:
            output = codemlOutput;
            break;
        case 3:
            output = pampOutput;
            break;
        case 4:
            output = yn00Output;
            break;
        case 5:
            output = evolverOutput;
            break;
        default:
            return;
        }

        if(!output) return;

        output->show();

        if( output->isMinimized() )
            output->showNormal();
        if( !output->isActiveWindow() )
            output->activateWindow();
    }
}

void MainWindow::on_actionPAML_User_Guide_triggered()
{
    QString path = absolutePAMLPath + "/doc/pamlDOC.pdf";

    if( QFileInfo(path).isFile() ) {
#if defined(Q_OS_WIN32)
        QString program = "explorer.exe";
        QStringList arguments;
        arguments << QDir::toNativeSeparators(path);
        QProcess::startDetached(program, arguments);
#elif defined(Q_OS_MAC)
        QString program = "open";
        QStringList arguments;
        arguments << QDir::toNativeSeparators(path);
        QProcess::startDetached(program, arguments);
#elif defined(Q_OS_LINUX) && defined(Q_WS_X11)
        QString program = "okular";
        QStringList arguments;
        arguments << QDir::toNativeSeparators(path);
        QProcess::startDetached(program, arguments);
#else
#warning Nothing to do on current platform
#endif
    }
    else
        showWarningMessageWhenPathFailed(path, false);
}

void MainWindow::on_actionPAML_FAQ_triggered()
{
    QString path = absolutePAMLPath + "/doc/pamlFAQs.pdf";

    if( QFileInfo(path).isFile() ) {
#if defined(Q_OS_WIN32)
        QString program = "explorer.exe";
        QStringList arguments;
        arguments << QDir::toNativeSeparators(path);
        QProcess::startDetached(program, arguments);
#elif defined(Q_OS_MAC)
        QString program = "open";
        QStringList arguments;
        arguments << QDir::toNativeSeparators(path);
        QProcess::startDetached(program, arguments);
#elif defined(Q_OS_LINUX) && defined(Q_WS_X11)
        QString program = "okular";
        QStringList arguments;
        arguments << QDir::toNativeSeparators(path);
        QProcess::startDetached(program, arguments);
#else
#warning Nothing to do on current platform
#endif
    }
    else
        showWarningMessageWhenPathFailed(path, false);
}

void MainWindow::on_actionPAML_History_triggered()
{
    QString path = absolutePAMLPath + "/doc/pamlHistory.txt";

    if( QFileInfo(path).isFile() ) {
        PAMLHistoryDialog pamlHistoryDialog(path, outputFont, this);
        pamlHistoryDialog.exec();
    }
    else
        showWarningMessageWhenPathFailed(path, false);
}

void MainWindow::on_actionMCMCTree_Tutorial_triggered()
{
    QString path = absolutePAMLPath + "/doc/MCMCtree.Tutorials.pdf";

    if( QFileInfo(path).isFile() ) {
#if defined(Q_OS_WIN32)
        QString program = "explorer.exe";
        QStringList arguments;
        arguments << QDir::toNativeSeparators(path);
        QProcess::startDetached(program, arguments);
#elif defined(Q_OS_MAC)
        QString program = "open";
        QStringList arguments;
        arguments << QDir::toNativeSeparators(path);
        QProcess::startDetached(program, arguments);
#elif defined(Q_OS_LINUX) && defined(Q_WS_X11)
        QString program = "okular";
        QStringList arguments;
        arguments << QDir::toNativeSeparators(path);
        QProcess::startDetached(program, arguments);
#else
#warning Nothing to do on current platform
#endif
    }
    else
        showWarningMessageWhenPathFailed(path, false);
}

void MainWindow::on_actionMCMCTree_Manual_triggered()
{
    QString path = absolutePAMLPath + "/doc/MCMCtreeDOC.pdf";

    if( QFileInfo(path).isFile() ) {
#if defined(Q_OS_WIN32)
        QString program = "explorer.exe";
        QStringList arguments;
        arguments << QDir::toNativeSeparators(path);
        QProcess::startDetached(program, arguments);
#elif defined(Q_OS_MAC)
        QString program = "open";
        QStringList arguments;
        arguments << QDir::toNativeSeparators(path);
        QProcess::startDetached(program, arguments);
#elif defined(Q_OS_LINUX) && defined(Q_WS_X11)
        QString program = "okular";
        QStringList arguments;
        arguments << QDir::toNativeSeparators(path);
        QProcess::startDetached(program, arguments);
#else
#warning Nothing to do on current platform
#endif
    }
    else
        showWarningMessageWhenPathFailed(path, false);
}

void MainWindow::on_actionHow_to_Cite_the_Program_triggered()
{
    QString citeString_1 = "Xu B, Yang Z. 2013. PAMLX: a graphical user interface for PAML. Mol Biol Evol. 30:2723-2724.";
    QString citeString_2 = "Yang Z. 2007. PAML 4: Phylogenetic analysis by maximum likelihood. Mol Biol Evol. 24:1586-1591.";

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::NoIcon);
    msgBox.setWindowIcon(QIcon(":/pamlxlogo.png"));
    msgBox.setWindowTitle(tr("How to Cite the Program"));
    msgBox.setText(tr("<p><b><font size=\"3\">You&nbsp;can&nbsp;cite&nbsp;the&nbsp;program&nbsp;as&nbsp;follows&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font></b></p>"));
    msgBox.setInformativeText("<p><font face=\"Courier New\">" + citeString_1 + "</font></p>" + "<p><font face=\"Courier New\">" + citeString_2 + "</font></p>");
    QPushButton* copyToClipboardButton = msgBox.addButton(tr("Copy to Clipboard"), QMessageBox::AcceptRole);
    msgBox.addButton(QMessageBox::Cancel);
    msgBox.setDefaultButton(copyToClipboardButton);
    msgBox.exec();

    if( msgBox.clickedButton() == copyToClipboardButton ) {
        QApplication::clipboard()->setText(citeString_1 + "\n" + citeString_2);
    }
}

void MainWindow::on_actionAbout_PAML_X_triggered()
{
    QString appName = QApplication::applicationName();

    QMessageBox::about(this, tr("About %1").arg(appName),
                       tr("<p><b><font size=\"6\" face=\"Times\">%1</font></b>&nbsp;&nbsp;&nbsp;version %2</p>"
                          "<hr/>"
                          "<p>Based on Qt %3</p>"
                          "<p>%1&nbsp;is&nbsp;a&nbsp;graphical&nbsp;user&nbsp;interface&nbsp;for&nbsp;paml&nbsp;package.</p>"
                          "<p>Copyright&nbsp;&copy;&nbsp;2012&nbsp;<b><font face=\"Times\">Bo&nbsp;Xu&nbsp;</font></b>&lt;<a href=\"mailto:xuxbob@gmail.com\">xuxbob@gmail.com</a>&gt;</p>").arg(appName).arg(QApplication::applicationVersion()).arg(QT_VERSION_STR)
                       );
}

void MainWindow::on_actionAbout_PAML_triggered()
{
    QMessageBox::about(this, tr("About PAML"),
                       tr("<p><b><font size=\"6\" face=\"Times\">Phylogenetic&nbsp;Analysis&nbsp;by&nbsp;Maximum&nbsp;Likelihood</font></b></p>"
                          "<hr/>"
                          "<p>PAML is a package of programs for phylogenetic analyses of DNA or protein sequences using maximum likelihood.</p>"
                          "<p>It is maintained and distributed for academic use free of charge by <b><font face=\"Times\">Ziheng&nbsp;Yang&nbsp;</font></b>&lt;<a href=\"mailto:z.yang@ucl.ac.uk\">z.yang@ucl.ac.uk</a>&gt;.</p>")
                        );
}


//----- mcmctree -----

void MainWindow::on_pushButton_clicked()
{
    pushButton->setDisabled(true);

    if(!mcmcTreeOutput) {
        mcmcTreeOutput = new OutputWindow;
        mcmcTreeOutput->setWindowTitle(tr("%1 Output").arg(strList[MCMCTree]));
        mcmcTreeOutput->textEdit->setCurrentFont(outputFont);
        mcmcTreeEngine->setOutputWindow(mcmcTreeOutput->textEdit);
        QObject::connect(mcmcTreeOutput, SIGNAL(closed()), mcmcTreeEngine, SLOT(terminate()));
        QObject::connect(mcmcTreeOutput, SIGNAL(deleted()), mcmcTreeEngine, SLOT(removeOutputWindow()));
    }

    if( !mcmcTreeEngine->run() ) {
        pushButton->setEnabled(true);
    }
    else {
        mcmcTreeOutput->show();

        if( mcmcTreeOutput->isMinimized() )
            mcmcTreeOutput->showNormal();
        if( !mcmcTreeOutput->isActiveWindow() )
            mcmcTreeOutput->activateWindow();
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    mcmcTreeEngine->terminate();
}

void MainWindow::on_toolButton_clicked()
{
    QString path = QDir::cleanPath( comboBox_20->lineEdit()->text() );

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QDir::cleanPath( QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path) );

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open seqfile"), path);
    if(fileName.isEmpty()) return;

    comboBox_20->lineEdit()->setText( QDir::toNativeSeparators(fileName) );
}

void MainWindow::on_toolButton_2_clicked()
{
    QString path = QDir::cleanPath( comboBox_21->lineEdit()->text() );

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QDir::cleanPath( QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path) );

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open treefile"), path);
    if(fileName.isEmpty()) return;

    comboBox_21->lineEdit()->setText( QDir::toNativeSeparators(fileName) );
}

void MainWindow::on_toolButton_4_clicked()
{
    QString path = QDir::cleanPath( comboBox_22->lineEdit()->text() );

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QDir::cleanPath( QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path) );

    QString directoryName = QFileDialog::getExistingDirectory(this, tr("Save output files in ..."), path);
    if(directoryName.isEmpty()) return;

    comboBox_22->lineEdit()->setText( QDir::toNativeSeparators(directoryName) );
}

void MainWindow::on_checkBox_toggled(bool checked)
{
    label_15->setEnabled(!checked);
    label_16->setEnabled(!checked);
    label_18->setEnabled(!checked);
    label_19->setEnabled(!checked);
    label_20->setEnabled(!checked);
    doubleSpinBox_11->setEnabled(!checked);
    doubleSpinBox_12->setEnabled(!checked);
    doubleSpinBox_14->setEnabled(!checked);
    doubleSpinBox_15->setEnabled(!checked);
    doubleSpinBox_16->setEnabled(!checked);
}

void MainWindow::on_lineEdit_5_textEdited(const QString& text)
{
    mcmcTreeEngine->nEdit_seed = text.toInt();
    setModifiedFlag();
}

void MainWindow::on_comboBox_20_editTextChanged(const QString& text)
{
    if( mcmcTreeEngine->strEdit_seqfile != text ) {
        mcmcTreeEngine->strEdit_seqfile = text;
        setModifiedFlag();
    }

    QString path = text;

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path);

    if( !QDir().exists(path) )
        comboBox_20->setStyleSheet("color: red");
    else
        comboBox_20->setStyleSheet("");
}

void MainWindow::on_comboBox_21_editTextChanged(const QString& text)
{
    if( mcmcTreeEngine->strEdit_treefile != text ) {
        mcmcTreeEngine->strEdit_treefile = text;
        setModifiedFlag();
    }

    QString path = text;

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path);

    if( !QDir().exists(path) )
        comboBox_21->setStyleSheet("color: red");
    else
        comboBox_21->setStyleSheet("");
}

void MainWindow::on_lineEdit_3_textEdited(const QString& text)
{
    mcmcTreeEngine->strEdit_outfile_Name = text;
    setModifiedFlag();
}

void MainWindow::on_comboBox_22_editTextChanged(const QString& text)
{
    if( mcmcTreeEngine->strEdit_outfile_Location != text ) {
        mcmcTreeEngine->strEdit_outfile_Location = text;
        setModifiedFlag();
    }

    QString path = text;

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path);

    if( !QDir(path).exists() )
        comboBox_22->setStyleSheet("color: red");
    else
        comboBox_22->setStyleSheet("");
}

void MainWindow::on_comboBox_activated(int index)
{
    if( mcmcTreeEngine->nComb_usedata != index ) {
        mcmcTreeEngine->nComb_usedata = index;
        setModifiedFlag();
    }
}

void MainWindow::on_spinBox_7_valueChanged(int i)
{
    if( mcmcTreeEngine->nEdit_ndata != i ) {
        mcmcTreeEngine->nEdit_ndata = i;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_2_activated(int index)
{
    if( mcmcTreeEngine->nComb_model != index ) {
        mcmcTreeEngine->nComb_model = index;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_3_activated(int index)
{
    if( mcmcTreeEngine->nComb_clock != index + 1 ) {
        mcmcTreeEngine->nComb_clock = index + 1;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_19_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_TipDate - d) >= precision ) {
        mcmcTreeEngine->nEdit_TipDate = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_20_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_TipDate_TimeUnit - d) >= precision ) {
        mcmcTreeEngine->nEdit_TipDate_TimeUnit = d;
        setModifiedFlag();
    }
}

void MainWindow::on_lineEdit_6_textEdited(const QString& text)
{
    mcmcTreeEngine->strEdit_RootAge = text;
    setModifiedFlag();
}

void MainWindow::on_doubleSpinBox_17_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_alpha - d) >= precision ) {
        mcmcTreeEngine->nEdit_alpha = d;
        setModifiedFlag();
    }
}

void MainWindow::on_spinBox_2_valueChanged(int i)
{
    if( mcmcTreeEngine->nEdit_ncatG != i ) {
        mcmcTreeEngine->nEdit_ncatG = i;
        setModifiedFlag();
    }
}

void MainWindow::on_checkBox_2_stateChanged(int state)
{
    if( mcmcTreeEngine->bButt_cleandata != bool(state) ) {
        mcmcTreeEngine->bButt_cleandata = state;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_7_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_BDparas[0] - d) >= precision ) {
        mcmcTreeEngine->nEdit_BDparas[0] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_8_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_BDparas[1] - d) >= precision ) {
        mcmcTreeEngine->nEdit_BDparas[1] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_13_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_BDparas[2] - d) >= precision ) {
        mcmcTreeEngine->nEdit_BDparas[2] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_18_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_BDparas[3] - d) >= precision ) {
        mcmcTreeEngine->nEdit_BDparas[3] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_kappa_gamma[0] - d) >= precision ) {
        mcmcTreeEngine->nEdit_kappa_gamma[0] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_2_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_kappa_gamma[1] - d) >= precision ) {
        mcmcTreeEngine->nEdit_kappa_gamma[1] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_3_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_alpha_gamma[0] - d) >= precision ) {
        mcmcTreeEngine->nEdit_alpha_gamma[0] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_4_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_alpha_gamma[1] - d) >= precision ) {
        mcmcTreeEngine->nEdit_alpha_gamma[1] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_50_activated(int index)
{
    if( mcmcTreeEngine->nComb_rgeneprior != index ) {
        mcmcTreeEngine->nComb_rgeneprior = index;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_5_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_rgene_para[0] - d) >= precision ) {
        mcmcTreeEngine->nEdit_rgene_para[0] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_6_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_rgene_para[1] - d) >= precision ) {
        mcmcTreeEngine->nEdit_rgene_para[1] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_27_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_rgene_para[2] - d) >= precision ) {
        mcmcTreeEngine->nEdit_rgene_para[2] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_9_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_sigma2_para[0] - d) >= precision ) {
        mcmcTreeEngine->nEdit_sigma2_para[0] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_10_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_sigma2_para[1] - d) >= precision ) {
        mcmcTreeEngine->nEdit_sigma2_para[1] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_28_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_sigma2_para[2] - d) >= precision ) {
        mcmcTreeEngine->nEdit_sigma2_para[2] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_checkBox_3_stateChanged(int state)
{
    if( mcmcTreeEngine->bButt_print != bool(state) ) {
        mcmcTreeEngine->bButt_print = state;
        setModifiedFlag();
    }
}

void MainWindow::on_spinBox_3_valueChanged(int i)
{
    if( mcmcTreeEngine->nEdit_burnin != i ) {
        mcmcTreeEngine->nEdit_burnin = i;
        setModifiedFlag();
    }
}

void MainWindow::on_spinBox_4_valueChanged(int i)
{
    if( mcmcTreeEngine->nEdit_sampfreq != i ) {
        mcmcTreeEngine->nEdit_sampfreq = i;
        setModifiedFlag();
    }
}

void MainWindow::on_spinBox_5_valueChanged(int i)
{
    if( mcmcTreeEngine->nEdit_nsample != i ) {
        mcmcTreeEngine->nEdit_nsample = i;
        setModifiedFlag();
    }
}

void MainWindow::on_checkBox_stateChanged(int state)
{
    if( mcmcTreeEngine->bButt_autoAdjustFinetune != bool(state) ) {
        mcmcTreeEngine->bButt_autoAdjustFinetune = state;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_11_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_finetune[0] - d) >= precision ) {
        mcmcTreeEngine->nEdit_finetune[0] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_12_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_finetune[1] - d) >= precision ) {
        mcmcTreeEngine->nEdit_finetune[1] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_14_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_finetune[2] - d) >= precision ) {
        mcmcTreeEngine->nEdit_finetune[2] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_15_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_finetune[3] - d) >= precision ) {
        mcmcTreeEngine->nEdit_finetune[3] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_16_valueChanged(double d)
{
    if( ::qAbs(mcmcTreeEngine->nEdit_finetune[4] - d) >= precision ) {
        mcmcTreeEngine->nEdit_finetune[4] = d;
        setModifiedFlag();
    }
}


//----- baseml -----

void MainWindow::on_pushButton_5_clicked()
{
    pushButton_5->setDisabled(true);

    if(!basemlOutput) {
        basemlOutput = new OutputWindow;
        basemlOutput->setWindowTitle(tr("%1 Output").arg(strList[BaseML]));
        basemlOutput->textEdit->setCurrentFont(outputFont);
        basemlEngine->setOutputWindow(basemlOutput->textEdit);
        QObject::connect(basemlOutput, SIGNAL(closed()), basemlEngine, SLOT(terminate()));
        QObject::connect(basemlOutput, SIGNAL(deleted()), basemlEngine, SLOT(removeOutputWindow()));
    }

    if( !basemlEngine->run() ) {
        pushButton_5->setEnabled(true);
    }
    else {
        basemlOutput->show();

        if( basemlOutput->isMinimized() )
            basemlOutput->showNormal();
        if( !basemlOutput->isActiveWindow() )
            basemlOutput->activateWindow();
    }
}

void MainWindow::on_pushButton_6_clicked()
{
    basemlEngine->terminate();
}

void MainWindow::on_toolButton_3_clicked()
{
    QString path = QDir::cleanPath( comboBox_17->lineEdit()->text() );

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QDir::cleanPath( QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path) );

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open seqfile"), path);
    if(fileName.isEmpty()) return;

    comboBox_17->lineEdit()->setText( QDir::toNativeSeparators(fileName) );
}

void MainWindow::on_toolButton_5_clicked()
{
    QString path = QDir::cleanPath( comboBox_18->lineEdit()->text() );

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QDir::cleanPath( QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path) );

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open treefile"), path);
    if(fileName.isEmpty()) return;

    comboBox_18->lineEdit()->setText( QDir::toNativeSeparators(fileName) );
}

void MainWindow::on_toolButton_6_clicked()
{
    QString path = QDir::cleanPath( comboBox_19->lineEdit()->text() );

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QDir::cleanPath( QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path) );

    QString directoryName = QFileDialog::getExistingDirectory(this, tr("Save output files in ..."), path);
    if(directoryName.isEmpty()) return;

    comboBox_19->lineEdit()->setText( QDir::toNativeSeparators(directoryName) );
}

void MainWindow::on_comboBox_17_editTextChanged(const QString& text)
{
    if( basemlEngine->strEdit_seqfile != text ) {
        basemlEngine->strEdit_seqfile = text;
        setModifiedFlag();
    }

    QString path = text;

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path);

    if( !QDir().exists(path) )
        comboBox_17->setStyleSheet("color: red");
    else
        comboBox_17->setStyleSheet("");
}

void MainWindow::on_comboBox_18_editTextChanged(const QString& text)
{
    if( basemlEngine->strEdit_treefile != text ) {
        basemlEngine->strEdit_treefile = text;
        setModifiedFlag();
    }

    QString path = text;

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path);

    if( !QDir().exists(path) )
        comboBox_18->setStyleSheet("color: red");
    else
        comboBox_18->setStyleSheet("");
}

void MainWindow::on_lineEdit_9_textEdited(const QString& text)
{
    basemlEngine->strEdit_outfile_Name = text;
    setModifiedFlag();
}

void MainWindow::on_comboBox_19_editTextChanged(const QString& text)
{
    if( basemlEngine->strEdit_outfile_Location != text ) {
        basemlEngine->strEdit_outfile_Location = text;
        setModifiedFlag();
    }

    QString path = text;

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path);

    if( !QDir(path).exists() )
        comboBox_19->setStyleSheet("color: red");
    else
        comboBox_19->setStyleSheet("");
}

void MainWindow::on_checkBox_5_stateChanged(int state)
{
    if( basemlEngine->bButt_cleandata != bool(state) ) {
        basemlEngine->bButt_cleandata = state;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_16_activated(int index)
{
    if( basemlEngine->nComb_runmode != index ) {
        basemlEngine->nComb_runmode = index;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_15_activated(int index)
{
    if( basemlEngine->nComb_method != index ) {
        basemlEngine->nComb_method = index;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_6_activated(int index)
{
    if( basemlEngine->nComb_clock != index ) {
        basemlEngine->nComb_clock = index;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_21_valueChanged(double d)
{
    if( ::qAbs(basemlEngine->nEdit_TipDate - d) >= precision ) {
        basemlEngine->nEdit_TipDate = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_22_valueChanged(double d)
{
    if( ::qAbs(basemlEngine->nEdit_TipDate_TimeUnit - d) >= precision ) {
        basemlEngine->nEdit_TipDate_TimeUnit = d;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_4_activated(int index)
{
    if( basemlEngine->nComb_Mgene != index ) {
        basemlEngine->nComb_Mgene = index;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_14_activated(int index)
{
    if( basemlEngine->nComb_nhomo != index ) {
        basemlEngine->nComb_nhomo = index;
        setModifiedFlag();
    }
}

void MainWindow::on_checkBox_9_stateChanged(int state)
{
    if( basemlEngine->bButt_getSE != bool(state) ) {
        basemlEngine->bButt_getSE = state;
        setModifiedFlag();
    }
}

void MainWindow::on_checkBox_10_stateChanged(int state)
{
    if( basemlEngine->bButt_RateAncestor != bool(state) ) {
        basemlEngine->bButt_RateAncestor = state;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_5_activated(int index)
{
    if( basemlEngine->nComb_model != index ) {
        basemlEngine->nComb_model = index;
        setModifiedFlag();
    }
}

void MainWindow::on_checkBox_7_stateChanged(int state)
{
    if( basemlEngine->bButt_fix_kappa != bool(state) ) {
        basemlEngine->bButt_fix_kappa = state;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_36_valueChanged(double d)
{
    if( ::qAbs(basemlEngine->nEdit_kappa - d) >= precision ) {
        basemlEngine->nEdit_kappa = d;
        setModifiedFlag();
    }
}

void MainWindow::on_checkBox_8_stateChanged(int state)
{
    if( basemlEngine->bButt_fix_alpha != bool(state) ) {
        basemlEngine->bButt_fix_alpha = state;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_23_valueChanged(double d)
{
    if( ::qAbs(d) < precision ) d = 0;

    if( ::qAbs(basemlEngine->nEdit_alpha - d) >= precision ) {
        basemlEngine->nEdit_alpha = d;
        setModifiedFlag();
    }
}

void MainWindow::on_checkBox_6_stateChanged(int state)
{
    if( basemlEngine->bButt_Malpha != bool(state) ) {
        basemlEngine->bButt_Malpha = state;
        setModifiedFlag();
    }
}

void MainWindow::on_spinBox_6_valueChanged(int i)
{
    if( basemlEngine->nEdit_ncatG != i ) {
        basemlEngine->nEdit_ncatG = i;
        setModifiedFlag();
    }
}

void MainWindow::on_checkBox_13_stateChanged(int state)
{
    if( basemlEngine->bButt_fix_rho != bool(state) ) {
        basemlEngine->bButt_fix_rho = state;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_24_valueChanged(double d)
{
    if( ::qAbs(d) < precision ) d = 0;

    if( ::qAbs(basemlEngine->nEdit_rho - d) >= precision ) {
        basemlEngine->nEdit_rho = d;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_13_activated(int index)
{
    if( basemlEngine->nComb_nparK != index ) {
        basemlEngine->nComb_nparK = index;
        setModifiedFlag();
    }
}

void MainWindow::on_spinBox_8_valueChanged(int i)
{
    if( basemlEngine->nEdit_ndata != i ) {
        basemlEngine->nEdit_ndata = i;
        setModifiedFlag();
    }
}

void MainWindow::on_lineEdit_12_textEdited(const QString& text)
{
    basemlEngine->strEdit_Small_Diff = text;
    setModifiedFlag();
}

void MainWindow::on_comboBox_26_activated(int index)
{
    if( basemlEngine->nComb_icode != index - 1 ) {
        basemlEngine->nComb_icode = index - 1;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_8_activated(int index)
{
    if( basemlEngine->nComb_fix_blength != index - 1 ) {
        basemlEngine->nComb_fix_blength = index - 1;
        setModifiedFlag();
    }
}


//----- codeml -----

void MainWindow::on_pushButton_7_clicked()
{
    pushButton_7->setDisabled(true);

    if(!codemlOutput) {
        codemlOutput = new OutputWindow;
        codemlOutput->setWindowTitle(tr("%1 Output").arg(strList[CodeML]));
        codemlOutput->textEdit->setCurrentFont(outputFont);
        codemlEngine->setOutputWindow(codemlOutput->textEdit);
        QObject::connect(codemlOutput, SIGNAL(closed()), codemlEngine, SLOT(terminate()));
        QObject::connect(codemlOutput, SIGNAL(deleted()), codemlEngine, SLOT(removeOutputWindow()));
    }

    if( !codemlEngine->run() ) {
        pushButton_7->setEnabled(true);
    }
    else {
        codemlOutput->show();

        if( codemlOutput->isMinimized() )
            codemlOutput->showNormal();
        if( !codemlOutput->isActiveWindow() )
            codemlOutput->activateWindow();
    }
}

void MainWindow::on_pushButton_10_clicked()
{
    codemlEngine->terminate();
}

void MainWindow::on_toolButton_11_clicked()
{
    QString path = QDir::cleanPath( comboBox_23->lineEdit()->text() );

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QDir::cleanPath( QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path) );

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open seqfile"), path);
    if(fileName.isEmpty()) return;

    comboBox_23->lineEdit()->setText( QDir::toNativeSeparators(fileName) );
}

void MainWindow::on_toolButton_12_clicked()
{
    QString path = QDir::cleanPath( comboBox_24->lineEdit()->text() );

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QDir::cleanPath( QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path) );

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open treefile"), path);
    if(fileName.isEmpty()) return;

    comboBox_24->lineEdit()->setText( QDir::toNativeSeparators(fileName) );
}

void MainWindow::on_toolButton_13_clicked()
{
    QString path = QDir::cleanPath( comboBox_25->lineEdit()->text() );

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QDir::cleanPath( QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path) );

    QString directoryName = QFileDialog::getExistingDirectory(this, tr("Save output files in ..."), path);
    if(directoryName.isEmpty()) return;

    comboBox_25->lineEdit()->setText( QDir::toNativeSeparators(directoryName) );
}

void MainWindow::on_toolButton_17_clicked()
{
    QString path = QDir::cleanPath( comboBox_45->lineEdit()->text() );

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QDir::cleanPath( QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path) );

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open amino acid rate file"), path);
    if(fileName.isEmpty()) return;

    comboBox_45->lineEdit()->setText( QDir::toNativeSeparators(fileName) );
}

void MainWindow::on_radioButton_toggled(bool checked)
{
    if(!checked) comboBox_37->setCurrentIndex(-1);
}

void MainWindow::on_comboBox_37_currentIndexChanged(int index)
{
    bool ok;
    int currentItemData = comboBox_39->itemData( comboBox_39->currentIndex() ).toInt(&ok);
    int num = comboBox_39->count();
    for(int i=0; i<num; i++)
        comboBox_39->setItemData(i, QVariant::Invalid);
    for(int i=num-1; i>0; i--)
        comboBox_39->removeItem(i);

    QObject::disconnect(listWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onListWidgetItemExclusive(QListWidgetItem*)));
    Qt::ItemFlags normalFlags = listWidget->item(0)->flags();
    bool ignored = true;
    num = listWidget->count();
    for(int i=0; i<num; i++) {
        listWidget->item(i)->setSelected(false);
        listWidget->item(i)->setData(Qt::UserRole, ignored);
        listWidget->item(i)->setFlags(normalFlags & ~Qt::ItemIsEnabled);
        listWidget->item(i)->setData(Qt::UserRole, QVariant::Invalid);
    }

    switch(index)
    {
    case 0:
        comboBox_39->setItemText(0, tr("0: one"));
        comboBox_39->setItemData(0, 0);
        comboBox_39->setCurrentIndex(0);
        on_comboBox_39_currentIndexChanged(0);

        listWidget->item(0)->setCheckState(Qt::Checked);
        for(int i=1; i<num; i++)
            listWidget->item(i)->setCheckState(Qt::Unchecked);
        break;

    case 1:
        comboBox_39->setItemText(0, tr("1: b"));
        comboBox_39->insertItems(1, QStringList()
//         << QApplication::translate("MainWindow", "2: two branch types", 0, QApplication::UnicodeUTF8)
        << QApplication::translate("MainWindow", "2: two branch types", 0)
        );
        comboBox_39->setItemData(0, 1);
        comboBox_39->setItemData(1, 2);
        comboBox_39->setCurrentIndex(1);
        on_comboBox_39_currentIndexChanged(1);

        listWidget->item(0)->setCheckState(Qt::Checked);
        for(int i=1; i<num; i++)
            listWidget->item(i)->setCheckState(Qt::Unchecked);
        break;

    case 2:
        comboBox_39->setItemText(0, tr("0: one"));
        comboBox_39->setItemData(0, 0);
        comboBox_39->setCurrentIndex(0);
        on_comboBox_39_currentIndexChanged(0);

        for(int i=0; i<num; i++)
            if( i == 0 || i == 1 || i == 2 || i == 7 || i == 8 ) {
                listWidget->item(i)->setFlags(normalFlags | Qt::ItemIsEnabled);
                listWidget->item(i)->setCheckState(Qt::Checked);
            }
            else
                listWidget->item(i)->setCheckState(Qt::Unchecked);
        break;

    case 3:
        comboBox_39->setItemText(0, tr("2: two branch types"));
        comboBox_39->setItemData(0, 2);
        comboBox_39->setCurrentIndex(0);
        on_comboBox_39_currentIndexChanged(0);

        listWidget->item(2)->setFlags(normalFlags | Qt::ItemIsEnabled);
        listWidget->item(2)->setCheckState(Qt::Checked);
        listWidget->item(3)->setFlags(normalFlags | Qt::ItemIsEnabled);
        listWidget->item(3)->setCheckState(Qt::Unchecked);
        for(int i=0; i<num; i++)
            if( i != 2 && i != 3 )
                listWidget->item(i)->setCheckState(Qt::Unchecked);
        QObject::connect(listWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onListWidgetItemExclusive(QListWidgetItem*)));
        break;

    case 4:
        comboBox_39->setItemText(0, tr("3:"));
        comboBox_39->setItemData(0, 3);
        comboBox_39->setCurrentIndex(0);
        on_comboBox_39_currentIndexChanged(0);

        listWidget->item(2)->setFlags(normalFlags | Qt::ItemIsEnabled);
        listWidget->item(2)->setCheckState(Qt::Checked);
        listWidget->item(3)->setFlags(normalFlags | Qt::ItemIsEnabled);
        listWidget->item(3)->setCheckState(Qt::Unchecked);
        for(int i=0; i<num; i++)
            if( i != 2 && i != 3 )
                listWidget->item(i)->setCheckState(Qt::Unchecked);
        QObject::connect(listWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onListWidgetItemExclusive(QListWidgetItem*)));
        break;

    default:
        comboBox_39->setItemText(0, tr("0: one"));
        comboBox_39->insertItems(1, QStringList()
//         << QApplication::translate("MainWindow", "1: b", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "1: b", 0)
//         << QApplication::translate("MainWindow", "2: 2 or more dN/dS", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "2: 2 or more dN/dS", 0)
//         << QApplication::translate("MainWindow", "3:", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "3:", 0)
        );
        if(!ok) currentItemData = 0;
        comboBox_39->setCurrentIndex(currentItemData);
        comboBox_39->setItemData(0, 0);
        comboBox_39->setItemData(1, 1);
        comboBox_39->setItemData(2, 2);
        comboBox_39->setItemData(3, 3);

        for(int i=0; i<num; i++) {
            listWidget->item(i)->setData(Qt::UserRole, ignored);
            listWidget->item(i)->setFlags(normalFlags | Qt::ItemIsEnabled);
            listWidget->item(i)->setData(Qt::UserRole, QVariant::Invalid);
        }
    }
}

void MainWindow::on_comboBox_31_currentIndexChanged(int index)
{
    index++;
    bool state1 = !(index == 2 || index == 3);
    bool state2 = !(index == 1);

    radioButton->setEnabled(state1);
    int num = listWidget->count();
    for(int i=0; i<num; i++)
        listWidget->item(i)->setSelected(false);
    radioButton_2->setEnabled(state1);
    radioButton_2->setChecked(state2); //nothing to do when state2 == false

    label_64->setEnabled(state1);
    listWidget->setEnabled(state1);
    label_91->setEnabled(state1);
    comboBox_42->setEnabled(state1);
    label_114->setEnabled(state1);
    comboBox_47->setEnabled(state1);
    label_110->setEnabled(state1);
    comboBox_46->setEnabled(state1);
    checkBox_19->setEnabled(state1);
    doubleSpinBox_41->setEnabled(state1);
    checkBox_26->setEnabled(state1);
    doubleSpinBox_26->setEnabled(state1);

    bool b = comboBox_45->isEnabled();
    if(!b && state2) {
        QString path = comboBox_45->lineEdit()->text();

        if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
            path = QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path);

        if( !QDir().exists(path) )
            comboBox_45->setStyleSheet("color: red");
        else
            comboBox_45->setStyleSheet("");
    }
    else if(b && !state2)
        comboBox_45->setStyleSheet("");

    label_95->setEnabled(state2);
    comboBox_45->setEnabled(state2);
    toolButton_17->setEnabled(state2);

    num = comboBox_39->count();
    if( index == 1 && num != 4 ) {
        if(num == 0) {
            comboBox_39->insertItems(0, QStringList()
//             << QApplication::translate("MainWindow", "0: one", 0, QApplication::UnicodeUTF8)
             << QApplication::translate("MainWindow", "0: one", 0)
//             << QApplication::translate("MainWindow", "1: b", 0, QApplication::UnicodeUTF8)
             << QApplication::translate("MainWindow", "1: b", 0)
//             << QApplication::translate("MainWindow", "2: 2 or more dN/dS", 0, QApplication::UnicodeUTF8)
             << QApplication::translate("MainWindow", "2: 2 or more dN/dS", 0)
//             << QApplication::translate("MainWindow", "3:", 0, QApplication::UnicodeUTF8)
             << QApplication::translate("MainWindow", "3:", 0)
           );
        }
        else {
            comboBox_39->setItemData(7, QVariant::Invalid);
            comboBox_39->setItemData(6, QVariant::Invalid);
            comboBox_39->setItemData(5, QVariant::Invalid);
            comboBox_39->setItemData(4, QVariant::Invalid);
            comboBox_39->setItemData(3, QVariant::Invalid);
            comboBox_39->setItemData(2, QVariant::Invalid);
            comboBox_39->setItemData(1, QVariant::Invalid);
            comboBox_39->setItemData(0, QVariant::Invalid);
            comboBox_39->removeItem(7);
            comboBox_39->removeItem(6);
            comboBox_39->removeItem(5);
            comboBox_39->removeItem(4);
            comboBox_39->setItemText(3, tr("3:"));
            comboBox_39->setItemText(2, tr("2: 2 or more dN/dS"));
            comboBox_39->setItemText(1, tr("1: b"));
            comboBox_39->setItemText(0, tr("0: one"));
         }
        comboBox_39->setItemData(0, 0);
        comboBox_39->setItemData(1, 1);
        comboBox_39->setItemData(2, 2);
        comboBox_39->setItemData(3, 3);
    }
    if( (index == 2 || index == 3) && num != 8 ) {
        if(num == 0) {
            comboBox_39->insertItems(0, QStringList()
//             << QApplication::translate("MainWindow", "0: poisson", 0, QApplication::UnicodeUTF8)
             << QApplication::translate("MainWindow", "0: poisson", 0)
//             << QApplication::translate("MainWindow", "1: proportional", 0, QApplication::UnicodeUTF8)
             << QApplication::translate("MainWindow", "1: proportional", 0)
//             << QApplication::translate("MainWindow", "2: Empirical", 0, QApplication::UnicodeUTF8)
             << QApplication::translate("MainWindow", "2: Empirical", 0)
//             << QApplication::translate("MainWindow", "3: Empirical+F", 0, QApplication::UnicodeUTF8)
             << QApplication::translate("MainWindow", "3: Empirical+F", 0)
//             << QApplication::translate("MainWindow", "6: FromCodon", 0, QApplication::UnicodeUTF8)
             << QApplication::translate("MainWindow", "6: FromCodon", 0)
//             << QApplication::translate("MainWindow", "7: AAClasses", 0, QApplication::UnicodeUTF8)
             << QApplication::translate("MainWindow", "7: AAClasses", 0)
//             << QApplication::translate("MainWindow", "8: REVaa_0", 0, QApplication::UnicodeUTF8)
             << QApplication::translate("MainWindow", "8: REVaa_0", 0)
//             << QApplication::translate("MainWindow", "9: REVaa(nr=189)", 0, QApplication::UnicodeUTF8)
             << QApplication::translate("MainWindow", "9: REVaa(nr=189)", 0)
            );
        }
        else {
            comboBox_39->setItemData(0, QVariant::Invalid);
            comboBox_39->setItemData(1, QVariant::Invalid);
            comboBox_39->setItemData(2, QVariant::Invalid);
            comboBox_39->setItemData(3, QVariant::Invalid);
            comboBox_39->setItemText(0, tr("0: poisson"));
            comboBox_39->setItemText(1, tr("1: proportional"));
            comboBox_39->setItemText(2, tr("2: Empirical"));
            comboBox_39->setItemText(3, tr("3: Empirical+F"));
            comboBox_39->insertItems(4, QStringList()
//             << QApplication::translate("MainWindow", "6: FromCodon", 0, QApplication::UnicodeUTF8)
//             << QApplication::translate("MainWindow", "7: AAClasses", 0, QApplication::UnicodeUTF8)
//             << QApplication::translate("MainWindow", "8: REVaa_0", 0, QApplication::UnicodeUTF8)
//             << QApplication::translate("MainWindow", "9: REVaa(nr=189)", 0, QApplication::UnicodeUTF8)
             << QApplication::translate("MainWindow", "6: FromCodon", 0)
             << QApplication::translate("MainWindow", "7: AAClasses", 0)
             << QApplication::translate("MainWindow", "8: REVaa_0", 0)
             << QApplication::translate("MainWindow", "9: REVaa(nr=189)", 0)
            );
        }
        comboBox_39->setItemData(0, 0);
        comboBox_39->setItemData(1, 1);
        comboBox_39->setItemData(2, 2);
        comboBox_39->setItemData(3, 3);
        comboBox_39->setItemData(4, 6);
        comboBox_39->setItemData(5, 7);
        comboBox_39->setItemData(6, 8);
        comboBox_39->setItemData(7, 9);
    }

    num = comboBox_40->count();
    if( index == 1 && num != 5 ) {
        if(num == 0) {
            comboBox_40->insertItems(0, QStringList()
//             << QApplication::translate("MainWindow", "0: rates", 0, QApplication::UnicodeUTF8)
//             << QApplication::translate("MainWindow", "1: separate", 0, QApplication::UnicodeUTF8)
//             << QApplication::translate("MainWindow", "2: diff pi", 0, QApplication::UnicodeUTF8)
//             << QApplication::translate("MainWindow", "3: diff kapa", 0, QApplication::UnicodeUTF8)
//             << QApplication::translate("MainWindow", "4: all diff", 0, QApplication::UnicodeUTF8)
             << QApplication::translate("MainWindow", "0: rates", 0)
             << QApplication::translate("MainWindow", "1: separate", 0)
             << QApplication::translate("MainWindow", "2: diff pi", 0)
             << QApplication::translate("MainWindow", "3: diff kapa", 0)
             << QApplication::translate("MainWindow", "4: all diff", 0)
            );
        }
        else {
            comboBox_40->setItemData(0, QVariant::Invalid);
            comboBox_40->setItemData(1, QVariant::Invalid);
            comboBox_40->insertItems(2, QStringList()
//             << QApplication::translate("MainWindow", "2: diff pi", 0, QApplication::UnicodeUTF8)
//             << QApplication::translate("MainWindow", "3: diff kapa", 0, QApplication::UnicodeUTF8)
//             << QApplication::translate("MainWindow", "4: all diff", 0, QApplication::UnicodeUTF8)
             << QApplication::translate("MainWindow", "2: diff pi", 0)
             << QApplication::translate("MainWindow", "3: diff kapa", 0)
             << QApplication::translate("MainWindow", "4: all diff", 0)
            );
        }
        comboBox_40->setItemData(0, 0);
        comboBox_40->setItemData(1, 1);
        comboBox_40->setItemData(2, 2);
        comboBox_40->setItemData(3, 3);
        comboBox_40->setItemData(4, 4);
    }
    if( (index == 2 || index == 3) && num != 2 ) {
        if(num == 0) {
            comboBox_40->insertItems(0, QStringList()
//             << QApplication::translate("MainWindow", "0: rates", 0, QApplication::UnicodeUTF8)
//             << QApplication::translate("MainWindow", "1: separate", 0, QApplication::UnicodeUTF8)
             << QApplication::translate("MainWindow", "0: rates", 0)
             << QApplication::translate("MainWindow", "1: separate", 0)
            );
        }
        else {
            comboBox_40->setItemData(4, QVariant::Invalid);
            comboBox_40->setItemData(3, QVariant::Invalid);
            comboBox_40->setItemData(2, QVariant::Invalid);
            comboBox_40->setItemData(1, QVariant::Invalid);
            comboBox_40->setItemData(0, QVariant::Invalid);
            comboBox_40->removeItem(4);
            comboBox_40->removeItem(3);
            comboBox_40->removeItem(2);
        }
        comboBox_40->setItemData(0, 0);
        comboBox_40->setItemData(1, 1);
    }

    int i = comboBox_39->currentIndex();
    b = ( state2 || i == -1 || comboBox_39->itemData(i).toInt() != 0 || !bool(listWidget->item(0)->checkState()) );
    label_92->setEnabled(b);
    comboBox_43->setEnabled(b);
}

void MainWindow::onListWidgetItemExclusive(QListWidgetItem* item)
{
    int row = listWidget->row(item);

    if( row != 2 && row != 3 ) return;

    listWidget->item( (row == 3)? 2 : 3 )->setCheckState( (bool(item->checkState()))? Qt::Unchecked : Qt::Checked );
}

void MainWindow::on_comboBox_23_editTextChanged(const QString& text)
{
    if( codemlEngine->strEdit_seqfile != text ) {
        codemlEngine->strEdit_seqfile = text;
        setModifiedFlag();
    }

    QString path = text;

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path);

    if( !QDir().exists(path) )
        comboBox_23->setStyleSheet("color: red");
    else
        comboBox_23->setStyleSheet("");
}

void MainWindow::on_comboBox_24_editTextChanged(const QString& text)
{
    if( codemlEngine->strEdit_treefile != text ) {
        codemlEngine->strEdit_treefile = text;
        setModifiedFlag();
    }

    QString path = text;

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path);

    if( !QDir().exists(path) )
        comboBox_24->setStyleSheet("color: red");
    else
        comboBox_24->setStyleSheet("");
}

void MainWindow::on_lineEdit_10_textEdited(const QString& text)
{
    codemlEngine->strEdit_outfile_Name = text;
    setModifiedFlag();
}

void MainWindow::on_comboBox_25_editTextChanged(const QString& text)
{
    if( codemlEngine->strEdit_outfile_Location != text ) {
        codemlEngine->strEdit_outfile_Location = text;
        setModifiedFlag();
    }

    QString path = text;

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path);

    if( !QDir(path).exists() )
        comboBox_25->setStyleSheet("color: red");
    else
        comboBox_25->setStyleSheet("");
}

void MainWindow::on_comboBox_31_activated(int index)
{
    if( codemlEngine->nComb_seqtype != index + 1 ) {
        codemlEngine->nComb_seqtype = index + 1;
        setModifiedFlag();
    }
}

void MainWindow::on_checkBox_25_stateChanged(int state)
{
    if( codemlEngine->bButt_cleandata != bool(state) ) {
        codemlEngine->bButt_cleandata = state;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_38_currentIndexChanged(int index)
{
    if(index == 6) index = -2;
    else if(index == 7) index = -3;

    groupBox_33->setEnabled( index == -3 );
    groupBox_34->setEnabled( index == -3 );

    if( codemlEngine->nComb_runmode != index ) {
        codemlEngine->nComb_runmode = index;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_33_valueChanged(double d)
{
    if( ::qAbs(codemlEngine->nEdit_t_gamma[0] - d) >= precision ) {
        codemlEngine->nEdit_t_gamma[0] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_34_valueChanged(double d)
{
    if( ::qAbs(codemlEngine->nEdit_t_gamma[1] - d) >= precision ) {
        codemlEngine->nEdit_t_gamma[1] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_38_valueChanged(double d)
{
    if( ::qAbs(codemlEngine->nEdit_w_gamma[0] - d) >= precision ) {
        codemlEngine->nEdit_w_gamma[0] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_42_valueChanged(double d)
{
    if( ::qAbs(codemlEngine->nEdit_w_gamma[1] - d) >= precision ) {
        codemlEngine->nEdit_w_gamma[1] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_44_activated(int index)
{
    if( codemlEngine->nComb_method != index ) {
        codemlEngine->nComb_method = index;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_41_activated(int index)
{
    if( codemlEngine->nComb_clock != index ) {
        codemlEngine->nComb_clock = index;
        setModifiedFlag();
    }
}

void MainWindow::on_checkBox_22_stateChanged(int state)
{
    if( codemlEngine->bButt_getSE != bool(state) ) {
        codemlEngine->bButt_getSE = state;
        setModifiedFlag();
    }
}

void MainWindow::on_checkBox_23_stateChanged(int state)
{
    if( codemlEngine->bButt_RateAncestor != bool(state) ) {
        codemlEngine->bButt_RateAncestor = state;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_42_activated(int index)
{
    if( codemlEngine->nComb_CodonFreq != index ) {
        codemlEngine->nComb_CodonFreq = index;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_47_activated(int index)
{
    if( codemlEngine->nComb_estFreq != index ) {
        codemlEngine->nComb_estFreq = index;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_39_currentIndexChanged(int index)
{
    QVariant variant = comboBox_39->itemData(index);
    if( index != -1 && variant.type() == QVariant::Invalid ) return;

    int i = (index == -1)? -1 : variant.toInt();

    if( comboBox_31->currentIndex() + 1 == 1 ) {
        bool b = ( index == -1 || i != 0 || !bool(listWidget->item(0)->checkState()) );
        label_92->setEnabled(b);
        comboBox_43->setEnabled(b);
    }

    if( codemlEngine->nComb_model != i ) {
        codemlEngine->nComb_model = i;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_43_activated(int index)
{
    if(index == -1) index = -7;
    else if(index > 6 && index < 13) index = 6 - index;
    else if(index == 13) index = 7;
    else if(index > 13) index -= 3;

    if( codemlEngine->nComb_aaDist != index ) {
        codemlEngine->nComb_aaDist = index;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_45_editTextChanged(const QString& text)
{
    if( codemlEngine->strEdit_aaRatefile != text ) {
        codemlEngine->strEdit_aaRatefile = text;
        setModifiedFlag();
    }

    if( comboBox_45->isEnabled() ) {
        QString path = text;

        if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
            path = QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path);

        if( !QDir().exists(path) )
            comboBox_45->setStyleSheet("color: red");
        else
            comboBox_45->setStyleSheet("");
    }
    else
        comboBox_45->setStyleSheet("");
}

void MainWindow::on_listWidget_itemChanged(QListWidgetItem* item)
{
    static bool ignored1 = false;
    bool ignored2 = item->data(Qt::UserRole).toBool();
    if( ignored1 || ignored2 ) {
        ignored1 = ignored2;
        return;
    }

    int row = listWidget->row(item);
    bool state = bool(item->checkState());

    if( comboBox_31->currentIndex() + 1 == 1 && row == 0 ) {
        int index = comboBox_39->currentIndex();
        bool b = ( !state || index == -1 || comboBox_39->itemData(index).toInt() != 0 );
        label_92->setEnabled(b);
        comboBox_43->setEnabled(b);
    }

    int i, j;
    for(i=-1,j=0; j<CodemlEngine::maxNSsitesModels; j++) {
        if( codemlEngine->triList_nsmodels[j] != CodemlEngine::Null ) {
            i++;
            if(i == row) break;
        }
    }

    if( i == row && bool(codemlEngine->triList_nsmodels[j]) != state ) {
        codemlEngine->triList_nsmodels[j] = (state)? CodemlEngine::True : CodemlEngine::False;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_46_activated(int index)
{
    if( codemlEngine->nComb_icode != index ) {
        codemlEngine->nComb_icode = index;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_40_currentIndexChanged(int index)
{
    QVariant variant = comboBox_40->itemData(index);
    if( index != -1 && variant.type() == QVariant::Invalid ) return;

    int i = (index == -1)? -1 : variant.toInt();

    if( codemlEngine->nComb_Mgene != i ) {
        codemlEngine->nComb_Mgene = i;
        setModifiedFlag();
    }
}

void MainWindow::on_checkBox_19_stateChanged(int state)
{
    if( codemlEngine->bButt_fix_kappa != bool(state) ) {
        codemlEngine->bButt_fix_kappa = state;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_41_valueChanged(double d)
{
    if( ::qAbs(codemlEngine->nEdit_kappa - d) >= precision ) {
        codemlEngine->nEdit_kappa = d;
        setModifiedFlag();
    }
}

void MainWindow::on_checkBox_26_stateChanged(int state)
{
    if( codemlEngine->bButt_fix_omega != bool(state) ) {
        codemlEngine->bButt_fix_omega = state;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_26_valueChanged(double d)
{
    if( ::qAbs(codemlEngine->nEdit_omega - d) >= precision ) {
        codemlEngine->nEdit_omega = d;
        setModifiedFlag();
    }
}

void MainWindow::on_checkBox_20_stateChanged(int state)
{
    if( codemlEngine->bButt_fix_alpha != bool(state) ) {
        codemlEngine->bButt_fix_alpha = state;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_25_valueChanged(double d)
{
    if( ::qAbs(d) < precision ) d = 0;

    if( ::qAbs(codemlEngine->nEdit_alpha - d) >= precision ) {
        codemlEngine->nEdit_alpha = d;
        setModifiedFlag();
    }
}

void MainWindow::on_checkBox_21_stateChanged(int state)
{
    if( codemlEngine->bButt_Malpha != bool(state) ) {
        codemlEngine->bButt_Malpha = state;
        setModifiedFlag();
    }
}

void MainWindow::on_spinBox_11_valueChanged(int i)
{
    if( codemlEngine->nEdit_ncatG != i ) {
        codemlEngine->nEdit_ncatG = i;
        setModifiedFlag();
    }
}

void MainWindow::on_spinBox_16_valueChanged(int i)
{
    if( codemlEngine->nEdit_ndata != i ) {
        codemlEngine->nEdit_ndata = i;
        setModifiedFlag();
    }
}

void MainWindow::on_lineEdit_14_textEdited(const QString& text)
{
    codemlEngine->strEdit_Small_Diff = text;
    setModifiedFlag();
}

void MainWindow::on_comboBox_49_activated(int index)
{
    if( codemlEngine->nComb_fix_blength != index - 1 ) {
        codemlEngine->nComb_fix_blength = index - 1;
        setModifiedFlag();
    }
}


//----- pamp -----

void MainWindow::on_pushButton_11_clicked()
{
    pushButton_11->setDisabled(true);

    if(!pampOutput) {
        pampOutput = new OutputWindow;
        pampOutput->setWindowTitle(tr("%1 Output").arg(strList[PAMP]));
        pampOutput->textEdit->setCurrentFont(outputFont);
        pampEngine->setOutputWindow(pampOutput->textEdit);
        QObject::connect(pampOutput, SIGNAL(closed()), pampEngine, SLOT(terminate()));
        QObject::connect(pampOutput, SIGNAL(deleted()), pampEngine, SLOT(removeOutputWindow()));
    }

    if( !pampEngine->run() ) {
        pushButton_11->setEnabled(true);
    }
    else {
        pampOutput->show();

        if( pampOutput->isMinimized() )
            pampOutput->showNormal();
        if( !pampOutput->isActiveWindow() )
            pampOutput->activateWindow();
    }
}

void MainWindow::on_pushButton_12_clicked()
{
    pampEngine->terminate();
}

void MainWindow::on_toolButton_14_clicked()
{
    QString path = QDir::cleanPath( comboBox_27->lineEdit()->text() );

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QDir::cleanPath( QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path) );

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open seqfile"), path);
    if(fileName.isEmpty()) return;

    comboBox_27->lineEdit()->setText( QDir::toNativeSeparators(fileName) );
}

void MainWindow::on_toolButton_15_clicked()
{
    QString path = QDir::cleanPath( comboBox_28->lineEdit()->text() );

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QDir::cleanPath( QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path) );

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open treefile"), path);
    if(fileName.isEmpty()) return;

    comboBox_28->lineEdit()->setText( QDir::toNativeSeparators(fileName) );
}

void MainWindow::on_toolButton_16_clicked()
{
    QString path = QDir::cleanPath( comboBox_29->lineEdit()->text() );

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QDir::cleanPath( QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path) );

    QString directoryName = QFileDialog::getExistingDirectory(this, tr("Save output files in ..."), path);
    if(directoryName.isEmpty()) return;

    comboBox_29->lineEdit()->setText( QDir::toNativeSeparators(directoryName) );
}

void MainWindow::on_comboBox_27_editTextChanged(const QString& text)
{
    if( pampEngine->strEdit_seqfile != text ) {
        pampEngine->strEdit_seqfile = text;
        setModifiedFlag();
    }

    QString path = text;

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path);

    if( !QDir().exists(path) )
        comboBox_27->setStyleSheet("color: red");
    else
        comboBox_27->setStyleSheet("");
}

void MainWindow::on_comboBox_28_editTextChanged(const QString& text)
{
    if( pampEngine->strEdit_treefile != text ) {
        pampEngine->strEdit_treefile = text;
        setModifiedFlag();
    }

    QString path = text;

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path);

    if( !QDir().exists(path) )
        comboBox_28->setStyleSheet("color: red");
    else
        comboBox_28->setStyleSheet("");
}

void MainWindow::on_lineEdit_4_textEdited(const QString& text)
{
    pampEngine->strEdit_outfile_Name = text;
    setModifiedFlag();
}

void MainWindow::on_comboBox_29_editTextChanged(const QString& text)
{
    if( pampEngine->strEdit_outfile_Location != text ) {
        pampEngine->strEdit_outfile_Location = text;
        setModifiedFlag();
    }

    QString path = text;

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path);

    if( !QDir(path).exists() )
        comboBox_29->setStyleSheet("color: red");
    else
        comboBox_29->setStyleSheet("");
}

void MainWindow::on_comboBox_32_activated(int index)
{
    if(index>0) index++;

    if( pampEngine->nComb_seqtype != index ) {
        pampEngine->nComb_seqtype = index;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_30_activated(int index)
{
    if( pampEngine->nComb_nhomo != index ) {
        pampEngine->nComb_nhomo = index;
        setModifiedFlag();
    }
}

void MainWindow::on_spinBox_9_valueChanged(int i)
{
    if( pampEngine->nEdit_ncatG != i ) {
        pampEngine->nEdit_ncatG = i;
        setModifiedFlag();
    }
}


//----- yn00 -----

void MainWindow::on_pushButton_13_clicked()
{
    pushButton_13->setDisabled(true);

    if(!yn00Output) {
        yn00Output = new OutputWindow;
        yn00Output->setWindowTitle(tr("%1 Output").arg(strList[YN00]));
        yn00Output->textEdit->setCurrentFont(outputFont);
        yn00Engine->setOutputWindow(yn00Output->textEdit);
        QObject::connect(yn00Output, SIGNAL(closed()), yn00Engine, SLOT(terminate()));
        QObject::connect(yn00Output, SIGNAL(deleted()), yn00Engine, SLOT(removeOutputWindow()));
    }

    if( !yn00Engine->run() ) {
        pushButton_13->setEnabled(true);
    }
    else {
        yn00Output->show();

        if( yn00Output->isMinimized() )
            yn00Output->showNormal();
        if( !yn00Output->isActiveWindow() )
            yn00Output->activateWindow();
    }
}

void MainWindow::on_pushButton_14_clicked()
{
    yn00Engine->terminate();
}

void MainWindow::on_toolButton_18_clicked()
{
    QString path = QDir::cleanPath( comboBox_33->lineEdit()->text() );

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QDir::cleanPath( QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path) );

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open seqfile"), path);
    if(fileName.isEmpty()) return;

    comboBox_33->lineEdit()->setText( QDir::toNativeSeparators(fileName) );
}

void MainWindow::on_toolButton_20_clicked()
{
    QString path = QDir::cleanPath( comboBox_35->lineEdit()->text() );

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QDir::cleanPath( QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path) );

    QString directoryName = QFileDialog::getExistingDirectory(this, tr("Save output files in ..."), path);
    if(directoryName.isEmpty()) return;

    comboBox_35->lineEdit()->setText( QDir::toNativeSeparators(directoryName) );
}

void MainWindow::on_comboBox_33_editTextChanged(const QString& text)
{
    if( yn00Engine->strEdit_seqfile != text ) {
        yn00Engine->strEdit_seqfile = text;
        setModifiedFlag();
    }

    QString path = text;

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path);

    if( !QDir().exists(path) )
        comboBox_33->setStyleSheet("color: red");
    else
        comboBox_33->setStyleSheet("");
}

void MainWindow::on_lineEdit_7_textEdited(const QString& text)
{
    yn00Engine->strEdit_outfile_Name = text;
    setModifiedFlag();
}

void MainWindow::on_comboBox_35_editTextChanged(const QString& text)
{
    if( yn00Engine->strEdit_outfile_Location != text ) {
        yn00Engine->strEdit_outfile_Location = text;
        setModifiedFlag();
    }

    QString path = text;

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path);

    if( !QDir(path).exists() )
        comboBox_35->setStyleSheet("color: red");
    else
        comboBox_35->setStyleSheet("");
}

void MainWindow::on_comboBox_34_activated(int index)
{
    if( yn00Engine->nComb_verbose != index ) {
        yn00Engine->nComb_verbose = index;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_36_activated(int index)
{
    int i = yn00Engine->nComb_noisy;
    if(i > 4 && i < 9) i = 4;
    else if(i > 9) i = 9;

    if(index == 5) index = 9;

    if( i != index ) {
        yn00Engine->nComb_noisy = index;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_48_activated(int index)
{
    if( yn00Engine->nComb_icode != index ) {
        yn00Engine->nComb_icode = index;
        setModifiedFlag();
    }
}

void MainWindow::on_spinBox_10_valueChanged(int i)
{
    if( yn00Engine->nEdit_ndata != i ) {
        yn00Engine->nEdit_ndata = i;
        setModifiedFlag();
    }
}

void MainWindow::on_checkBox_4_stateChanged(int state)
{
    if( yn00Engine->bButt_weighting != bool(state) ) {
        yn00Engine->bButt_weighting = state;
        setModifiedFlag();
    }
}

void MainWindow::on_checkBox_11_stateChanged(int state)
{
    if( yn00Engine->bButt_commonkappa != bool(state) ) {
        yn00Engine->bButt_commonkappa = state;
        setModifiedFlag();
    }
}

void MainWindow::on_checkBox_12_stateChanged(int state)
{
    if( yn00Engine->bButt_commonf3x4 != bool(state) ) {
        yn00Engine->bButt_commonf3x4 = state;
        setModifiedFlag();
    }
}


//----- evolver -----

void MainWindow::on_pushButton_8_clicked()
{
    pushButton_8->setDisabled(true);

    if(!evolverOutput) {
        evolverOutput = new OutputWindow;
        evolverOutput->setWindowTitle(tr("%1 Output").arg(strList[Evolver]));
        evolverOutput->textEdit->setCurrentFont(outputFont);
        evolverEngine->setOutputWindow(evolverOutput->textEdit);
        QObject::connect(evolverOutput, SIGNAL(closed()), evolverEngine, SLOT(terminate()));
        QObject::connect(evolverOutput, SIGNAL(deleted()), evolverEngine, SLOT(removeOutputWindow()));
    }

    if( !evolverEngine->run() ) {
        pushButton_8->setEnabled(true);
    }
    else {
        evolverOutput->show();

        if( evolverOutput->isMinimized() )
            evolverOutput->showNormal();
        if( !evolverOutput->isActiveWindow() )
            evolverOutput->activateWindow();
    }
}

void MainWindow::on_pushButton_9_clicked()
{
    evolverEngine->terminate();
}

void MainWindow::on_toolButton_7_clicked()
{
    QString path = QDir::cleanPath( comboBox_10->lineEdit()->text() );

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QDir::cleanPath( QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path) );

    QString directoryName = QFileDialog::getExistingDirectory(this, tr("Save output files in ..."), path);
    if(directoryName.isEmpty()) return;

    comboBox_10->lineEdit()->setText( QDir::toNativeSeparators(directoryName) );
}

void MainWindow::on_toolButton_8_clicked()
{
    QString path = QDir::cleanPath( comboBox_11->lineEdit()->text() );

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QDir::cleanPath( QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path) );

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open amino acid substitution rate file"), path);
    if(fileName.isEmpty()) return;

    comboBox_11->lineEdit()->setText( QDir::toNativeSeparators(fileName) );
}

void MainWindow::on_toolButton_9_clicked()
{
    if( codonFrequencyDialog->exec() == QDialog::Accepted ) {
        double d;
        bool modified = false;
        for(int i=0; i<64; i++) {
            d = codonFrequencyDialog->tableWidget->item(i/4, i%4)->text().toDouble();
            if( ::qAbs(evolverEngine->nEdit_codonFrequencies[i] - d) >= precision ) {
                evolverEngine->nEdit_codonFrequencies[i] = d;
                modified = true;
            }
        }
        if(modified)
            setModifiedFlag();
    }
    else {
        for(int i=0; i<64; i++)
            codonFrequencyDialog->tableWidget->item(i/4, i%4)->setText( QString::number(evolverEngine->nEdit_codonFrequencies[i]) );
    }
}

void MainWindow::on_toolButton_10_clicked()
{
    if( aminoAcidFrequencyDialog->exec() == QDialog::Accepted ) {
        double d;
        bool modified = false;
        for(int i=0; i<20; i++) {
            d = aminoAcidFrequencyDialog->tableWidget->item(i, 1)->text().toDouble();
            if( ::qAbs(evolverEngine->nEdit_aminoAcidFrequencies[i] - d) >= precision ) {
                evolverEngine->nEdit_aminoAcidFrequencies[i] = d;
                modified = true;
            }
        }
        if(modified)
            setModifiedFlag();
    }
    else {
        for(int i=0; i<20; i++)
            aminoAcidFrequencyDialog->tableWidget->item(i, 1)->setText( QString::number(evolverEngine->nEdit_aminoAcidFrequencies[i]) );
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if( evolverEngine->fileType() != EvolverEngine::Type(index) ) {
        evolverEngine->setFileType(EvolverEngine::Type(index));
    }

    currentDocumentType = ControlFileType(MCbaseDat + int(evolverEngine->fileType()));
}

void MainWindow::on_comboBox_10_editTextChanged(const QString& text)
{
    if( evolverEngine->strEdit_Output_Location != text ) {
        evolverEngine->strEdit_Output_Location = text;
    }

    QString path = text;

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path);

    if( !QDir(path).exists() )
        comboBox_10->setStyleSheet("color: red");
    else
        comboBox_10->setStyleSheet("");
}

void MainWindow::on_comboBox_9_activated(int index)
{
    if( evolverEngine->nComb_outFormat != index ) {
        evolverEngine->nComb_outFormat = index;
        setModifiedFlag();
    }
}

void MainWindow::on_lineEdit_18_textEdited(const QString& text)
{
    evolverEngine->nEdit_seed = text.toInt();
    setModifiedFlag();
}

void MainWindow::on_spinBox_14_valueChanged(int i)
{
    if( evolverEngine->nEdit_seqs != i ) {
        evolverEngine->nEdit_seqs = i;
        setModifiedFlag();
    }
}

void MainWindow::on_spinBox_13_valueChanged(int i)
{
    if( evolverEngine->nEdit_sites != i ) {
        evolverEngine->nEdit_sites = i;
        setModifiedFlag();
    }
}

void MainWindow::on_spinBox_15_valueChanged(int i)
{
    if( evolverEngine->nEdit_replicates != i ) {
        evolverEngine->nEdit_replicates = i;
        setModifiedFlag();
    }
}

void MainWindow::on_lineEdit_25_textEdited(const QString& text)
{
    evolverEngine->nEdit_treeLength = text.toDouble();
    setModifiedFlag();
}

void MainWindow::on_lineEdit_20_textEdited(const QString& text)
{
    evolverEngine->strEdit_tree = text;
    setModifiedFlag();
}

void MainWindow::on_comboBox_7_activated(int index)
{
    if( evolverEngine->nComb_model != index ) {
        evolverEngine->nComb_model = index;
        setModifiedFlag();
    }
}

void MainWindow::on_lineEdit_19_textEdited(const QString& text)
{
    evolverEngine->strEdit_parameters = text;
    setModifiedFlag();
}

void MainWindow::on_doubleSpinBox_35_valueChanged(double d)
{
    if( ::qAbs(evolverEngine->nEdit_alpha - d) >= precision ) {
        evolverEngine->nEdit_alpha = d;
        setModifiedFlag();
    }
}

void MainWindow::on_spinBox_12_valueChanged(int i)
{
    if( evolverEngine->nEdit_ncatG != i ) {
        evolverEngine->nEdit_ncatG = i;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_47_valueChanged(double d)
{
    if( ::qAbs(evolverEngine->nEdit_baseFrequencies[0] - d) >= precision ) {
        evolverEngine->nEdit_baseFrequencies[0] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_48_valueChanged(double d)
{
    if( ::qAbs(evolverEngine->nEdit_baseFrequencies[1] - d) >= precision ) {
        evolverEngine->nEdit_baseFrequencies[1] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_49_valueChanged(double d)
{
    if( ::qAbs(evolverEngine->nEdit_baseFrequencies[2] - d) >= precision ) {
        evolverEngine->nEdit_baseFrequencies[2] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_50_valueChanged(double d)
{
    if( ::qAbs(evolverEngine->nEdit_baseFrequencies[3] - d) >= precision ) {
        evolverEngine->nEdit_baseFrequencies[3] = d;
        setModifiedFlag();
    }
}

void MainWindow::on_lineEdit_33_textEdited(const QString& text)
{
    evolverEngine->nEdit_seed = text.toInt();
    setModifiedFlag();
}

void MainWindow::on_spinBox_21_valueChanged(int i)
{
    if( evolverEngine->nEdit_seqs != i ) {
        evolverEngine->nEdit_seqs = i;
        setModifiedFlag();
    }
}

void MainWindow::on_spinBox_22_valueChanged(int i)
{
    if( evolverEngine->nEdit_sites != i ) {
        evolverEngine->nEdit_sites = i;
        setModifiedFlag();
    }
}

void MainWindow::on_spinBox_20_valueChanged(int i)
{
    if( evolverEngine->nEdit_replicates != i ) {
        evolverEngine->nEdit_replicates = i;
        setModifiedFlag();
    }
}

void MainWindow::on_lineEdit_32_textEdited(const QString& text)
{
    evolverEngine->nEdit_treeLength = text.toDouble();
    setModifiedFlag();
}

void MainWindow::on_lineEdit_34_textEdited(const QString& text)
{
    evolverEngine->strEdit_tree = text;
    setModifiedFlag();
}

void MainWindow::on_doubleSpinBox_39_valueChanged(double d)
{
    if( ::qAbs(evolverEngine->nEdit_omega - d) >= precision ) {
        evolverEngine->nEdit_omega = d;
        setModifiedFlag();
    }
}

void MainWindow::on_doubleSpinBox_40_valueChanged(double d)
{
    if( ::qAbs(evolverEngine->nEdit_kappa - d) >= precision ) {
        evolverEngine->nEdit_kappa = d;
        setModifiedFlag();
    }
}

void MainWindow::on_lineEdit_24_textEdited(const QString& text)
{
    evolverEngine->nEdit_seed = text.toInt();
    setModifiedFlag();
}

void MainWindow::on_spinBox_24_valueChanged(int i)
{
    if( evolverEngine->nEdit_seqs != i ) {
        evolverEngine->nEdit_seqs = i;
        setModifiedFlag();
    }
}

void MainWindow::on_spinBox_25_valueChanged(int i)
{
    if( evolverEngine->nEdit_sites != i ) {
        evolverEngine->nEdit_sites = i;
        setModifiedFlag();
    }
}

void MainWindow::on_spinBox_23_valueChanged(int i)
{
    if( evolverEngine->nEdit_replicates != i ) {
        evolverEngine->nEdit_replicates = i;
        setModifiedFlag();
    }
}

void MainWindow::on_lineEdit_28_textEdited(const QString& text)
{
    evolverEngine->nEdit_treeLength = text.toDouble();
    setModifiedFlag();
}

void MainWindow::on_lineEdit_29_textEdited(const QString& text)
{
    evolverEngine->strEdit_tree = text;
    setModifiedFlag();
}

void MainWindow::on_doubleSpinBox_37_valueChanged(double d)
{
    if( ::qAbs(evolverEngine->nEdit_alpha - d) >= precision ) {
        evolverEngine->nEdit_alpha = d;
        setModifiedFlag();
    }
}

void MainWindow::on_spinBox_30_valueChanged(int i)
{
    if( evolverEngine->nEdit_ncatG != i ) {
        evolverEngine->nEdit_ncatG = i;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_12_activated(int index)
{
    if( evolverEngine->nComb_model != index ) {
        evolverEngine->nComb_model = index;
        setModifiedFlag();
    }
}

void MainWindow::on_comboBox_11_editTextChanged(const QString& text)
{
    if( evolverEngine->strEdit_aminoAcidSbstRtFile != text ) {
        evolverEngine->strEdit_aminoAcidSbstRtFile = text;
        setModifiedFlag();
    }

    QString path = text;

    if( !QDir::isAbsolutePath(path) && currentDocument && !currentDocument->fileName().isEmpty() )
        path = QFileInfo( currentDocument->fileName() ).absoluteDir().absoluteFilePath(path);

    if( !QDir().exists(path) )
        comboBox_11->setStyleSheet("color: red");
    else
        comboBox_11->setStyleSheet("");
}


//-------------------- Global Function --------------------

QDataStream& operator >> (QDataStream& in, MainWindow::Pair& pair)
{
    quint32 type;
    in >> pair.fileName >> type;
    pair.fileType = MainWindow::ControlFileType(type);
    return in;
}

QDataStream& operator << (QDataStream& out, const MainWindow::Pair& pair)
{
    out << pair.fileName << quint32(pair.fileType);
    return out;
}
