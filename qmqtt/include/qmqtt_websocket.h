#ifndef QMQTT_WEBSOCKET_H
#define QMQTT_WEBSOCKET_H

#include <QWebSocket>
#include <QPointer>
#include <QBuffer>
#include "qmqtt_frame.h"

namespace QMQTT {

class NetworkWebSocket : public QObject
{
    Q_OBJECT
public:
    explicit NetworkWebSocket(QObject *parent = nullptr);
    ~NetworkWebSocket();
    void disconnect();
    void sendFrame(Frame & frame);

    bool isConnected();    

    QAbstractSocket::SocketState state() const;
signals:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError);
    void received(Frame &frame);
public slots:
    void connectTo(const QString & uri);
protected slots:
    void initSocket();
    void gotMsg(const QByteArray &msg);
private:
    QPointer<QWebSocket> _socket;
    int readRemaingLength(QDataStream &in);
    //read data
    QPointer<QBuffer> _buffer;
    quint8 _header;
    int _offsetBuf;
    int _leftSize;
};
}
#endif // QMQTT_WEBSOCKET_H
