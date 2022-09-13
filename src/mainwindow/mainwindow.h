#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include "manualstagecontrol.h"
#include "manualpumpcontrol.h"
#include "imageprocessing.h"
#include "imagepopout.h"
#include "PrintElements.h"
#include "printcontrol.h"
#include "focuscal.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/*!
 * \brief The MainWindow class handles front end ui
 */
class MainWindow : public QMainWindow {
    Q_OBJECT


public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    imagepopout popout;
    printcontrol PrintControl;


    // enums for hardware selection dropdowns
    enum PumpSelect_t {
        PUMP_NONE,
        PUMP_HARVARDAPPARATUS_V1,
        PUMP_DEBUG,
    } PumpSelect;

    enum RollToRollSelect_t {
        ROLLTOROLL_NONE,
        ROLLTOROLL_ARDUINO_V1
    } RollToRollSelect;

    enum StageSelect_t {
        STAGE_SMC100CC,
        STAGE_KVS30,
        STAGE_GGCODE,
        STAGE_DEBUG
    } StageSelect;

    enum LightEngineSelect_t {
        LIGHTENGINE_DLI,
        LIGHTENGINE_FIREBIRD,
        LIGHTENGINE_DEBUG,
    } LightEngineSelect;

    enum ProjectionModeSelect_t {
        PROJECTIONMODE_POTF,
        PROJECTIONMODE_VP,
        PROJECTIONMODE_VIDEO
    } ProjectionModeSelect;

    
public slots:
    void showError(QString errMsg);
    void PrintToTerminal(QString StringToPrint);
    void refreshScript();
    
private slots:
    void on_ManualStage_clicked();
    
    void on_InitializeAndSynchronize_clicked();
    
    void on_StartPrint_clicked();
    
    void on_StageConnectButton_clicked();
    
    void on_LightEngineConnectButton_clicked();
    
    void ExposureTimeSlot();
    
    void DarkTimeSlot();
    
    void PrintProcess();
    
    void saveText();
    
    void on_LogFileBrowse_clicked();
    
    void on_AbortPrint_clicked();
    
    void on_ManualPumpControl_clicked();
    
    void on_PumpConnectButton_clicked();
    
    void on_GetPosition_clicked();
    
    void on_ImageProcess_clicked();
    
    void on_POTFcheckbox_clicked();
    
    void on_VP_HDMIcheckbox_clicked();
    
    void Check4VideoLock();
    
    void on_DICLIPSelect_clicked();
    
    void pumpingSlot();
    
    void on_CLIPSelect_clicked();
    
    void on_NewCLIPSelect_clicked();

    //void on_PumpDropdown_clicked();
    
    void updatePosition(QString CurrentPosition);
    
    void StageConnected();
    
    void PumpConnected();
    
    void on_LogName_textChanged();
    
    void on_VideoCheckbox_clicked();
    
    void on_FocusCal_clicked();
    
    void on_resetButton_clicked();

    void on_PumpDropdown_activated(int index);
    void on_RollToRollDropdown_activated(int index);
    void on_StageDropdown_activated(int index);
    void on_LightEngineDropdown_activated(int index);
    void on_ProjectionModeDropdown_activated(int index);

    void PumpSelect_f(PumpSelect_t index);
    void RollToRollSelect_f(RollToRollSelect_t index);
    void StageSelect_f(StageSelect_t index);
    void LightEngineSelect_f(LightEngineSelect_t index);
    void ProjectionModeSelect_f(ProjectionModeSelect_t index);


private:
    Ui::MainWindow* ui;
    PrintScripts m_PrintScript;
    PrintSettings m_PrintSettings;
    PrintControls m_PrintControls;
    InjectionSettings m_InjectionSettings;
    ManualStageControl* ManualStageUI;
    manualpumpcontrol* ManualPumpUI;
    imageprocessing* ImageProcessUI;
    imagepopout* ImagePopoutUI;
    FocusCal* FocusCalUI;
    
    bool ValidateSettings(void);
    void loadSettings();
    void saveSettings();
    void initSettings();
    QVector<double> qv_x, qv_y;
    void printParameters();
    bool initConfirmationScreen();
    void AutoMode();
    void initImagePopout();
    void SetExposureTimer();
    void SetDarkTimer();
    void VP8bitWorkaround();
    bool PrintScriptApply(uint layerCount, QStringList Script, Parameter_t DynamicParam);
    void PrintComplete();
    double CalcPrintEnd(PrintControls m_PrintControls, PrintSettings m_PrintSettings);
    double CalcContinuousVelocity(PrintSettings m_PrintSettings);
    QStringList GetImageList(PrintControls m_PrintControls, PrintSettings m_PrintSettings);
    void PrintScriptHandler(PrintControls m_PrintControls, PrintSettings m_PrintSettings, PrintScripts m_PrintScript);
    
    void InitializeMW();
    void StartPrint();
    void PrintProcess2();
    void initConnections();
    bool initResetConfirmation();
    void autoConnect();
    
    void setScriptParam(float& param, QStringList script, int layerCount);
};
#endif // MAINWINDOW_H
