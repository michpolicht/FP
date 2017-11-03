#ifndef TRANSITIONLISTMODEL_HPP
#define TRANSITIONLISTMODEL_HPP

#include "T1.hpp"

#include <QAbstractListModel>
#include <QList>

class T1ListModel:
	public QAbstractListModel
{
	Q_OBJECT

	public:
		T1ListModel(QObject * parent = 0);

		int rowCount(const QModelIndex & parent) const override;

		QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

		QHash<int, QByteArray> roleNames() const override;

		Q_INVOKABLE const T1 & at(int index) const;

		Q_INVOKABLE void append();

		Q_INVOKABLE void remove(int index);

		Q_INVOKABLE void setTemperature(int index, qreal temperature);

		Q_INVOKABLE void setEnthalpy(int index, qreal enthalpy);

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
	private:
		enum class Role : int {
			Temperature = Qt::UserRole,
			Enthalpy
		};

	private:
		QList<T1> m_list;
		QHash<int, QByteArray> m_roleNames;
};

#endif // TRANSITIONLISTMODEL_HPP
