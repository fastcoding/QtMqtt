#include "qmqtt_websocket.h"
#include <QIODevice>
#include <QDataStream>
namespace QMQTT {

NetworkWebSocket::NetworkWebSocket(QObject *parent) : QObject(parent)
{
    _buffer = new QBuffer(this);
    _offsetBuf = 0;
    _leftSize = 0;
    initSocket();
}

void NetworkWebSocket::initSocket()
{
    if(_socket) {
        _socket->abort();
        delete _socket;
    }

   _socket = new QWebSocket(QString(),QWebSocketProtocol::VersionLatest,this);
   connect(_socket, SIGNAL(connected()), this, SIGNAL(connected()));
   connect(_socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
   connect(_socket, SIGNAL(binaryMessageReceived(const QByteArray &)), this, SLOT(gotMsg(const QByteArray &)));
   connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(error(QAbstractSocket::SocketError)));
}

NetworkWebSocket::~NetworkWebSocket()
{
    disconnect();
}

bool NetworkWebSocket::isConnected()
{
    return _socket->state()==QAbstractSocket::ConnectedState;
}

void NetworkWebSocket::connectTo(const QString & uri)
{
    if(!_socket)
    {
        qWarning("AMQP: Socket didn't create.");
        return;
    }
    qDebug() << "Connect to" << uri;
    _socket->open(uri);
}

void NetworkWebSocket::sendFrame(Frame & frame)
{

    if(_socket->state() == QAbstractSocket::ConnectedState)
    {
        QByteArray ba;
        {
            QDataStream stream(&ba,QIODevice::WriteOnly);
            frame.write(stream);
        }
        _socket->sendBinaryMessage(ba);
    }
}

void NetworkWebSocket::disconnect()
{
    if(_socket) _socket->close();
}

QAbstractSocket::SocketState NetworkWebSocket::state() const
{
    if(_socket)
    {
        return _socket->state();
    } else {
        return QAbstractSocket::UnconnectedState;
    }
}


int NetworkWebSocket::readRemaingLength(QDataStream &in)
{
     qint8 byte;
     int len = 0;
     int mul = 1;
     do {
         in >> byte;
         len += (byte & 127) * mul;
         mul *= 128  ;
     } while ((byte & 128) != 0);
     return len;
}

void NetworkWebSocket::gotMsg(const QByteArray&msg)
{
   // qDebug("sockReadReady...");
    QDataStream in(msg);
    QDataStream out(_buffer);

    if(_leftSize == 0)
    {
        _leftSize  = 0;
        _offsetBuf = 0;

        in >> _header;
        _leftSize = readRemaingLength(in);
    }
    QByteArray data;
    data.resize(_leftSize);
    _offsetBuf = in.readRawData(data.data(), data.size());
    _leftSize -= _offsetBuf;
    out.writeRawData(data.data(), _offsetBuf);
    if(_leftSize == 0)
    {
        _buffer->reset();
        Frame frame(_header, _buffer->buffer());
        _buffer->buffer().clear();
       // qDebug("network emit received(frame), header: %d", _header);
        emit received(frame);
    }else if (_leftSize<0){
        qDebug()<<"wrong protocol data detected!";
    }
}
}
