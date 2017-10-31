#ifndef FILEREADER_HPP
#define FILEREADER_HPP

#include <QObject>
#include <QString>
#include <QList>

namespace QtCharts { class QAbstractSeries; }

class Model: public QObject
{
	Q_OBJECT

	public:
		Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
		Q_PROPERTY(QList<qreal> * t READ t NOTIFY tChanged)
		Q_PROPERTY(QList<qreal> * cp READ cp NOTIFY cpChanged)
		Q_PROPERTY(qreal integral READ integral NOTIFY integralChanged)

		explicit Model(QObject * parent = 0);

		QString source() const;

		void setSource(const QString & source);

		QList<qreal> * t();

		QList<qreal> * cp();

		qreal integral() const;

	signals:
		void sourceChanged();

		void tChanged();

		void cpChanged();

		void integralChanged();

	public slots:
		void parseCSV();

		void integrate();

        void updateSeries(QtCharts::QAbstractSeries * series);

	private:
		QString m_source;
		QList<qreal> m_t;
		QList<qreal> m_cp;
		qreal m_integral;
};

#endif // FILEREADER_HPP
