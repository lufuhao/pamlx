//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include "engine.h"

class MCMCTreeEngine;
class BasemlEngine;
class CodemlEngine;
class PampEngine;
class YN00Engine;
class EvolverEngine;
class Chi2Dialog;
class CodonFrequencyDialog;
class AminoAcidFrequencyDialog;
class OutputWindow;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    enum {
        StartPage = 0,
        MCMCTree,
        BaseML,
        CodeML,
        PAMP,
        YN00,
        Evolver
    };

    enum ControlFileType {
        Invalid = -1,
        MCMCTreeCtl,
        BaseMLCtl,
        CodeMLCtl,
        PAMPCtl,
        YN00Ctl,
        MCbaseDat,
        MCcodonDat,
        MCaaDat
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

protected:
    bool eventFilter(QObject* watched, QEvent* event);
    void closeEvent(QCloseEvent* event);
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);

private:
    void loadSettings();
    void saveSettings();
    void loadDocumentFile(const QString& fileName, ControlFileType type);
    bool saveDocumentFile(const QString& fileName);
    void addToRecentFiles(const QString& fileName, ControlFileType type);
    void removeFromRecentFiles(const QString& fileName, ControlFileType type);
    ControlFileType getDocumentType(Document* document);
    bool saveModifiedFile(Document* document, bool saveInBackground = false);
    void setModifiedFlag();
    void updateEnginesPath();
    void updateOutputFont();
    void updateRecentFileActions();
    void updateUiTitle();
    void updateUi();
    void updateEngine(); //for dubug only
    void showWarningMessageWhenPathFailed(const QString& aPath, bool isExecutable);

    friend class ConfigurationDialog;

public slots:
    void onMCMCTreeEngineStateChanged(Engine::EngineState newState);
    void onBasemlEngineStateChanged(Engine::EngineState newState);
    void onCodemlEngineStateChanged(Engine::EngineState newState);
    void onPampEngineStateChanged(Engine::EngineState newState);
    void onYN00EngineStateChanged(Engine::EngineState newState);
    void onEvolverEngineStateChanged(Engine::EngineState newState);

private slots:
    void print(QPrinter* printer);
    void on_stackedWidget_currentChanged(int index);

    //menu
    void on_menuFile_aboutToShow();
    void on_actionMCMCTree_ctl_triggered();
    void on_actionBaseml_ctl_triggered();
    void on_actionCodeml_ctl_triggered();
    void on_actionPamp_ctl_triggered();
    void on_actionYN00_ctl_triggered();
    void on_actionMCbase_dat_triggered();
    void on_actionMCcodon_dat_triggered();
    void on_actionMCaa_dat_triggered();
    void on_actionOpen_triggered();
    void on_actionClose_triggered();
    bool on_actionSave_triggered();
    bool on_actionSave_As_triggered();
    void on_actionSet_Printer_Font_triggered();
    void on_actionPrint_Preview_triggered();
    void on_actionPrint_triggered();
    void onActionOpenRecentFile();
    void on_actionClear_History_triggered();

    void on_menuEdit_aboutToShow();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_actionCut_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();
    void on_actionDelete_triggered();
    void on_actionSelect_All_triggered();

    void on_menuView_aboutToShow();
    void on_actionStart_Page_triggered();
    void on_actionText_View_toggled(bool checked);

    void on_actionMCMCTree_triggered();
    void on_actionBaseml_triggered();
    void on_actionCodeml_triggered();
    void on_actionPAMP_triggered();
    void on_actionYN00_triggered();

    void on_menuTools_aboutToShow();
    void on_actionEvolver_triggered();
    void on_actionChi2_triggered();
    void on_actionConfiguration_triggered();

    void on_menuWindow_aboutToShow();
    void onActionOutputWindow();

    void on_actionPAML_User_Guide_triggered();
    void on_actionPAML_FAQ_triggered();
    void on_actionPAML_History_triggered();
    void on_actionMCMCTree_Tutorial_triggered();
    void on_actionMCMCTree_Manual_triggered();
    void on_actionHow_to_Cite_the_Program_triggered();
    void on_actionAbout_PAML_X_triggered();
    void on_actionAbout_PAML_triggered();

    //mcmctree
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_toolButton_clicked();
    void on_toolButton_2_clicked();
    void on_toolButton_4_clicked();
    void on_checkBox_toggled(bool checked);

    void on_lineEdit_5_textEdited(const QString& text);
    void on_comboBox_20_editTextChanged(const QString& text);
    void on_comboBox_21_editTextChanged(const QString& text);
    void on_lineEdit_3_textEdited(const QString& text);
    void on_comboBox_22_editTextChanged(const QString& text);
    void on_comboBox_activated(int index);
    void on_spinBox_7_valueChanged(int i);
    void on_comboBox_2_activated(int index);
    void on_comboBox_3_activated(int index);
    void on_doubleSpinBox_19_valueChanged(double d);
    void on_doubleSpinBox_20_valueChanged(double d);
    void on_lineEdit_6_textEdited(const QString& text);
    void on_doubleSpinBox_17_valueChanged(double d);
    void on_spinBox_2_valueChanged(int i);
    void on_checkBox_2_stateChanged(int state);
    void on_doubleSpinBox_7_valueChanged(double d);
    void on_doubleSpinBox_8_valueChanged(double d);
    void on_doubleSpinBox_13_valueChanged(double d);
    void on_doubleSpinBox_18_valueChanged(double d);
    void on_doubleSpinBox_valueChanged(double d);
    void on_doubleSpinBox_2_valueChanged(double d);
    void on_doubleSpinBox_3_valueChanged(double d);
    void on_doubleSpinBox_4_valueChanged(double d);
    void on_comboBox_50_activated(int index);
    void on_doubleSpinBox_5_valueChanged(double d);
    void on_doubleSpinBox_6_valueChanged(double d);
    void on_doubleSpinBox_27_valueChanged(double d);
    void on_doubleSpinBox_9_valueChanged(double d);
    void on_doubleSpinBox_10_valueChanged(double d);
    void on_doubleSpinBox_28_valueChanged(double d);
    void on_checkBox_3_stateChanged(int state);
    void on_spinBox_3_valueChanged(int i);
    void on_spinBox_4_valueChanged(int i);
    void on_spinBox_5_valueChanged(int i);
    void on_checkBox_stateChanged(int state);
    void on_doubleSpinBox_11_valueChanged(double d);
    void on_doubleSpinBox_12_valueChanged(double d);
    void on_doubleSpinBox_14_valueChanged(double d);
    void on_doubleSpinBox_15_valueChanged(double d);
    void on_doubleSpinBox_16_valueChanged(double d);

    //baseml
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_toolButton_3_clicked();
    void on_toolButton_5_clicked();
    void on_toolButton_6_clicked();

    void on_comboBox_17_editTextChanged(const QString& text);
    void on_comboBox_18_editTextChanged(const QString& text);
    void on_lineEdit_9_textEdited(const QString& text);
    void on_comboBox_19_editTextChanged(const QString& text);
    void on_checkBox_5_stateChanged(int state);
    void on_comboBox_16_activated(int index);
    void on_comboBox_15_activated(int index);
    void on_comboBox_6_activated(int index);
    void on_doubleSpinBox_21_valueChanged(double d);
    void on_doubleSpinBox_22_valueChanged(double d);
    void on_comboBox_4_activated(int index);
    void on_comboBox_14_activated(int index);
    void on_checkBox_9_stateChanged(int state);
    void on_checkBox_10_stateChanged(int state);
    void on_comboBox_5_activated(int index);
    void on_checkBox_7_stateChanged(int state);
    void on_doubleSpinBox_36_valueChanged(double d);
    void on_checkBox_8_stateChanged(int state);
    void on_doubleSpinBox_23_valueChanged(double d);
    void on_checkBox_6_stateChanged(int state);
    void on_spinBox_6_valueChanged(int i);
    void on_checkBox_13_stateChanged(int state);
    void on_doubleSpinBox_24_valueChanged(double d);
    void on_comboBox_13_activated(int index);
    void on_spinBox_8_valueChanged(int i);
    void on_lineEdit_12_textEdited(const QString& text);
    void on_comboBox_26_activated(int index);
    void on_comboBox_8_activated(int index);

    //codeml
    void on_pushButton_7_clicked();
    void on_pushButton_10_clicked();
    void on_toolButton_11_clicked();
    void on_toolButton_12_clicked();
    void on_toolButton_13_clicked();
    void on_toolButton_17_clicked();
    void on_radioButton_toggled(bool checked);
    void on_comboBox_37_currentIndexChanged(int index);
    void on_comboBox_31_currentIndexChanged(int index);
    void onListWidgetItemExclusive(QListWidgetItem* item);

    void on_comboBox_23_editTextChanged(const QString& text);
    void on_comboBox_24_editTextChanged(const QString& text);
    void on_lineEdit_10_textEdited(const QString& text);
    void on_comboBox_25_editTextChanged(const QString& text);
    void on_comboBox_31_activated(int index);
    void on_checkBox_25_stateChanged(int state);
    void on_comboBox_38_currentIndexChanged(int index);
    void on_doubleSpinBox_33_valueChanged(double d);
    void on_doubleSpinBox_34_valueChanged(double d);
    void on_doubleSpinBox_38_valueChanged(double d);
    void on_doubleSpinBox_42_valueChanged(double d);
    void on_comboBox_44_activated(int index);
    void on_comboBox_41_activated(int index);
    void on_checkBox_22_stateChanged(int state);
    void on_checkBox_23_stateChanged(int state);
    void on_comboBox_42_activated(int index);
    void on_comboBox_47_activated(int index);
    void on_comboBox_39_currentIndexChanged(int index);
    void on_comboBox_43_activated(int index);
    void on_comboBox_45_editTextChanged(const QString& text);
    void on_listWidget_itemChanged(QListWidgetItem* item);
    void on_comboBox_46_activated(int index);
    void on_comboBox_40_currentIndexChanged(int index);
    void on_checkBox_19_stateChanged(int state);
    void on_doubleSpinBox_41_valueChanged(double d);
    void on_checkBox_26_stateChanged(int state);
    void on_doubleSpinBox_26_valueChanged(double d);
    void on_checkBox_20_stateChanged(int state);
    void on_doubleSpinBox_25_valueChanged(double d);
    void on_checkBox_21_stateChanged(int state);
    void on_spinBox_11_valueChanged(int i);
    void on_spinBox_16_valueChanged(int i);
    void on_lineEdit_14_textEdited(const QString& text);
    void on_comboBox_49_activated(int index);

    //pamp
    void on_pushButton_11_clicked();
    void on_pushButton_12_clicked();
    void on_toolButton_14_clicked();
    void on_toolButton_15_clicked();
    void on_toolButton_16_clicked();

    void on_comboBox_27_editTextChanged(const QString& text);
    void on_comboBox_28_editTextChanged(const QString& text);
    void on_lineEdit_4_textEdited(const QString& text);
    void on_comboBox_29_editTextChanged(const QString& text);
    void on_comboBox_32_activated(int index);
    void on_comboBox_30_activated(int index);
    void on_spinBox_9_valueChanged(int i);

    //yn00
    void on_pushButton_13_clicked();
    void on_pushButton_14_clicked();
    void on_toolButton_18_clicked();
    void on_toolButton_20_clicked();

    void on_comboBox_33_editTextChanged(const QString& text);
    void on_lineEdit_7_textEdited(const QString& text);
    void on_comboBox_35_editTextChanged(const QString& text);
    void on_comboBox_34_activated(int index);
    void on_comboBox_36_activated(int index);
    void on_comboBox_48_activated(int index);
    void on_spinBox_10_valueChanged(int i);
    void on_checkBox_4_stateChanged(int state);
    void on_checkBox_11_stateChanged(int state);
    void on_checkBox_12_stateChanged(int state);

    //evolver
    void on_pushButton_8_clicked();
    void on_pushButton_9_clicked();
    void on_toolButton_7_clicked();
    void on_toolButton_8_clicked();
    void on_toolButton_9_clicked();
    void on_toolButton_10_clicked();
    void on_tabWidget_currentChanged(int index);

    void on_comboBox_10_editTextChanged(const QString& text);
    void on_comboBox_9_activated(int index);

    void on_lineEdit_18_textEdited(const QString& text);
    void on_spinBox_14_valueChanged(int i);
    void on_spinBox_13_valueChanged(int i);
    void on_spinBox_15_valueChanged(int i);
    void on_lineEdit_25_textEdited(const QString& text);
    void on_lineEdit_20_textEdited(const QString& text);
    void on_comboBox_7_activated(int index);
    void on_lineEdit_19_textEdited(const QString& text);
    void on_doubleSpinBox_35_valueChanged(double d);
    void on_spinBox_12_valueChanged(int i);
    void on_doubleSpinBox_47_valueChanged(double d);
    void on_doubleSpinBox_48_valueChanged(double d);
    void on_doubleSpinBox_49_valueChanged(double d);
    void on_doubleSpinBox_50_valueChanged(double d);

    void on_lineEdit_33_textEdited(const QString& text);
    void on_spinBox_21_valueChanged(int i);
    void on_spinBox_22_valueChanged(int i);
    void on_spinBox_20_valueChanged(int i);
    void on_lineEdit_32_textEdited(const QString& text);
    void on_lineEdit_34_textEdited(const QString& text);
    void on_doubleSpinBox_39_valueChanged(double d);
    void on_doubleSpinBox_40_valueChanged(double d);

    void on_lineEdit_24_textEdited(const QString& text);
    void on_spinBox_24_valueChanged(int i);
    void on_spinBox_25_valueChanged(int i);
    void on_spinBox_23_valueChanged(int i);
    void on_lineEdit_28_textEdited(const QString& text);
    void on_lineEdit_29_textEdited(const QString& text);
    void on_doubleSpinBox_37_valueChanged(double d);
    void on_spinBox_30_valueChanged(int i);
    void on_comboBox_12_activated(int index);
    void on_comboBox_11_editTextChanged(const QString& text);

public:
    struct Pair{
        Pair() {}
        Pair(const QString& name, ControlFileType type) : fileName(name), fileType(type) {}
        bool operator == (const Pair& pair) { return (fileName == pair.fileName); }

        QString fileName;
        ControlFileType fileType;
    };

private:
    QString                   pamlPath;
    QFont                     outputFont;
    QFont                     printFont;
    QList<Pair>               recentFiles;
    QString                   lastFileOpenPath;
    QString                   absolutePAMLPath;
    const QFont               defaultOutputFont;
    const QFont               defaultPrintFont;

    MCMCTreeEngine*           mcmcTreeEngine;
    BasemlEngine*             basemlEngine;
    CodemlEngine*             codemlEngine;
    PampEngine*               pampEngine;
    YN00Engine*               yn00Engine;
    EvolverEngine*            evolverEngine;

    OutputWindow*             mcmcTreeOutput;
    OutputWindow*             basemlOutput;
    OutputWindow*             codemlOutput;
    OutputWindow*             pampOutput;
    OutputWindow*             yn00Output;
    OutputWindow*             evolverOutput;

    Document*                 currentDocument;
    ControlFileType           currentDocumentType;

    enum {
        MaxRecentFiles = 10,
        MaxOutputWindows = 6
    };

    QAction*                  recentFileActions[MaxRecentFiles];
    QAction*                  outputWindowActions[MaxOutputWindows];
    Chi2Dialog*               chi2Dialog;
    CodonFrequencyDialog*     codonFrequencyDialog;
    AminoAcidFrequencyDialog* aminoAcidFrequencyDialog;
    QWidget*                  focusEdit;
    QActionGroup*             actionGroup;
    QButtonGroup*             buttonGroup;
};


//-------------------- Global Function --------------------

QDataStream& operator >> (QDataStream& in, MainWindow::Pair& pair);
QDataStream& operator << (QDataStream& out, const MainWindow::Pair& pair);

#endif // MAINWINDOW_H
