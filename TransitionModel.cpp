#include "TransitionModel.hpp"

TransitionModel::TransitionModel(QObject * parent):
	QAbstractListModel(parent),
	m_roleNames(QAbstractListModel::roleNames())
{
	m_roleNames[static_cast<int>(Role::Temperature)] = "temperature";
	m_roleNames[static_cast<int>(Role::Enthalpy)] = "enthalpy";
}

int TransitionModel::rowCount(const QModelIndex & parent) const
{
	if (parent.isValid())
		return 0;
	return m_list.count();
}

QVariant TransitionModel::data(const QModelIndex & index, int role) const
{
	if (!index.isValid())
		return QVariant();
	else if (role == static_cast<int>(Role::Temperature))
		return m_list.at(index.row()).t1;
	else if (role == static_cast<int>(Role::Enthalpy))
		return m_list.at(index.row()).h;

	 return QVariant();
}

QHash<int, QByteArray> TransitionModel::roleNames() const
{
	return m_roleNames;
}

const Transition & TransitionModel::at(int index) const
{
	Q_ASSERT(index >= 0 && index < m_list.count());

	return m_list.at(index);
}

void TransitionModel::append()
{
	beginInsertRows(QModelIndex(), m_list.count(), m_list.count());
	m_list.append(Transition{0.0, 0.0, 0.0});
	endInsertRows();
}

void TransitionModel::remove(int index)
{
	if (index >= m_list.count()) {
		qWarning(QString("remove(%1) - index is out of range").arg(index).toLocal8Bit().constData());
		return;
	}

	beginRemoveRows(QModelIndex(), index, index);
	m_list.removeAt(index);
	endRemoveRows();
}

void TransitionModel::setT1(int index, qreal temperature)
{
	Q_ASSERT(index >= 0 && index < m_list.count());

	m_list[index].t1 = temperature;

	emit dataChanged(QAbstractListModel::index(index, 0), QAbstractListModel::index(index, 0));
}

void TransitionModel::setT2(int index, qreal temperature)
{
	Q_ASSERT(index >= 0 && index < m_list.count());

	m_list[index].t2 = temperature;

	emit dataChanged(QAbstractListModel::index(index, 0), QAbstractListModel::index(index, 0));
}

void TransitionModel::setH(int index, qreal enthalpy)
{
	Q_ASSERT(index >= 0 && index < m_list.count());

	m_list[index].h = enthalpy;

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
