#include "socketlistmodel.h"

SocketListModel::SocketListModel(QObject *parent) :QAbstractListModel(parent)
{

}

int SocketListModel::rowCount(const QModelIndex &parent) const
{
    return socketsList.count();
}

QVariant SocketListModel::data(const QModelIndex &index, int role) const
{
    if (role==Qt::DisplayRole) {
        return /*QVariant(*/socketsList.at(index.row())->peerAddress().toString()/*)*/;
    }else{
        return QVariant();
    }
}

void SocketListModel::setDatasList(QList<QTcpSocket *> list)
{
    socketsList.clear();
    socketsList.append(list);

    QModelIndex topLeft = index(0, 0);
    QModelIndex bottomRight = index(socketsList.count() - 1, 0);

    emit dataChanged(topLeft, bottomRight);
}
