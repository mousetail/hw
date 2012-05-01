/*
 * Hedgewars, a free turn based strategy game
 * Copyright (c) 2004-2012 Andrey Korotaev <unC0Rr@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

/**
 * @file
 * @brief RoomsListModel class implementation
 */

#include "roomslistmodel.h"

#include <QIcon>

RoomsListModel::RoomsListModel(QObject *parent) :
    QAbstractTableModel(parent),
    c_nColumns(8)
{
    m_headerData =
    QStringList()
     << tr("In progress")
     << tr("Room Name")
     << tr("C")
     << tr("T")
     << tr("Owner")
     << tr("Map")
     << tr("Rules")
     << tr("Weapons");
}


QVariant RoomsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical || role != Qt::DisplayRole)
        return QVariant();
    else
        return QVariant(m_headerData.at(section));
}


int RoomsListModel::rowCount(const QModelIndex & parent) const
{
    if(parent.isValid())
        return 0;
    else
        return m_data.size();
}


int RoomsListModel::columnCount(const QModelIndex & parent) const
{
    if(parent.isValid())
        return 0;
    else
        return c_nColumns;
}


QVariant RoomsListModel::data(const QModelIndex &index, int role) const
{
    int column = index.column();
    int row = index.row();
    
    if (!index.isValid() || (row < 0)
            || (row >= m_data.size())
            || (column >= c_nColumns)
            || ((role != Qt::DecorationRole) && (role != Qt::DisplayRole))
       )
        return QVariant();

    // decorate room name based on room state
    if (role == Qt::DecorationRole)
    {
        if (column != 1)
            return QVariant();

        const QIcon roomBusyIcon(":/res/iconDamage.png");
        const QIcon roomWaitingIcon(":/res/iconTime.png");

        if (m_data.at(row).at(0).isEmpty())
            return QVariant(roomWaitingIcon);
        else
            return QVariant(roomBusyIcon);
    }

    QString content = m_data.at(row).at(column);

    if (column == 0)
        return QVariant(!content.isEmpty());

    return content;
}


void RoomsListModel::setRoomsList(const QStringList & rooms)
{
    beginResetModel();

    m_data.clear();

    int nRooms = rooms.size();

    for (int i = 0; i < nRooms; i += c_nColumns)
    {
        QStringList l;

#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
        l.reserve(c_nColumns);  // small optimisation not supported in old Qt
#endif

        for (int t = 0; t < c_nColumns; t++)
        {
            l.append(rooms[i + t]);
        }

        m_data.append(roomInfo2RoomRecord(l));
    }

    endResetModel();
}


void RoomsListModel::addRoom(const QStringList & info)
{
    beginInsertRows(QModelIndex(), 0, 0);

    m_data.prepend(roomInfo2RoomRecord(info));

    endInsertRows();
}


int RoomsListModel::rowOfRoom(const QString & name)
{
    int size = m_data.size();

    if (size < 1)
        return -1;

    int i = 0;

    // search for record with matching room name
    while(m_data[i].at(1) != name)
    {
        i++;
        if(i >= size)
            return -1;
    }

    return i;
}


void RoomsListModel::removeRoom(const QString & name)
{
    int i = rowOfRoom(name);

    if (i < 0)
        return;

    beginRemoveRows(QModelIndex(), i, i);

    m_data.removeAt(i);

    endRemoveRows();
}


void RoomsListModel::updateRoom(const QString & name, const QStringList & info)
{
    int i = rowOfRoom(name);

    if (i < 0)
        return;

    m_data[i] = roomInfo2RoomRecord(info);

    emit dataChanged(index(i, 0), index(i, columnCount(QModelIndex()) - 1));
}


QStringList RoomsListModel::roomInfo2RoomRecord(const QStringList & info)
{
    QStringList result;

    result = info;

    // for matters of less memory usage and quicker access store
    // the boolean string as either "t" or empty
    if (info[0].toLower() == "true")
        result[0] = "t";
    else
        result[0] = QString();

    return result;
}
