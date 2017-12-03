#ifndef TRANSITIONT_HPP
#define TRANSITIONT_HPP

#include <QtGlobal>
#include <QString>
#include <QList>
#include <QObject>

namespace QtCharts { class QAbstractSeries; }

class Transition:
		public QObject
{
	Q_OBJECT

	public:
		Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
		Q_PROPERTY(qreal temperatureBegin READ temperatureBegin WRITE setTemperatureBegin NOTIFY temperatureBeginChanged)
		Q_PROPERTY(qreal temperatureEnd READ temperatureEnd WRITE setTemperatureEnd NOTIFY temperatureEndChanged)
		Q_PROPERTY(qreal enthalpy READ enthalpy WRITE setEnthalpy NOTIFY enthalpyChanged)
		Q_PROPERTY(QString functionExpr READ functionExpr WRITE setFunctionExpr NOTIFY functionExprChanged)
		Q_PROPERTY(int samples READ samples WRITE setSamples NOTIFY samplesChanged)
		Q_PROPERTY(bool bAlign READ bAlign WRITE setBAlign NOTIFY bAlignChanged)
		Q_PROPERTY(bool subtractBeginTemperature READ subtractBeginTemperature WRITE setSubtractBeginTemperature NOTIFY subtractBeginTemperatureChanged)
		Q_PROPERTY(bool subtractEndTemperature READ subtractEndTemperature WRITE setSubtractEndTemperature NOTIFY subtractEndTemperatureChanged)
		Q_PROPERTY(qreal subtractCustomTemperature READ subtractCustomTemperature WRITE setSubtractCustomTemperature NOTIFY subtractCustomTemperatureChanged)
		Q_PROPERTY(qreal a READ a WRITE setA NOTIFY aChanged)
		Q_PROPERTY(qreal b READ b WRITE setB NOTIFY bChanged)
		Q_PROPERTY(QList<qreal> * t READ t NOTIFY cpTChanged)
		Q_PROPERTY(QList<qreal> * cp READ cp NOTIFY cpTChanged)

		explicit Transition(QObject * parent = 0);

		~Transition() override;

		QString name() const;

		void setName(const QString & name);

		qreal temperatureBegin() const;

		void setTemperatureBegin(qreal temperature);

		qreal temperatureEnd() const;

		void setTemperatureEnd(qreal temperature);

		qreal enthalpy() const;

		void setEnthalpy(qreal enthalpy);

		QString functionExpr() const;

		void setFunctionExpr(const QString & functionExpr);

		int samples() const;

		void setSamples(int samples);

		bool bAlign() const;

		bool subtractBeginTemperature() const;

		void setSubtractBeginTemperature(bool flag);

		bool subtractEndTemperature() const;

		void setSubtractEndTemperature(bool flag);

		qreal subtractCustomTemperature() const;

		void setSubtractCustomTemperature(qreal temperature);

		void setBAlign(bool bAlign);

		qreal a() const;

		void setA(qreal a);

		qreal b() const;

		void setB(qreal b);

		QList<qreal> * x();

		QList<qreal> * y();

		QList<qreal> * t();

		QList<qreal> * cp();

		Q_INVOKABLE Transition * clone() const;	// QObject does not allow direct copying.

		Q_INVOKABLE void copy(Transition * other); // QObject does not allow direct copying. Non-const pointer for QML.

//		Q_INVOKABLE void copy(const Transition & other); // QObject does not allow direct copying.

		Q_INVOKABLE bool isExprValid(const QString & expr) const;

		Q_INVOKABLE void updateSeries(QtCharts::QAbstractSeries * series);

		Q_INVOKABLE void update();

	public slots:
		void deleteLater();

	signals:
		void nameChanged();

		void temperatureBeginChanged();

		void temperatureEndChanged();

		void enthalpyChanged();

		void functionExprChanged();

		void samplesChanged();

		void bAlignChanged();

		void subtractBeginTemperatureChanged();

		void subtractEndTemperatureChanged();

		void subtractCustomTemperatureChanged();

		void aChanged();

		void bChanged();

		void cpTChanged();

	private:
		void connectUpdateSignals();

		void disconnectUpdateSignals();

		qreal calculateA(const QList<qreal> & x, const QList<qreal> & y) const;

		qreal getSubtractedTemperature(qreal t) const;

	private:
		QString m_name;
		qreal m_temperatureBegin;
		qreal m_temperatureEnd;
		qreal m_enthalpy;
		QString m_functionExpr;
		int m_samples;
		bool m_bAlign;
		bool m_subtractBeingTemperature;
		bool m_subtractEndTemperature;
		qreal m_subtractCustomTemperature;
		qreal m_a;
		qreal m_b;
		QList<qreal> m_t;
		QList<qreal> m_cp;
};

#endif
