#include "mytcpserver.h"
#include <QDebug>
#include <QCoreApplication>

MyTcpServer::~MyTcpServer()
{
    mTcpServer->close();
    server_status = 0;
}

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent){
    mTcpServer = new QTcpServer(this);
    connect(mTcpServer, &QTcpServer::newConnection,
            this, &MyTcpServer::slotNewConnection);

    if(!mTcpServer->listen(QHostAddress::Any, 33333)){
        qDebug() << "Server is not online";
    }
    else {
        server_status = 1;
        qDebug() << "Server online";
    }
}

void MyTcpServer::slotNewConnection(){
    if(server_status == 1)
    {
        mTcpSocket = mTcpServer -> nextPendingConnection();
        int userid_soc = mTcpSocket -> socketDescriptor();
        active_clients[mTcpSocket] = userid_soc;
        qDebug() << "New connection -" << userid_soc;
        connect(active_clients.key(userid_soc), SIGNAL(readyRead()),
                this, SLOT(slotServerRead()));
        connect(active_clients.key(userid_soc), SIGNAL(disconnected()),
                this, SLOT(slotClientDisconnected()));
        foreach(QTcpSocket* i, active_clients.keys())
        {
            qDebug() << i
                        << active_clients[i];
        }
    }
}

void MyTcpServer::slotServerRead(){
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    QString res = "";
    while(clientSocket->bytesAvailable()>0)
    {
        res = clientSocket -> readLine();
    }
    //qDebug() << clientSocket << "called write()";
    clientSocket -> write(QTime::currentTime().toString().toUtf8() + ' ' + res.toUtf8());
}

void MyTcpServer::slotClientDisconnected(){
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    qDebug() << clientSocket << "disconnected\n";
    active_clients.remove(clientSocket);
    clientSocket -> close();
//    if(active_clients.isEmpty())
//    {
//        qDebug() << "Server is shutting down...";
//        mTcpServer->~QTcpServer();
//    }
}