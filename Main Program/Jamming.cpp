#include "Jamming.h"
#include <QMessageBox>
#include <random>
#include <QPointF>

Jamming::Jamming(QWidget* parent): QMainWindow(parent),
    LimeSDR(nullptr),
    txThread(nullptr),
    isTransmitting(false),
    usb2anyProcess(nullptr),
    hPipe(INVALID_HANDLE_VALUE)
{
    ui.setupUi(this);
    connectusb2any_helper();
    ui.stackedWidget->setCurrentIndex(0);
    //windows transfer
    connect(ui.pushButton_Operational, &QPushButton::clicked, this, &Jamming::openOperationalWindow);
    connect(ui.pushButton_Tecnical, &QPushButton::clicked, this, &Jamming::openTecnicalWindow);
    connect(ui.pushButton_Graph, &QPushButton::clicked, this, &Jamming::openGraphWindow);
    connect(ui.pushButton_Information, &QPushButton::clicked, this, &Jamming::openInformation);
    connect(ui.pushButton_EXIT, &QPushButton::clicked, this, &Jamming::returnToMain);
    connect(ui.pushButton_EXIT_2, &QPushButton::clicked, this, &Jamming::returnToMain);
    connect(ui.pushButton_EXIT_3, &QPushButton::clicked, this, &Jamming::returnToMain);
    connect(ui.pushButton_EXIT_4, &QPushButton::clicked, this, &Jamming::returnToMain);
    QPixmap pixmap("redred.png");
    ui.label_Diagram->setPixmap(pixmap.scaled(
        ui.label_Diagram->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation));

    QPixmap pixmapBackGround("openwindowW2.png");
    ui.label_backGround->setPixmap(pixmapBackGround.scaled(
        ui.label_backGround->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation));

    QPixmap StartTransmissionGPS("Start_GPS.png");
    ui.label_StartTransmissionGPS->setPixmap(StartTransmissionGPS.scaled(
        ui.label_StartTransmissionGPS->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation));
    QPixmap StartTransmissionWIFI24("Start_24.png");
    ui.label_StartTransmissionWIFI24->setPixmap(StartTransmissionWIFI24.scaled(
        ui.label_StartTransmissionWIFI24->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation));
    QPixmap StartTransmissionWIFI58("Start_58.png");
    ui.label_StartTransmissionWIFI58->setPixmap(StartTransmissionWIFI58.scaled(
        ui.label_StartTransmissionWIFI58->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation));


    //graph
    connect(ui.pushButton_TriangleSweep24, &QPushButton::clicked, this, &Jamming::TriangleSweep24);
    connect(ui.pushButton_FFT24, &QPushButton::clicked, this, &Jamming::FFT24);
    connect(ui.pushButton_TriangleSweep58, &QPushButton::clicked, this, &Jamming::TriangleSweep58);
    connect(ui.pushButton_FFT58, &QPushButton::clicked, this, &Jamming::FFT58);
    connect(ui.pushButton_noiseGPS, &QPushButton::clicked, this, &Jamming::noiseGPS);
    connect(ui.pushButton_FFTGPS, &QPushButton::clicked, this, &Jamming::FFTGPS);

    chartTriangle24 = new QChart();
    ViewTriangle24 = new QChartView(chartTriangle24);
    ViewTriangle24->setRenderHint(QPainter::Antialiasing);
    ViewTriangle24->setParent(ui.horizontalFrameTriangle24);

    chartFFT24 = new QChart();
    ViewFFT24 = new QChartView(chartFFT24);
    ViewFFT24->setRenderHint(QPainter::Antialiasing);
    ViewFFT24->setParent(ui.horizontalFrameFFT24);

    chartTriangle58 = new QChart();
    ViewTriangle58 = new QChartView(chartTriangle58);
    ViewTriangle58->setRenderHint(QPainter::Antialiasing);
    ViewTriangle58->setParent(ui.horizontalFrameTriangle58);

    chartFFT58 = new QChart();
    ViewFFT58 = new QChartView(chartFFT58);
    ViewFFT58->setRenderHint(QPainter::Antialiasing);
    ViewFFT58->setParent(ui.horizontalFrameFFT58);

    chartNoiseGPS = new QChart();
    ViewNoiseGPS = new QChartView(chartNoiseGPS);
    ViewNoiseGPS->setRenderHint(QPainter::Antialiasing);
    ViewNoiseGPS->setParent(ui.horizontalFrameNoiseGPS);

    chartFFTGPS = new QChart();
    ViewFFTGPS = new QChartView(chartFFTGPS);
    ViewFFTGPS->setRenderHint(QPainter::Antialiasing);
    ViewFFTGPS->setParent(ui.horizontalFrameFFTGPS);

    //LimeSDRmini2 Startconfig
    centerFreq = ((ui.spinBox_LIME_StopFreq->value() + ui.spinBox_LIME_StartFreq->value()) / 2) * 1e6;
    sampleRate = (ui.spinBox_LIME_StopFreq->value() - ui.spinBox_LIME_StartFreq->value()) * 1e6;
    tx_buffer = generateFilteredGaussianSignal();
    send_cnt = tx_buffer.size() / 2;
    // Connect buttons to functions
    connect(ui.pushButton_ConnectLimeSDR, &QPushButton::clicked, this, &Jamming::connectLimeSDR);
    connect(ui.pushButton_ConfigureLime, &QPushButton::clicked, this, &Jamming::configureLimeSDR);
    connect(ui.pushButton_StartTransmissionGPS, &QPushButton::clicked, this, &Jamming::startTransmission);
    connect(ui.pushButton_StopTransmissionGPS, &QPushButton::clicked, this, &Jamming::stopTransmission);
    ui.pushButton_StartTransmissionGPS->setEnabled(false);
    ui.pushButton_StopTransmissionGPS->setEnabled(false);
    // LMX2492EVM24
    //connect(ui.pushButton_usb2any_helper, &QPushButton::clicked, this, &Jamming::connectusb2any_helper);
    connect(ui.pushButton_ConnectLMX2492EVM24, &QPushButton::clicked, this, &Jamming::connectLMX2492EVM24);
    connect(ui.pushButton_ConfigureLMX2492EVM24, &QPushButton::clicked, this, &Jamming::configureLMX2492EVM24);
    connect(ui.pushButton_StartTransmissionWIFI24, &QPushButton::clicked, this, &Jamming::startTransmission24);
    connect(ui.pushButton_StopTransmissionWIFI24, &QPushButton::clicked, this, &Jamming::stopTransmission24);
    ui.pushButton_StartTransmissionWIFI24->setEnabled(false);
    ui.pushButton_StopTransmissionWIFI24->setEnabled(false);
    // LMX2492EVM58
    connect(ui.pushButton_ConnectLMX2492EVM58, &QPushButton::clicked, this, &Jamming::connectLMX2492EVM58);
    connect(ui.pushButton_ConfigureLMX2492EVM58, &QPushButton::clicked, this, &Jamming::configureLMX2492EVM58);
    connect(ui.pushButton_StartTransmissionWIFI58, &QPushButton::clicked, this, &Jamming::startTransmission58);
    connect(ui.pushButton_StopTransmissionWIFI58, &QPushButton::clicked, this, &Jamming::stopTransmission58);
    ui.pushButton_StartTransmissionWIFI58->setEnabled(false);
    ui.pushButton_StopTransmissionWIFI58->setEnabled(false);

    //info
    connect(ui.pushButton_tipGPS, &QPushButton::clicked, this, &Jamming::functipGPS);
    connect(ui.pushButton_tip24, &QPushButton::clicked, this, &Jamming::functip24);
    connect(ui.pushButton_tip58, &QPushButton::clicked, this, &Jamming::functip58);
    ui.label_tipGPS->hide();
    ui.label_tip24->hide();
    ui.label_tip58->hide();

    //ui.label_LMX2492EVM->setText("Connected");
    //ui.label_LMX2492EVM->setStyleSheet("QLabel { color : green; }");
    //ui.label_transmission24->setText("Ready For Operation");
    //ui.label_transmission24->setStyleSheet("QLabel { color : green; }");
    //ui.pushButton_StartTransmissionWIFI24->setEnabled(true);
    //updateDiagram();

}

Jamming::~Jamming()
{

    if (txThread)
    {
        txThread->join();
        delete txThread;
        txThread = nullptr;
    }
    if (LimeSDR)
    {
        LMS_Close(LimeSDR); // Close SDR before exiting
        //delete LimeSDR; //?
    }

    if (hPipe != INVALID_HANDLE_VALUE) {
        string exitCmd = "EXIT";
        DWORD written;
        WriteFile(hPipe, exitCmd.c_str(), exitCmd.length(), &written, NULL); 
        CloseHandle(hPipe);  // Close pipe connection
        hPipe = INVALID_HANDLE_VALUE;
    }
    if (usb2anyProcess) {
        if (usb2anyProcess->state() != QProcess::NotRunning) {
            usb2anyProcess->waitForFinished(1000); // wait up to 1s for it to exit
            if (usb2anyProcess->state() != QProcess::NotRunning) {
                usb2anyProcess->terminate();  // force kill if still running
            }
        }
        delete usb2anyProcess;
        usb2anyProcess = nullptr;
    }

    if (seriesTriangle24) {
        chartTriangle24->removeSeries(seriesTriangle24);
        delete seriesTriangle24;
    }
    // ואז ממלא

    if (seriesFFT24) {
        chartFFT24->removeSeries(seriesFFT24);
        delete seriesFFT24;
    }

    if (seriesTriangle58) {
        chartTriangle58->removeSeries(seriesTriangle58);
        delete seriesTriangle58;
    }

    if (seriesFFT58) {
        chartFFT58->removeSeries(seriesFFT58);
        delete seriesFFT58;
    }

    if (seriesNoiseGPS) {
        chartNoiseGPS->removeSeries(seriesNoiseGPS);
        delete seriesNoiseGPS;
    }

    if (seriesFFTGPS) {
        chartFFTGPS->removeSeries(seriesFFTGPS);
        delete seriesFFTGPS;
    }

    if (chartTriangle24) delete chartTriangle24;
    if (ViewTriangle24) delete ViewTriangle24;

    if (chartFFT24) delete chartFFT24;
    if (ViewFFT24) delete ViewFFT24;

    if (chartTriangle58) delete chartTriangle58;
    if (ViewTriangle58) delete ViewTriangle58;

    if (chartFFT58) delete chartFFT58;
    if (ViewFFT58) delete ViewFFT58;

    if (chartNoiseGPS) delete chartNoiseGPS;
    if (ViewNoiseGPS) delete ViewNoiseGPS;

    if (chartFFTGPS) delete chartFFTGPS;
    if (ViewFFTGPS) delete ViewFFTGPS;


}

//graph
void Jamming::drawChart()
{
    if (seriesTriangle24) {
        chartTriangle24->removeSeries(seriesTriangle24);
        delete seriesTriangle24;
    }
    seriesTriangle24 = new QLineSeries();
    // ואז ממלא

    if (seriesFFT24) {
        chartFFT24->removeSeries(seriesFFT24);
        delete seriesFFT24;
    }
    seriesFFT24 = new QLineSeries();

    if (seriesTriangle58) {
        chartTriangle58->removeSeries(seriesTriangle58);
        delete seriesTriangle58;
    }
    seriesTriangle58 = new QLineSeries();

    if (seriesFFT58) {
        chartFFT58->removeSeries(seriesFFT58);
        delete seriesFFT58;
    }
    seriesFFT58 = new QLineSeries();

    if (seriesNoiseGPS) {
        chartNoiseGPS->removeSeries(seriesNoiseGPS);
        delete seriesNoiseGPS;
    }
    seriesNoiseGPS = new QLineSeries();

    if (seriesFFTGPS) {
        chartFFTGPS->removeSeries(seriesFFTGPS);
        delete seriesFFTGPS;
    }
    seriesFFTGPS = new QLineSeries();

    double startFreqGPS = ui.spinBox_LIME_StartFreq->value();     // MHz
    double endFreqGPS = ui.spinBox_LIME_StopFreq->value();       // MHz
    double startFreq = ui.spinBox_StartFrequencyVCO->value();     // MHz
    double endFreq = ui.spinBox_StopFrequencyVCO->value();       // MHz
    int totalTime_us = ui.spinBox_SweepDuration->value();
    int riseTime = totalTime_us / 2;
    int fallTime = totalTime_us / 2;
    double period = riseTime + fallTime;
    int numCycles = 5;

    int pointsPerCycle = 200; // רזולוציה של כל מחזור
    int totalPoints = pointsPerCycle * numCycles;

    for (int i = 0; i <= totalPoints; ++i)
    {
        double t = (i * period * 1.0) / pointsPerCycle; // זמן במיקרו-שניות
        double cycleTime = fmod(t, period); // זמן בתוך מחזור בודד

        double freq;
        if (cycleTime <= riseTime) {
            freq = startFreq + ((endFreq - startFreq) * (cycleTime / riseTime));
        }
        else {
            double tFall = cycleTime - riseTime;
            freq = endFreq - ((endFreq - startFreq) * (tFall / fallTime));
        }

        seriesTriangle24->append(t, freq);
    }
    //chartTriangle24->removeAllSeries();
    chartTriangle24->addSeries(seriesTriangle24);
    chartTriangle24->createDefaultAxes();
    chartTriangle24->setTitle("Triangle Frequency Sweep 2.4 GHz     X-axis: Time (μs), Y-axis: Frequency (MHz)");

    seriesFFT24->append(startFreq-100, 0);
    seriesFFT24->append(startFreq, 0);

    seriesFFT24->append(startFreq, 30);
    seriesFFT24->append(endFreq, 30);

    seriesFFT24->append(endFreq, 0);
    seriesFFT24->append(endFreq+100, 0);

    //chartFFT24->removeAllSeries();
    chartFFT24->addSeries(seriesFFT24);
    chartFFT24->createDefaultAxes();
    chartFFT24->setTitle("Frequency Domain 2.4 GHz     X-axis: Frequency (MHz), Y-axis: Amplitude (dBm)");

    startFreq = ui.spinBox_StartFrequencyVCO58->value();     // MHz
    endFreq = ui.spinBox_StopFrequencyVCO58->value();       // MHz
    totalTime_us = ui.spinBox_SweepDuration58->value();
    riseTime = totalTime_us / 2;
    fallTime = totalTime_us / 2;
    period = riseTime + fallTime;
    numCycles = 5;

    pointsPerCycle = 200; // רזולוציה של כל מחזור
    totalPoints = pointsPerCycle * numCycles;

    for (int i = 0; i <= totalPoints; ++i)
    {
        double t = (i * period * 1.0) / pointsPerCycle; // זמן במיקרו-שניות
        double cycleTime = fmod(t, period); // זמן בתוך מחזור בודד

        double freq;
        if (cycleTime <= riseTime) {
            freq = startFreq + ((endFreq - startFreq) * (cycleTime / riseTime));
        }
        else {
            double tFall = cycleTime - riseTime;
            freq = endFreq - ((endFreq - startFreq) * (tFall / fallTime));
        }

        seriesTriangle58->append(t, freq);
    }

    //chartTriangle58->removeAllSeries();
    chartTriangle58->addSeries(seriesTriangle58);
    chartTriangle58->createDefaultAxes();
    chartTriangle58->setTitle("Triangle Frequency Sweep 5.8 GHz     X-axis: Time (μs), Y-axis: Frequency (MHz)");


    seriesFFT58->append(startFreq-100, 0);
    seriesFFT58->append(startFreq, 0);

    seriesFFT58->append(startFreq, 30);
    seriesFFT58->append(endFreq, 30);

    seriesFFT58->append(endFreq, 0);
    seriesFFT58->append(endFreq+100, 0);

    //chartFFT58->removeAllSeries();
    chartFFT58->addSeries(seriesFFT58);
    chartFFT58->createDefaultAxes();
    chartFFT58->setTitle("Frequency Domain 5.8 GHz     X-axis: Frequency (MHz), Y-axis: Amplitude (dBm)");


    for (int i = 0; i < send_cnt; ++i)
    {
        double t = i * 1e6 / sampleRate; 
        double amplitude = tx_buffer[2 * i]; 
        seriesNoiseGPS->append(t, amplitude);
    }

    //chartNoiseGPS->removeAllSeries();
    chartNoiseGPS->addSeries(seriesNoiseGPS);
    chartNoiseGPS->createDefaultAxes();
    chartNoiseGPS->setTitle("GPS Gaussian Noise Signal - Time Domain");

    seriesFFTGPS->append(startFreqGPS - 100, 0);
    seriesFFTGPS->append(startFreqGPS, 0);

    seriesFFTGPS->append(startFreqGPS, 30);
    seriesFFTGPS->append(endFreqGPS, 30);

    seriesFFTGPS->append(endFreqGPS, 0);
    seriesFFTGPS->append(endFreqGPS + 100, 0);

    //chartFFTGPS->removeAllSeries();
    chartFFTGPS->addSeries(seriesFFTGPS);
    chartFFTGPS->createDefaultAxes();
    chartFFTGPS->setTitle("Frequency Domain GPS     X-axis: Frequency (MHz), Y-axis: Amplitude (dBm)");

}

void Jamming::TriangleSweep24()
{
    drawChart();
    ui.stackedWidgetGraph->setCurrentIndex(0);
}
void Jamming::FFT24()
{
    drawChart();
    ui.stackedWidgetGraph->setCurrentIndex(1);
}
void Jamming::TriangleSweep58()
{
    drawChart();
    ui.stackedWidgetGraph->setCurrentIndex(2);
}
void Jamming::FFT58()
{
    drawChart();
    ui.stackedWidgetGraph->setCurrentIndex(3);
}
void Jamming::noiseGPS()
{
    drawChart();
    ui.stackedWidgetGraph->setCurrentIndex(4);
}
void Jamming::FFTGPS()
{
    drawChart();
    ui.stackedWidgetGraph->setCurrentIndex(5);
}

//windows
void Jamming::updateDiagram()
{
    QString limeText = ui.label_LimeSDR->text();
    QString lmxText = ui.label_LMX2492EVM->text();

    QString fileName;

    if (limeText == "Connected" && lmxText == "Connected")
        fileName = "greengreen.png";
    else if (limeText == "Disconnected" && lmxText == "Connected")
        fileName = "greenred.png";
    else if (limeText == "Connected" && lmxText == "Disconnected")
        fileName = "redgreen.png";
    else
        fileName = "redred.png";

    QPixmap pixmap(fileName);
    ui.label_Diagram->setPixmap(pixmap.scaled(
        ui.label_Diagram->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation));
}
void Jamming::openOperationalWindow()
{
    ui.stackedWidget->setCurrentIndex(1);
}
void Jamming::openTecnicalWindow()
{
    ui.stackedWidget->setCurrentIndex(2);
}
void Jamming::openGraphWindow()
{
    ui.stackedWidget->setCurrentIndex(3);
}
void Jamming::openInformation()
{
    ui.stackedWidget->setCurrentIndex(4);
}
void Jamming::returnToMain()
{
    ui.stackedWidget->setCurrentIndex(0);
}

void Jamming::functipGPS()
{
    ui.label_tipGPS->setVisible(!ui.label_tipGPS->isVisible());
}
void Jamming::functip24()
{
    ui.label_tip24->setVisible(!ui.label_tip24->isVisible());
}
void Jamming::functip58()
{
    ui.label_tip58->setVisible(!ui.label_tip58->isVisible());
}



void Jamming::connectusb2any_helper()
{
    usb2anyProcess = new QProcess(this);
    usb2anyProcess->start("usb2any_helper.exe");
     // Wait for the process to start (Max 10 seconds)
    if (!usb2anyProcess->waitForStarted(10000)) {
        QMessageBox::critical(this, "Connection Failed", "Unable to connect to usb2any_helper.\nPlease restart the program.");

        return;
    }
    if (!usb2anyProcess->waitForReadyRead(10000)) {
        QMessageBox::critical(this, "Connection Failed", "Unable to connect to usb2any_helper.\nPlease restart the program.");
        return;
    }
    hPipe = CreateFileA("\\\\.\\pipe\\usb2any_pipe", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hPipe == INVALID_HANDLE_VALUE) {
        QMessageBox::critical(this, "Connection Failed", "Unable to connect to usb2any_helper.\nPlease restart the program.");
        return;
    }
    char buffer[128];
    DWORD bytesRead;
    ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL);
    buffer[bytesRead] = '\0';
    if (QString::fromStdString(buffer) != "connected")
    {
        QMessageBox::critical(this, "Connection Failed", "Unable to connect to usb2any_helper.\nPlease restart the program.");
    }
}

//LMX24
void Jamming::connectLMX2492EVM24()
{
    if (hPipe == INVALID_HANDLE_VALUE) {
        QMessageBox::warning(this, "Connection Failed", "Unable to establish connection with LMX2492EVM.\nPlease check the device and try again.");
        return;
    }
    string command = "connectLMX2492EVM24";
    DWORD bytesWritten;
    WriteFile(hPipe, command.c_str(), command.length(), &bytesWritten, NULL);
    char buffer[128];
    DWORD bytesRead;
    ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL);
    buffer[bytesRead] = '\0';
    if (QString::fromStdString(buffer) == "SUCCESS:LMX2492EVM connected")
    {
        ui.label_LMX2492EVM->setText("Connected");
        ui.label_LMX2492EVM->setStyleSheet("QLabel { color : green; }");
        ui.label_transmission24->setText("Ready For Operation");
        ui.label_transmission24->setStyleSheet("QLabel { color : green; }");
        ui.pushButton_StartTransmissionWIFI24->setEnabled(true);
        updateDiagram();
        QMessageBox::information(this, "Connection Successful", "LMX2492EVM has been successfully connected.\nThe system is now ready for operation.");
    }
    else
        QMessageBox::warning(this, "Connection Failed", "Unable to establish connection with LMX2492EVM.\nPlease check the device and try again.");
}
void Jamming::configureLMX2492EVM24()
{
    if (hPipe == INVALID_HANDLE_VALUE)
    {
        QMessageBox::warning(this, "Configuration Failed", "Unable to apply configuration settings.\nPlease check the connection or parameters.");
        return;
    }
    if (ui.label_LMX2492EVM->text() == "Disconnected")
    {
        QMessageBox::warning(this, "Configuration Failed", "Unable to apply configuration settings.\nPlease check the connection or parameters.");
        return;
    }
    double crystalClock = 100;
    double StartFrequencyVCO = ((double)ui.spinBox_StartFrequencyVCO->value());
    double StopFrequencyVCO = ((double)ui.spinBox_StopFrequencyVCO->value());
    double SweepDuration = ((double)ui.spinBox_SweepDuration->value());
    // Calculate RAMP_LEN
    int ramp_len = SweepDuration * crystalClock;
    uint8_t ramp_len_low = (ramp_len >> 0) & 0xFF;
    uint8_t ramp_len_high = (ramp_len >> 8) & 0xFF;
    // Calculate RAMP_INC
    double ramp1 = StopFrequencyVCO - StartFrequencyVCO;
    ramp1 = ramp1 / crystalClock;
    ramp1 = ramp1 * 16777216;
    ramp1 = ramp1 / (SweepDuration * crystalClock);
    double ramp2 = 1073741824 - ramp1;
    int ramp1_int = static_cast<int>(round(ramp1));
    int ramp2_int = static_cast<int>(round(ramp2));

    uint8_t low = (ramp1_int >> 0) & 0xFF;
    uint8_t middle = (ramp1_int >> 8) & 0xFF;
    uint8_t high = (ramp1_int >> 16) & 0xFF;

    uint8_t lowD = (ramp2_int >> 0) & 0xFF;
    uint8_t middleD = (ramp2_int >> 8) & 0xFF;
    uint8_t highD = (ramp2_int >> 16) & 0xFF;
    uint8_t addD = (ramp2_int >> 24) & 0xFF;

    //Calculate PLL_N for Start Frequency and dividerFraction
    double pll_n = StartFrequencyVCO / crystalClock;
    int pll_n_int = static_cast<int>(pll_n);
    pll_n = pll_n - pll_n_int;
    int dividerFraction = round(pll_n * 16777216);
    uint8_t pll_n_low = pll_n_int & 0xFF;        // R16
    uint8_t pll_n_med = (pll_n_int >> 8) & 0xFF; // R17
    uint8_t FRACNum_low = (dividerFraction >> 0) & 0xFF;        // R19
    uint8_t FRACNum_middle = (dividerFraction >> 8) & 0xFF; // R20
    uint8_t FRACNum_high = (dividerFraction >> 16) & 0xFF; //21 
    // Calculate R82
    int ramp_Limit_high = StartFrequencyVCO / crystalClock;
    ramp_Limit_high = 25 - ramp_Limit_high;

    vector<pair<int, int>> spiCommands = {
        {0x0013, FRACNum_low},   // R19
        {0x0014, FRACNum_middle},   // R20
        {0x0015, FRACNum_high},  // R21
        {0x0010, pll_n_low},   // R16 - PLL_N low bits
        {0x0011, pll_n_med},  // R17 - PLL_N high bits
        {0x0052, ramp_Limit_high},   // R82
        {0x0056,  low},     // R86
        {0x0057,  middle},  // R87
        {0x0058,  high},    // R88
        {0x005D,  lowD},    // R93
        {0x005E,  middleD}, // R94
        {0x005F,  highD},    // R95
        {0x0060, addD },    // R96
        {0x005A,  ramp_len_low },     // R90
        {0x005B,  ramp_len_high},  // R91
        {0x0061,  ramp_len_low},     // R97
        {0x0062,  ramp_len_high}  // R98
        
    };
    for (const auto& [reg, val] : spiCommands) {
        stringstream ss;
        ss << "updateConfiguration " << hex << reg << " " << val;
        string command = ss.str();
        DWORD bytesWritten;
        WriteFile(hPipe, command.c_str(), command.length(), &bytesWritten, NULL);
        char buffer[128];
        DWORD bytesRead;
        ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL);
        buffer[bytesRead] = '\0';
        if (QString::fromStdString(buffer) == "ERROR:LMX2492EVM update register failed")
        {
            QMessageBox::warning(this, "Configuration Failed", "Unable to apply configuration settings.\nPlease check the connection or parameters.");
            return;
        }
    }
    QMessageBox::information(this, "Configuration Successful", "Configuration settings have been successfully applied.\nThe system is ready for operation.");
}
void Jamming::startTransmission24()
{
    if (hPipe == INVALID_HANDLE_VALUE) {
        QMessageBox::warning(this, "Transmission Failed", "Unable to start the transmission.\nPlease ensure the connection and configuration settings are correct.");
        return;
    }
    // Send a test command
    string command = "startTransmission24";
    DWORD bytesWritten;
    WriteFile(hPipe, command.c_str(), command.length(), &bytesWritten, NULL);
    // Read the response
    char buffer[128];
    DWORD bytesRead;
    ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL);
    buffer[bytesRead] = '\0';

    // Display the response
    if (QString::fromStdString(buffer) == "SUCCESS:LMX2492EVM transmission started")
    {
        QMessageBox::information(this, "Transmission Started", "Jamming signals are now broadcasting on the Wi-Fi 2.4 band.");
        ui.label_transmission24->setText("Transmitting");
        ui.label_transmission24->setStyleSheet("QLabel { color : #89CFF0; }");
        ui.pushButton_StartTransmissionWIFI24->setEnabled(false);
        ui.pushButton_StopTransmissionWIFI24->setEnabled(true);
    }
    else
        QMessageBox::warning(this, "Transmission Failed", "Unable to start the transmission.\nPlease ensure the connection and configuration settings are correct.");
}
void Jamming::stopTransmission24()
{
    if (hPipe == INVALID_HANDLE_VALUE) {
        QMessageBox::critical(this, "Stop Transmission Failed", "Unable to stop the transmission.\nPlease check the system status and try again.");
        return;
    }
    int reg = 0x0002; // R2
    int value = 5; // FOR RESET 5
    // Format command string
    stringstream ss;
    ss << "stopTransmission24 " << hex << reg << " " << value;
    string command = ss.str();
    // Send to helper process
    DWORD bytesWritten;
    WriteFile(hPipe, command.c_str(), command.length(), &bytesWritten, NULL);
    // Read back result
    char buffer[128];
    DWORD bytesRead;
    ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL);
    buffer[bytesRead] = '\0';
    if (QString::fromStdString(buffer) == "SUCCESS:LMX2492EVM transmission stoped")
    {
        QMessageBox::information(this, "Transmission Stoped", "Jamming signals have been successfully stopped on Wi-Fi 2.4 GHz.");
        ui.label_transmission24->setText("Ready For Operation");
        ui.label_transmission24->setStyleSheet("QLabel { color : green; }");
        ui.pushButton_StartTransmissionWIFI24->setEnabled(true);
        ui.pushButton_StopTransmissionWIFI24->setEnabled(false);
    }
    else
        QMessageBox::critical(this, "Stop Transmission Failed", "Unable to stop the transmission.\nPlease check the system status and try again.");
}

//LMX58
void Jamming::connectLMX2492EVM58()
{
    if (hPipe == INVALID_HANDLE_VALUE) {
        QMessageBox::warning(this, "Connection Failed", "Unable to establish connection with LMX2492EVM.\nPlease check the device and try again.");
        return;
    }
    // Send a test command
    string command = "connectLMX2492EVM58";
    DWORD bytesWritten;
    WriteFile(hPipe, command.c_str(), command.length(), &bytesWritten, NULL);

    // Read the response
    char buffer[128];
    DWORD bytesRead;
    ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL);
    buffer[bytesRead] = '\0';

    // Display the response
    if (QString::fromStdString(buffer) == "SUCCESS:LMX2492EVM58 connected")
    {
        ui.label_LMX2492EVM58->setText("Connected");
        ui.label_LMX2492EVM58->setStyleSheet("QLabel { color : green; }");
        ui.label_transmission58->setText("Ready For Operation");
        ui.label_transmission58->setStyleSheet("QLabel { color : green; }");
        ui.pushButton_StartTransmissionWIFI58->setEnabled(true);
        updateDiagram();
        QMessageBox::information(this, "Connection Successful", "LMX2492EVM has been successfully connected.\nThe system is now ready for operation.");

    }
    else
        QMessageBox::warning(this, "Connection Failed", "Unable to establish connection with LMX2492EVM.\nPlease check the device and try again.");

}
void Jamming::configureLMX2492EVM58()
{
    if (hPipe == INVALID_HANDLE_VALUE)
    {
        QMessageBox::warning(this, "Configuration Failed", "Unable to apply configuration settings.\nPlease check the connection or parameters.");        return;
        return;
    }
    if (ui.label_LMX2492EVM58->text() == "Disconnected")
    {
        QMessageBox::warning(this, "Configuration Failed", "Unable to apply configuration settings.\nPlease check the connection or parameters.");        return;
        return;
    }

    double crystalClock = 100;
    double StartFrequencyVCO = ((double)ui.spinBox_StartFrequencyVCO58->value());
    double StopFrequencyVCO = ((double)ui.spinBox_StopFrequencyVCO58->value());
    double SweepDuration = ((double)ui.spinBox_SweepDuration58->value());

    // ✅ Calculate RAMP_LEN
    int ramp_len = SweepDuration * crystalClock;
    uint8_t ramp_len_low = (ramp_len >> 0) & 0xFF;
    uint8_t ramp_len_high = (ramp_len >> 8) & 0xFF;

    // ✅ Calculate RAMP_INC
    double ramp1 = StopFrequencyVCO - StartFrequencyVCO;
    ramp1 = ramp1 / crystalClock;
    ramp1 = ramp1 * 16777216;
    ramp1 = ramp1 / (SweepDuration * crystalClock);
    double ramp2 = 1073741824 - ramp1;
    int ramp1_int = static_cast<int>(round(ramp1));
    int ramp2_int = static_cast<int>(round(ramp2));

    uint8_t low = (ramp1_int >> 0) & 0xFF;
    uint8_t middle = (ramp1_int >> 8) & 0xFF;
    uint8_t high = (ramp1_int >> 16) & 0xFF;

    uint8_t lowD = (ramp2_int >> 0) & 0xFF;
    uint8_t middleD = (ramp2_int >> 8) & 0xFF;
    uint8_t highD = (ramp2_int >> 16) & 0xFF;
    uint8_t addD = (ramp2_int >> 24) & 0xFF;

    // ✅ Calculate PLL_N for Start Frequency and dividerFraction
    double pll_n = StartFrequencyVCO / crystalClock;
    int pll_n_int = static_cast<int>(pll_n);
    pll_n = pll_n - pll_n_int;
    int dividerFraction = round(pll_n * 16777216);
    uint8_t pll_n_low = pll_n_int & 0xFF;        // R16
    uint8_t pll_n_med = (pll_n_int >> 8) & 0xFF; // R17


    uint8_t FRACNum_low = (dividerFraction >> 0) & 0xFF;        // R19
    uint8_t FRACNum_middle = (dividerFraction >> 8) & 0xFF; // R20
    uint8_t FRACNum_high = (dividerFraction >> 16) & 0xFF; //21 

    // ✅ Calculate R82
    int ramp_Limit_high = StartFrequencyVCO / crystalClock;
    ramp_Limit_high = 25 - ramp_Limit_high;


    vector<pair<int, int>> spiCommands = {
        {0x0013, FRACNum_low},   // R19
        {0x0014, FRACNum_middle},   // R20
        {0x0015, FRACNum_high},  // R21

        {0x0010, pll_n_low},   // R16 - PLL_N low bits
        {0x0011, pll_n_med},  // R17 - PLL_N high bits

        {0x0052, ramp_Limit_high},   // R82

        {0x0056,  low},     // R86
        {0x0057,  middle},  // R87
        {0x0058,  high},    // R88
        {0x005D,  lowD},    // R93
        {0x005E,  middleD}, // R94
        {0x005F,  highD},    // R95
        {0x0060, addD },    // R96

        {0x005A,  ramp_len_low },     // R90
        {0x005B,  ramp_len_high},  // R91
        {0x0061,  ramp_len_low},     // R97
        {0x0062,  ramp_len_high}  // R98

    };

    for (const auto& [reg, val] : spiCommands) {
        stringstream ss;
        ss << "updateConfiguration " << hex << reg << " " << val;
        string command = ss.str();

        DWORD bytesWritten;
        WriteFile(hPipe, command.c_str(), command.length(), &bytesWritten, NULL);

        // Read response
        char buffer[128];
        DWORD bytesRead;
        ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL);
        buffer[bytesRead] = '\0';
        if (QString::fromStdString(buffer) == "ERROR:LMX2492EVM58 update register failed")
        {
            QMessageBox::warning(this, "Configuration Failed", "Unable to apply configuration settings.\nPlease check the connection or parameters.");
            return;
        }
    }
    QMessageBox::information(this, "Configuration Successful", "Configuration settings have been successfully applied.\nThe system is ready for operation.");
}
void Jamming::startTransmission58()
{
    if (hPipe == INVALID_HANDLE_VALUE) {
        QMessageBox::warning(this, "Transmission Failed", "Unable to start the transmission.\nPlease ensure the connection and configuration settings are correct.");
        return;
    }
    // Send a test command
    string command = "startTransmission58";
    DWORD bytesWritten;
    WriteFile(hPipe, command.c_str(), command.length(), &bytesWritten, NULL);
    // Read the response
    char buffer[128];
    DWORD bytesRead;
    ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL);
    buffer[bytesRead] = '\0';

    // Display the response
    if (QString::fromStdString(buffer) == "SUCCESS:LMX2492EVM58 transmission started")
    {
        QMessageBox::information(this, "Transmission Started", "Jamming signals are now broadcasting on the Wi-Fi 5.8 GHz band.");
        ui.label_transmission58->setText("Transmitting");
        ui.label_transmission58->setStyleSheet("QLabel { color : #89CFF0; }");
        ui.pushButton_StartTransmissionWIFI58->setEnabled(false);
        ui.pushButton_StopTransmissionWIFI58->setEnabled(true);
    }
    else
        QMessageBox::warning(this, "Transmission Failed", "Unable to start the transmission.\nPlease ensure the connection and configuration settings are correct.");

}
void Jamming::stopTransmission58()
{
    if (hPipe == INVALID_HANDLE_VALUE) {
        QMessageBox::critical(this, "Stop Transmission Failed", "Unable to stop the transmission.\nPlease check the system status and try again.");
        return;
    }
    int reg = 0x0002; // R58
    int value = 5; // 0000 0101

    // Format command string
    stringstream ss;
    ss << "stopTransmission58 " << hex << reg << " " << value;
    string command = ss.str();

    // Send to helper process
    DWORD bytesWritten;
    WriteFile(hPipe, command.c_str(), command.length(), &bytesWritten, NULL);

    // Read back result
    char buffer[128];
    DWORD bytesRead;
    ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL);
    buffer[bytesRead] = '\0';

    if (QString::fromStdString(buffer) == "SUCCESS:LMX2492EVM58 transmission stoped")
    {
        QMessageBox::information(this, "Transmission Stoped", "Jamming signals have been successfully stopped on Wi-Fi 5.8 GHz.");
        ui.label_transmission58->setText("Ready For Operation");
        ui.label_transmission58->setStyleSheet("QLabel { color : green; }");
        ui.pushButton_StartTransmissionWIFI58->setEnabled(true);
        ui.pushButton_StopTransmissionWIFI58->setEnabled(false);
    }
    else
        QMessageBox::critical(this, "Stop Transmission Failed", "Unable to stop the transmission.\nPlease check the system status and try again.");

}

//GPS
void Jamming::connectLimeSDR()
{
    if (LMS_Open(&LimeSDR, nullptr, nullptr) != 0) {
        QMessageBox::warning(this, "Connection Failed", "Unable to establish connection with LimeSDR Mini 2.0.\nPlease check the device and try again.");
        return;
    }
        ui.label_LimeSDR->setText("Connected");
        ui.label_LimeSDR->setStyleSheet("QLabel { color : green; }");
        ui.label_transmissionGPS->setText("Ready For Operation");
        ui.label_transmissionGPS->setStyleSheet("QLabel { color : green; }");
        ui.pushButton_StartTransmissionGPS->setEnabled(true);
        updateDiagram();
        QMessageBox::information(this, "Connection Successful", "LimeSDR Mini 2.0 has been successfully connected.\nThe system is now ready for operation.");
}
void Jamming::configureLimeSDR()
{
    if (LimeSDR == nullptr) {
        QMessageBox::warning(this, "Configuration Failed", "Unable to apply configuration settings.\nPlease check the connection or parameters."); 
        return;
    }
    
    centerFreq = ((ui.spinBox_LIME_StopFreq->value() + ui.spinBox_LIME_StartFreq->value()) / 2) * 1e6;
    sampleRate = (ui.spinBox_LIME_StopFreq->value() - ui.spinBox_LIME_StartFreq->value()) * 1e6;
    tx_buffer = generateFilteredGaussianSignal();
    send_cnt = tx_buffer.size() / 2;
    QMessageBox::information(this, "Configuration Successful", "Configuration settings have been successfully applied.\nThe system is ready for operation.");
}
void Jamming::startTransmission()
{
    if (LMS_Init(LimeSDR) != 0) {
        QMessageBox::warning(this, "Transmission Failed", "Unable to start the transmission.\nPlease ensure the connection and configuration settings are correct.");
        return;
    }
    if (LMS_EnableChannel(LimeSDR, LMS_CH_TX, 0, true) != 0) {
        QMessageBox::warning(this, "Transmission Failed", "Unable to start the transmission.\nPlease ensure the connection and configuration settings are correct.");
        return;
    }
    if (LMS_SetSampleRate(LimeSDR, sampleRate, 0) != 0) {
        QMessageBox::warning(this, "Transmission Failed", "Unable to start the transmission.\nPlease ensure the connection and configuration settings are correct.");
        return;
    }
    if (LMS_SetLOFrequency(LimeSDR, LMS_CH_TX, 0, centerFreq) != 0) {
        QMessageBox::warning(this, "Transmission Failed", "Unable to start the transmission.\nPlease ensure the connection and configuration settings are correct.");
        return;
    }
    if (LMS_SetNormalizedGain(LimeSDR, LMS_CH_TX, 0, 0.9) != 0) {
        QMessageBox::warning(this, "Transmission Failed", "Unable to start the transmission.\nPlease ensure the connection and configuration settings are correct.");
        return;
    }
    if (LMS_Calibrate(LimeSDR, LMS_CH_TX, 0, sampleRate, 0) != 0) {
        QMessageBox::warning(this, "Transmission Failed", "Unable to start the transmission.\nPlease ensure the connection and configuration settings are correct.");
        return;
    }
    tx_stream.channel = 0;
    tx_stream.fifoSize = 512 * 1024;  // Increased FIFO size to support higher bandwidth
    tx_stream.throughputVsLatency = 0.5;
    tx_stream.dataFmt = lms_stream_t::LMS_FMT_F32;
    tx_stream.isTx = true;
    if (LMS_SetupStream(LimeSDR, &tx_stream) != 0) {
        QMessageBox::warning(this, "Transmission Failed", "Unable to start the transmission.\nPlease ensure the connection and configuration settings are correct.");
        return;
    }
    if (LMS_StartStream(&tx_stream) != 0) {
        QMessageBox::warning(this, "Transmission Failed", "Unable to start the transmission.\nPlease ensure the connection and configuration settings are correct.");
        return;
    }
    isTransmitting = true;
    if (!txThread) {
        txThread = new std::thread([this]() {
            while (isTransmitting) {
                sendData();
            }
            });
    }
    isTransmitting = true;
    QMessageBox::information(this, "Transmission Started", "Jamming signals are now broadcasting the GPS band.");
    ui.label_transmissionGPS->setText("Transmitting");
    ui.label_transmissionGPS->setStyleSheet("QLabel { color : #89CFF0; }");
    ui.pushButton_StartTransmissionGPS->setEnabled(false);
    ui.pushButton_StopTransmissionGPS->setEnabled(true);
}
void Jamming::stopTransmission()
{
    if (LMS_Init(LimeSDR) != 0) {
        QMessageBox::critical(this, "Stop Transmission Failed", "Unable to stop the transmission.\nPlease check the system status and try again.");
        return;
    }
    if (isTransmitting)
    {
        isTransmitting = false;
        if (txThread) {
            txThread->join();
            delete txThread;
            txThread = nullptr;
        }
        // Stop and destroy the stream
        LMS_StopStream(&tx_stream);
        LMS_DestroyStream(LimeSDR, &tx_stream);

        if (LMS_EnableChannel(LimeSDR, LMS_CH_TX, 0, false) != 0) {
            QMessageBox::critical(this, "Stop Transmission Failed", "Unable to stop the transmission.\nPlease check the system status and try again.");
        }
        QMessageBox::information(this, "Transmission Stoped", "Jamming signals have been successfully stopped on GPS band.");
        ui.label_transmissionGPS->setText("Ready For Operation");
        ui.label_transmissionGPS->setStyleSheet("QLabel { color : green; }");
        ui.pushButton_StartTransmissionGPS->setEnabled(true);
        ui.pushButton_StopTransmissionGPS->setEnabled(false);
    }
}
void Jamming::sendData()
{
    int ret = LMS_SendStream(&tx_stream, tx_buffer.data(), send_cnt, nullptr, 1000);
    if (ret <= 0) {
        stopTransmission();
    }
}


vector<float> Jamming::generateFilteredGaussianSignal()
{
    const int buffer_size = 16384;
    const float amplitude = 5.0f;
    const float cutoffFreq = 10e6;
    const float transitionWidth = 50e3;
    const int filter_size = 512;
    vector<float> noise(buffer_size * 2);
    //  Fix 1: Use Fixed Seed for Noise Stability
    default_random_engine generator(42);
    normal_distribution<float> distribution(0.0f, amplitude);
    for (int i = 0; i < buffer_size; i++) {
        noise[2 * i] = distribution(generator);
        noise[2 * i + 1] = distribution(generator);
    }

    vector<float> filter(filter_size);
    float beta = 6.76;

    for (int i = 0; i < filter_size; i++) {
        float n = (i - (filter_size - 1) / 2.0f);
        if (n == 0) {
            filter[i] = 2 * (cutoffFreq / sampleRate);
        }
        else {
            float sinc_val = sin(2 * M_PI * cutoffFreq * n / sampleRate) / (M_PI * n);
            float window_val = 0.54 - 0.46 * cos((2 * M_PI * i) / (filter_size - 1));
            filter[i] = sinc_val * window_val;
        }
    }
    // Fix 2: Ensure Filter Normalization
    float sum = 0;
    for (float v : filter) sum += v;
    if (sum != 0) {
        for (float& v : filter) v /= sum;
    }
    //  Fix 3: Avoid Power Loss During Filtering
    vector<float> filtered_noise(buffer_size * 2, 0);
    for (int i = 0; i < buffer_size; i++) {
        for (int j = 0; j < min(filter_size, i + 1); j++) {
            filtered_noise[2 * i] += noise[2 * (i - j)] * filter[j];
            filtered_noise[2 * i + 1] += noise[2 * (i - j) + 1] * filter[j];
        }
    }
    //  Fix 4: Use Phase Accumulator for Stable Modulation
    float phase = 0.0f;
    float phase_increment = 2.0f * M_PI * (centerFreq / sampleRate);
    for (int i = 0; i < buffer_size; i++) {
        float cosPhase = cos(phase);
        float sinPhase = sin(phase);

        float I = filtered_noise[2 * i];
        float Q = filtered_noise[2 * i + 1];

        filtered_noise[2 * i] = I * cosPhase - Q * sinPhase;
        filtered_noise[2 * i + 1] = I * sinPhase + Q * cosPhase;

        phase += phase_increment;
        if (phase > 2.0f * M_PI) phase -= 2.0f * M_PI;
    }
    return filtered_noise;
}
