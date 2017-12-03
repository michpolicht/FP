#ifndef FILEREADER_HPP
#define FILEREADER_HPP

#include "TransitionList.hpp"

#include <QObject>
#include <QString>
#include <QList>

#include <memory>

namespace QtCharts { class QAbstractSeries; }

class Model:
	public QObject
{
	Q_OBJECT

	public:
		Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
		Q_PROPERTY(qreal baseEnthalpy READ baseEnthalpy WRITE setBaseEnthalpy NOTIFY baseEnthalpyChanged)
		Q_PROPERTY(QList<qreal> * t READ t NOTIFY cpTChanged)
		Q_PROPERTY(QList<qreal> * cp READ cp NOTIFY cpTChanged)
		Q_PROPERTY(qreal integral READ integral NOTIFY integralChanged)
		Q_PROPERTY(qreal minT READ minT NOTIFY minTChanged)
		Q_PROPERTY(qreal maxT READ maxT NOTIFY maxTChanged)
		Q_PROPERTY(qreal minCp READ minCp NOTIFY minCpChanged)
		Q_PROPERTY(qreal maxCp READ maxCp NOTIFY maxCpChanged)
		Q_PROPERTY(QAbstractListModel * transitionList READ transitionList CONSTANT)

		explicit Model(QObject * parent = 0);

		QString source() const;

		void setSource(const QString & source);

		qreal baseEnthalpy() const;

		void setBaseEnthalpy(qreal enthalpy);

		QList<qreal> * t();

		QList<qreal> * cp();

		qreal integral() const;

		qreal minT() const;

		qreal maxT() const;

		qreal minCp() const;

		qreal maxCp() const;

		QAbstractListModel * transitionList() const;

	public slots:
		void setSourceAsUrl(const QString & url);

		void parseCSV();

		void integrate(qreal from, qreal to);

        void updateSeries(QtCharts::QAbstractSeries * series);

		void updateCumulativeSeries(QtCharts::QAbstractSeries * series);

	signals:
		void sourceChanged();

		void baseEnthalpyChanged();

		void cpTChanged();

		void integralChanged();

		void minTChanged();

		void maxTChanged();

		void minCpChanged();

		void maxCpChanged();

	private:
		class CumulativeLessThan
		{
			public:
				bool operator()(std::pair<qreal, qreal> p1, std::pair<qreal, qreal> p2) const;

			private:
				const QList<qreal> * m_t;
		};

	private:
		void findMinMaxT();

		void findMinMaxCp();

		qreal integrateTransitions(qreal from, qreal to) const;

		qreal trapezePoint(qreal t1, qreal t2, qreal base, qreal pos) const;

	private:
		QString m_source;
		qreal m_baseEnthalpy;
		QList<qreal> m_t;
		QList<qreal> m_cp;
		qreal m_integral;
		qreal m_minT;
		qreal m_maxT;
		qreal m_minCp;
		qreal m_maxCp;
		std::unique_ptr<TransitionList> m_transitionList;
		QList<std::pair<qreal, qreal>> m_cumulative;
};

#endif // FILEREADER_HPP
