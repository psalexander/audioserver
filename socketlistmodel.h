#ifndef SOCKETLISTMODEL_H
#define SOCKETLISTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QList>
#include <QTcpSocket>
#include <QHostAddress>

class SocketListModel : public QAbstractListModel
{
    Q_OBJECT
public:
//    explicit SocketListModel(QObject *parent = 0);
    SocketListModel(QObject *parent = 0);


signals:

public slots:

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    QList<QTcpSocket *> socketsList;
    void setDatasList(QList<QTcpSocket *> list);
};

#endif // SOCKETLISTMODEL_H
