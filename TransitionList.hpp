#ifndef TRANSITIONLIST_HPP
#define TRANSITIONLIST_HPP

#include "Transition.hpp"

#include <QAbstractListModel>
#include <QList>
#include <QSignalMapper>

#include <memory>

class TransitionList:
	public QAbstractListModel
{
	Q_OBJECT

	public:
		TransitionList(QObject * parent = 0);

		~TransitionList() override;

		int rowCount(const QModelIndex & parent) const override;

		QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

		QHash<int, QByteArray> roleNames() const override;

		Q_INVOKABLE Transition * at(int index) const;

		Q_INVOKABLE void append();

		Q_INVOKABLE void remove(int index);

	protected slots:
		void dataChangedFromTransition(QObject * transition);

	private:
		enum class Role : int {
			NAME = Qt::UserRole,
			TEMPERATURE_BEGIN,
			TEMPERATURE_END,
			ENTHALPY
		};

	private:
		QList<Transition *> m_list;
		QHash<int, QByteArray> m_roleNames;
		QSignalMapper m_transitionSignalMapper;
};

#endif // TRANSITIONLISTMODEL_HPP
