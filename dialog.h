#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QTcpServer>
#include <QTcpSocket>
#include <QStringList>
#include <QNetworkSession>
#include <QNetworkConfigurationManager>
#include <QMessageBox>
#include <QEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <QPointF>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QListView>
#include <QLineEdit>
#include <QTextBrowser>
#include <QAudioInput>
#include <QAudioFormat>

#include "simulationmouseevent.h"
#include "socketlistmodel.h"
#include "aacenc_lib.h"

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = 0);
    ~Dialog();
private:
    QPushButton *mStartServerButton;
    QPushButton *mStopServerButton;
    QTextEdit *mLogServerEdit;
    QLineEdit *messageLine;

    QTcpServer *tcpServer;
    QStringList fortunes;

    QList<QTcpSocket *> socketsList;

    QNetworkSession *networkSession;
QTextBrowser *textBrowser;

    SocketListModel *model;
    QListView *connectionsListWidget;

    void sessionOpened();
    void sendFortune();

    QAudioInput *audioInput;

    void initGUI();
    void startServer();
    void stopServer();

    QString getFdkAacErrorCode(AACENC_ERROR res);
public slots:
    void onClickStartButton();
    void onClickStopButton();

    void onClickStartAudioButton();
    void onClickStopAudioButton();
    void onClickSendMessageButton();

    void onNewConnection();
    void onDisconnected();
    void onReadFromSocket();

    void startCaptureAudio();
    void stopCaptureAudio();

    void readyReadAudioBuffer();
};

#endif // DIALOG_H
