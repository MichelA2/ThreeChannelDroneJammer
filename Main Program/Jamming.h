#pragma once
#include <QtWidgets/QMainWindow>
#include <QTimer>
#include "ui_Jamming.h"
#include <lime/LimeSuite.h>
#include <vector>
#include <windows.h>   
#include <iostream>     
#include <thread>
#include <QProcess>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QPen>
#include <cmath>
#include <complex>
#include <QStackedWidget>
#include <QPixmap>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <qboxlayout.h>
using namespace std;

class Jamming : public QMainWindow
{
    Q_OBJECT

public:
    Jamming(QWidget *parent = nullptr);
    ~Jamming();

private slots:
    //SDR
    void connectLimeSDR();  // Connect to SDR
    void configureLimeSDR(); // Set frequency, gain, and sample rate
    void startTransmission();
    void stopTransmission();

    //usb2any_helper procces
    void connectusb2any_helper();

    //LMX24
    void connectLMX2492EVM24();
    void configureLMX2492EVM24();
    void startTransmission24();
    void stopTransmission24();

    //LMX58
    void connectLMX2492EVM58();
    void configureLMX2492EVM58();
    void startTransmission58();
    void stopTransmission58();

    //windows
    void openOperationalWindow();
    void openTecnicalWindow();
    void openGraphWindow();
    void openInformation();
    void returnToMain();

    //tips
    void functipGPS();
    void functip24();
    void functip58();

    //graph
    void TriangleSweep24();
    void FFT24();
    void TriangleSweep58();
    void FFT58();
    void noiseGPS();
    void FFTGPS();

private:
    Ui::JammingClass ui;

    //LimeSDR mini 2.0
    lms_device_t* LimeSDR; 
    lms_stream_t tx_stream ;
    vector<float> tx_buffer;
    bool isTransmitting;       
    std::thread* txThread; 
    int send_cnt;
    double centerFreq;
    double sampleRate;

    //LMX2492EVM
    QProcess* usb2anyProcess;
    HANDLE hPipe; // Store Named Pipe handle

    QChartView* ViewTriangle24;
    QChart* chartTriangle24;
    QChartView* ViewFFT24;
    QChart* chartFFT24;
    QChartView* ViewTriangle58;
    QChart* chartTriangle58;
    QChartView* ViewFFT58;
    QChart* chartFFT58;
    QChartView* ViewNoiseGPS;
    QChart* chartNoiseGPS;
    QChartView* ViewFFTGPS;
    QChart* chartFFTGPS;
    QLineSeries* seriesTriangle24;
    QLineSeries* seriesFFT24;
    QLineSeries* seriesTriangle58;
    QLineSeries* seriesFFT58;
    QLineSeries* seriesNoiseGPS;
    QLineSeries* seriesFFTGPS;

    //SDR
    void sendData();
    vector<float> generateFilteredGaussianSignal();
    //windows
    void drawChart();
    void updateDiagram();
};
