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

//		Q_INVOKABLE qreal temperatureBegin(int index) const;

//		Q_INVOKABLE void setTemperatureBegin(int index, qreal temperature);

//		Q_INVOKABLE qreal temperatureEnd(int index) const;

//		Q_INVOKABLE void setTemperatureEnd(int index, qreal temperature);

//		Q_INVOKABLE qreal enthalpy(int index) const;

//		Q_INVOKABLE void setEnthalpy(int index, qreal enthalpy);

//		Q_INVOKABLE qreal a(int index) const;

//		Q_INVOKABLE qreal b(int index) const;

//		Q_INVOKABLE int samples(int index) const;

//		Q_INVOKABLE QString functionExpr(int index) const;

//		Q_INVOKABLE void setFunctionExpr(int index, const QString & functionExpr);




//		Qt::ItemFlags flags(const QModelIndex & index) const override;

//		bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;

//		bool removeRows(int row, int count, const QModelIndex &parent) override;

//		bool dropMimeData(const QMimeData *data, Qt::DropAction action,
//						  int row, int column, const QModelIndex &parent) override;
//		QMimeData *mimeData(const QModelIndexList &indexes) const override;
//		QStringList mimeTypes() const override;
//		Qt::DropActions supportedDropActions() const override;

//		void addPiece(const QPixmap &pixmap, const QPoint &location);
//		void addPieces(const QPixmap& pixmap);

	protected slots:
		void dataChangedFromTransition(QObject * transition);

	private:
		enum class Role : int {
			TEMPERATURE_BEGIN = Qt::UserRole,
			TEMPERATURE_END,
			ENTHALPY
		};

	private:
		QList<Transition *> m_list;
		QHash<int, QByteArray> m_roleNames;
		QSignalMapper m_transitionSignalMapper;
};

#endif // TRANSITIONLISTMODEL_HPP
