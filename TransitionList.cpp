#include "TransitionList.hpp"

#include <QQmlEngine>

TransitionList::TransitionList(QObject * parent):
	QAbstractListModel(parent),
	m_roleNames(QAbstractListModel::roleNames())
{
	m_roleNames[static_cast<int>(Role::TEMPERATURE_BEGIN)] = "temperatureBegin";
	m_roleNames[static_cast<int>(Role::TEMPERATURE_END)] = "temperatureEnd";
	m_roleNames[static_cast<int>(Role::ENTHALPY)] = "enthalpy";
	connect(& m_transitionSignalMapper, SIGNAL(mapped(QObject *)), this, SLOT(dataChangedFromTransition(QObject *)));
}

TransitionList::~TransitionList()
{
	while (!m_list.isEmpty())
		delete m_list.takeFirst();
}

int TransitionList::rowCount(const QModelIndex & parent) const
{
	if (parent.isValid())
		return 0;
	return m_list.count();
}

QVariant TransitionList::data(const QModelIndex & index, int role) const
{
	if (!index.isValid())
		return QVariant();
	else if (role == static_cast<int>(Role::TEMPERATURE_BEGIN))
		return m_list.at(index.row())->temperatureBegin();
	else if (role == static_cast<int>(Role::TEMPERATURE_END))
		return m_list.at(index.row())->temperatureEnd();
	else if (role == static_cast<int>(Role::ENTHALPY))
		return m_list.at(index.row())->enthalpy();

	 return QVariant();
}

QHash<int, QByteArray> TransitionList::roleNames() const
{
	return m_roleNames;
}

Transition * TransitionList::at(int index) const
{
	Q_ASSERT(index >= 0 && index < m_list.count());

	return m_list.at(index);
}

void TransitionList::append()
{
	beginInsertRows(QModelIndex(), m_list.count(), m_list.count());
	Transition * newTransition = new Transition;
	QQmlEngine::setObjectOwnership(newTransition, QQmlEngine::CppOwnership);
	connect(newTransition, SIGNAL(temperatureBeginChanged()), & m_transitionSignalMapper, SLOT(map()));
	connect(newTransition, SIGNAL(temperatureEndChanged()), & m_transitionSignalMapper, SLOT(map()));
	connect(newTransition, SIGNAL(enthalpyChanged()), & m_transitionSignalMapper, SLOT(map()));
	connect(newTransition, SIGNAL(functionExprChanged()), & m_transitionSignalMapper, SLOT(map()));
	connect(newTransition, SIGNAL(samplesChanged()), & m_transitionSignalMapper, SLOT(map()));
	connect(newTransition, SIGNAL(aChanged()), & m_transitionSignalMapper, SLOT(map()));
	connect(newTransition, SIGNAL(bChanged()), & m_transitionSignalMapper, SLOT(map()));
	connect(newTransition, SIGNAL(cpTChanged()), & m_transitionSignalMapper, SLOT(map()));
	m_transitionSignalMapper.setMapping(newTransition, newTransition);
	m_list.append(newTransition);
	endInsertRows();
}

void TransitionList::remove(int index)
{
	if (index >= m_list.count()) {
		qWarning(QString("remove(%1) - index is out of range").arg(index).toLocal8Bit().constData());
		return;
	}

	beginRemoveRows(QModelIndex(), index, index);
	delete m_list.takeAt(index);
	endRemoveRows();
}

//qreal TransitionList::temperatureBegin(int index) const
//{
//	Q_ASSERT(index >= 0 && index < m_list.count());

//	return m_list.at(index)->temperatureBegin();
//}

//void TransitionList::setTemperatureBegin(int index, qreal temperature)
//{
//	Q_ASSERT(index >= 0 && index < m_list.count());

//	m_list[index]->setTemperatureBegin(temperature);

//	emit dataChanged(QAbstractListModel::index(index, 0), QAbstractListModel::index(index, 0));
//}

//qreal TransitionList::temperatureEnd(int index) const
//{
//	Q_ASSERT(index >= 0 && index < m_list.count());

//	return m_list.at(index)->temperatureEnd();
//}

//void TransitionList::setTemperatureEnd(int index, qreal temperature)
//{
//	Q_ASSERT(index >= 0 && index < m_list.count());

//	m_list[index]->setTemperatureEnd(temperature);

//	emit dataChanged(QAbstractListModel::index(index, 0), QAbstractListModel::index(index, 0));
//}

//qreal TransitionList::enthalpy(int index) const
//{
//	Q_ASSERT(index >= 0 && index < m_list.count());

//	return m_list.at(index)->enthalpy();
//}

//void TransitionList::setEnthalpy(int index, qreal enthalpy)
//{
//	Q_ASSERT(index >= 0 && index < m_list.count());

//	m_list[index]->setEnthalpy(enthalpy);

//	emit dataChanged(QAbstractListModel::index(index, 0), QAbstractListModel::index(index, 0));
//}

//qreal TransitionList::a(int index) const
//{
//	Q_ASSERT(index >= 0 && index < m_list.count());

//	return m_list.at(index)->a();
//}

//qreal TransitionList::b(int index) const
//{
//	Q_ASSERT(index >= 0 && index < m_list.count());

//	return m_list.at(index)->b();
//}

//int TransitionList::samples(int index) const
//{
//	Q_ASSERT(index >= 0 && index < m_list.count());

//	return m_list.at(index)->samples();
//}

//QString TransitionList::functionExpr(int index) const
//{
//	Q_ASSERT(index >= 0 && index < m_list.count());

//	return m_list.at(index)->functionExpr();
//}

//void TransitionList::setFunctionExpr(int index, const QString & functionExpr)
//{
//	Q_ASSERT(index >= 0 && index < m_list.count());

//	m_list[index]->setFunctionExpr(functionExpr);

//	emit dataChanged(QAbstractListModel::index(index, 0), QAbstractListModel::index(index, 0));
//}

void TransitionList::dataChangedFromTransition(QObject * transition)
{
	int index = m_list.indexOf(static_cast<Transition * >(transition));

	Q_ASSERT(index != -1);

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
