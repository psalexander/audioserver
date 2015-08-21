#include "dialog.h"



Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    initGUI();

    int ErrorStatus = 0;
    HANDLE_AACENCODER hAacEncoder = NULL;
    if((ErrorStatus = aacEncOpen(&hAacEncoder,0,0))!=AACENC_OK){
        mLogServerEdit->append(tr("AAC encoder is NOT init!"));
    }else{
        mLogServerEdit->append(tr("AAC encoder is init!"));
    }

    ErrorStatus = aacEncoder_SetParam(hAacEncoder, parameter, value);

    ErrorStatus = aacEncEncode(hAacEncoder, NULL, NULL, NULL, NULL);
}

Dialog::~Dialog()
{
    if(mStartServerButton != NULL)
        delete mStartServerButton;
    if(mStopServerButton != NULL)
        delete mStopServerButton;
    if(mLogServerEdit != NULL)
        delete mLogServerEdit;
}

void Dialog::sessionOpened()
{
    if (networkSession) {
        QString id;
        id = "DefaultNetworkConfiguration";
    }

    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress::Any, 28333)) {
        QMessageBox::critical(this, tr("Server"), tr("Unable to start the server: %1.").arg(tcpServer->errorString()));
        mLogServerEdit->append(tr("Unable to start the server: %1.").arg(tcpServer->errorString()));
        return;
    }

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    mLogServerEdit->append(tr("The server is running on\n\nIP: %1\nport: %2\n").arg(ipAddress).arg(tcpServer->serverPort()));
}

void Dialog::initGUI()
{
    mLogServerEdit = new QTextEdit(this);
    mStartServerButton = new QPushButton(tr("Start"), this);
    mStopServerButton = new QPushButton(tr("Stop"), this);

    QHBoxLayout *main1Layout = new QHBoxLayout;

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(mStartServerButton);
    buttonLayout->addWidget(mStopServerButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(mLogServerEdit);
    mainLayout->addLayout(buttonLayout);


    QVBoxLayout *tableLayout = new QVBoxLayout;

    connectionsListWidget = new QListView;

    model = new SocketListModel;
    connectionsListWidget->setModel(model);

    tableLayout->addWidget(connectionsListWidget);
    textBrowser = new QTextBrowser;
    tableLayout->addWidget(textBrowser);

    messageLine = new QLineEdit;
    tableLayout->addWidget(messageLine);

    QHBoxLayout *actionButtonsLayout = new QHBoxLayout;
    QPushButton *startAudioButton = new QPushButton(tr("Start"), this);
    QPushButton *stopAudioButton = new QPushButton(tr("Stop"), this);
    QPushButton *sendMessageButton = new QPushButton(tr("Send"), this);
    actionButtonsLayout->addWidget(startAudioButton);
    actionButtonsLayout->addWidget(stopAudioButton);
    actionButtonsLayout->addWidget(sendMessageButton);

    connect(startAudioButton, &QPushButton::clicked, this, &Dialog::onClickStartAudioButton);
    connect(stopAudioButton, &QPushButton::clicked, this, &Dialog::onClickStopAudioButton);
    connect(sendMessageButton, &QPushButton::clicked, this, &Dialog::onClickSendMessageButton);

    tableLayout->addLayout(actionButtonsLayout);

    main1Layout->addLayout(mainLayout);
    main1Layout->addLayout(tableLayout);

    setLayout(main1Layout);

    connect(mStartServerButton, &QPushButton::clicked, this, &Dialog::onClickStartButton);
    connect(mStopServerButton, &QPushButton::clicked, this, &Dialog::onClickStopButton);
}

void Dialog::startServer()
{
    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        const QString id = "DefaultNetworkConfiguration";

        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) != QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }
        networkSession = new QNetworkSession(config, this);
        connect(networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

        mLogServerEdit->append(tr("Opening network session."));
        networkSession->open();
    } else {
        sessionOpened();
    }
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

void Dialog::stopServer()
{
    tcpServer->close();
}

void Dialog::onClickStartButton()
{
    mLogServerEdit->append(tr("Start server..."));
    startServer();
}

void Dialog::onClickStopButton()
{
    mLogServerEdit->append(tr("Stop server..."));
    stopServer();
}

void Dialog::onClickStartAudioButton()
{
    startCaptureAudio();
}

void Dialog::onClickStopAudioButton()
{
    stopCaptureAudio();
}

void Dialog::onClickSendMessageButton()
{
    QTcpSocket *s = socketsList.at(connectionsListWidget->currentIndex().row());
    s->write(messageLine->text().toUtf8());
    s->write(QString("\n").toUtf8());
    textBrowser->append(tr("Запрос %1 : %2").arg(s->peerAddress().toString()).arg(messageLine->text()));
}

void Dialog::onNewConnection()
{
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
//  connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));
    connect(clientConnection, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(clientConnection, SIGNAL(readyRead()), this, SLOT(onReadFromSocket()));

    mLogServerEdit->append(tr("Connected:"));
    mLogServerEdit->append(tr("Adress: %1").arg(clientConnection->peerAddress().toString()));
    mLogServerEdit->append(tr("Name: %1").arg(clientConnection->peerName()));
    mLogServerEdit->append(tr("Port: %1").arg(clientConnection->peerPort()));
    socketsList.append(clientConnection);
    model->setDatasList(socketsList);
}

void Dialog::onDisconnected()
{
    QTcpSocket *s = qobject_cast<QTcpSocket*>(sender());
    mLogServerEdit->append(tr("Disconnected Adress: %1").arg(s->peerAddress().toString()));

    socketsList.removeOne(s);
    model->setDatasList(socketsList);
    s->deleteLater();
}

void Dialog::onReadFromSocket()
{
//    mLogServerEdit->append(tr("onReadFromSocket"));
//    QTcpSocket *tcpSocket = (QTcpSocket*)sender();
//    QDataStream clientReadStream(tcpSocket);
//    QString str1;
//    clientReadStream >> str1;
//    mLogServerEdit->append(tr("Data1: %1").arg(str1));

    qint64 bytesExpected=0;
    QByteArray buffer;
    QTcpSocket *tcpSocket = (QTcpSocket*)sender();
//    if (bytesExpected == 0 && tcpSocket->bytesAvailable() >= sizeof(bytesExpected)) {
//        tcpSocket->read((char *)&bytesExpected, sizeof(bytesExpected));
//        qDebug() << "Expecting:" << &bytesExpected;
//    }

    if (/*bytesExpected > 0 &&*/ tcpSocket->bytesAvailable() > 0) {
        QByteArray chunk = tcpSocket->read(qMax(bytesExpected, tcpSocket->bytesAvailable()));
        buffer += chunk;
        bytesExpected -= chunk.size();
        QString res = buffer.data();
        res = res.trimmed();

        qDebug() << "res:" << res;
        textBrowser->append(tr("Ответ от %1 : %2").arg(tcpSocket->peerAddress().toString()).arg(res));
        return;

        qDebug() << "res:" << res;
        if(res.at(0)=='M' && res.at(res.size()-1)==';'){//Move

            QStringList ls = res.split(QRegExp("[,;]"));
            float x = ls.at(1).toDouble();
            float y = ls.at(2).toDouble();

        }else if(res.startsWith("CLICK")){
        }else if(res.startsWith("RC")){//right click
        }else if(res.startsWith("SU")){//scroll up
        }else if(res.startsWith("SD")){//scroll down
        }else if(res.startsWith("RD")){//right down
        }else if(res.startsWith("RU")){//right up
        }else if(res.startsWith("LD")){//left down
        }else if(res.startsWith("LU")){//left up
        }
        if (bytesExpected == 0) {
            deleteLater();
        }
    }

//    int next_block_size = 0;
//    while(true) {
//        if (!next_block_size) {
//            if (tcpSocket->bytesAvailable() < sizeof(quint16)) { // are size data available
//                break;
//            }
//            clientReadStream >> next_block_size;
//        }
//        if (tcpSocket->bytesAvailable() < next_block_size) {
//            break;
//        }
//        QString str;
//        clientReadStream >> str;
//        next_block_size = 0;
    //    }
}

void Dialog::startCaptureAudio()
{
    QAudioFormat format;
    format.setChannelCount(1);
    format.setSampleSize(8);
    format.setSampleRate(16000);
//    format.setCodec("audio/pcm");
    format.setCodec("audio/x-raw");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
        qWarning()<<"default format not supported try to use nearest";
        format = info.nearestFormat(format);
    }
    audioInput = new QAudioInput(format, this);
    QIODevice *dev = audioInput->start();
    connect(dev, SIGNAL(readyRead()), this, SLOT(readyReadAudioBuffer()));
}

void Dialog::stopCaptureAudio()
{
    audioInput->stop();
    delete audioInput;
}

void Dialog::readyReadAudioBuffer()
{
    QIODevice *dev = (QIODevice*)sender();
    qDebug()<< "Bytes ready:" << audioInput->bytesReady();
    QByteArray arr = dev->readAll();
    qDebug()<< "QByteArray:" << arr.count();
}

QString Dialog::getFdkAacErrorCode(AACENC_ERROR res)
{
    switch(res) {
        case AACENC_OK:
            return QStringLiteral("AACENC_OK");
        case AACENC_INVALID_HANDLE:
            return QStringLiteral("AACENC_INVALID_HANDLE");
        case AACENC_MEMORY_ERROR:
            return QStringLiteral("AACENC_MEMORY_ERROR");
        case AACENC_UNSUPPORTED_PARAMETER:
            return QStringLiteral("AACENC_UNSUPPORTED_PARAMETER");
        case AACENC_INVALID_CONFIG:
            return QStringLiteral("AACENC_INVALID_CONFIG");
        case AACENC_INIT_ERROR:
            return QStringLiteral("AACENC_INIT_ERROR");
        case AACENC_INIT_AAC_ERROR:
            return QStringLiteral("AACENC_INIT_AAC_ERROR");
        case AACENC_INIT_SBR_ERROR:
            return QStringLiteral("AACENC_INIT_SBR_ERROR");
        case AACENC_INIT_TP_ERROR:
            return QStringLiteral("AACENC_INIT_TP_ERROR");
        case AACENC_INIT_META_ERROR:
            return QStringLiteral("AACENC_INIT_META_ERROR");
        case AACENC_ENCODE_ERROR:
            return QStringLiteral("AACENC_ENCODE_ERROR");
        case AACENC_ENCODE_EOF:
            return QStringLiteral("AACENC_ENCODE_EOF");
        default:
            return QString.number((int)res);
    }
}
