#include "T1ListModel.hpp"

T1ListModel::T1ListModel(QObject * parent):
	QAbstractListModel(parent),
	m_roleNames(QAbstractListModel::roleNames())
{
	m_roleNames[static_cast<int>(Role::Temperature)] = "temperature";
	m_roleNames[static_cast<int>(Role::Enthalpy)] = "enthalpy";
}

int T1ListModel::rowCount(const QModelIndex & parent) const
{
	if (parent.isValid())
		return 0;
	return m_list.count();
}

QVariant T1ListModel::data(const QModelIndex & index, int role) const
{
	if (!index.isValid())
		return QVariant();
	else if (role == static_cast<int>(Role::Temperature))
		return m_list.at(index.row()).temperature;
	else if (role == static_cast<int>(Role::Enthalpy))
		return m_list.at(index.row()).enthalpy;

	 return QVariant();
}

QHash<int, QByteArray> T1ListModel::roleNames() const
{
	return m_roleNames;
}

const T1 & T1ListModel::at(int index) const
{
	Q_ASSERT(index >= 0 && index < m_list.count());

	return m_list.at(index);
}

void T1ListModel::append()
{
	beginInsertRows(QModelIndex(), m_list.count(), m_list.count());
	m_list.append(T1{0.0, 0.0});
	endInsertRows();
}

void T1ListModel::remove(int index)
{
	if (index >= m_list.count()) {
		qWarning(QString("remove(%1) - index is out of range").arg(index).toLocal8Bit().constData());
		return;
	}

	beginRemoveRows(QModelIndex(), index, index);
	m_list.removeAt(index);
	endRemoveRows();
}


void T1ListModel::setTemperature(int index, qreal temperature)
{
	Q_ASSERT(index >= 0 && index < m_list.count());

	m_list[index].temperature = temperature;

	emit dataChanged(QAbstractListModel::index(index, 0), QAbstractListModel::index(index, 0));
}

void T1ListModel::setEnthalpy(int index, qreal enthalpy)
{
	Q_ASSERT(index >= 0 && index < m_list.count());

	m_list[index].enthalpy = enthalpy;

	emit dataChanged(QAbstractListModel::index(index, 0), QAbstractListModel::index(index, 0));
}


//Qt::ItemFlags TransitionT1ListModel::flags(const QModelIndex & index) const
//{
//	if (index.isValid())
//		return (QAbstractListModel::flags(index) | Qt::ItemIsEditable);

//	return QAbstractListModel::flags(index);
//}

//bool TransitionT1ListModel::setData(const QModelIndex & index, const QVariant & value, int role)
//{
//	if (!index.isValid())
//		return false;
//	else if (role == static_cast<int>(Role::Temperature)) {
//		return m_list.at(index.row()).temperature;
//		emit dataChanged()
//		return true;
//	}
//	else if (role == static_cast<int>(Role::Enthalpy))
//		return m_list.at(index.row()).enthalpy;

//}
