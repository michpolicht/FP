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
		Q_PROPERTY(qreal temperatureBegin READ temperatureBegin WRITE setTemperatureBegin NOTIFY temperatureBeginChanged)
		Q_PROPERTY(qreal temperatureEnd READ temperatureEnd WRITE setTemperatureEnd NOTIFY temperatureEndChanged)
		Q_PROPERTY(qreal enthalpy READ enthalpy WRITE setEnthalpy NOTIFY enthalpyChanged)
		Q_PROPERTY(QString functionExpr READ functionExpr WRITE setFunctionExpr NOTIFY functionExprChanged)
		Q_PROPERTY(int samples READ samples WRITE setSamples NOTIFY samplesChanged)
		Q_PROPERTY(bool bAlign READ bAlign WRITE setBAlign NOTIFY bAlignChanged)
//		Q_PROPERTY(bool subtractBeginTemperature READ subtractBeginTemperature WRITE setSubtractBeginTemperature NOTIFY subtractBeginTemperatureChanged)
//		Q_PROPERTY(bool subtractEndTemperature READ subtractEndTemperature WRITE setSubtractEndTemperature NOTIFY subtractEndTemperatureChanged)
		Q_PROPERTY(qreal a READ a WRITE setA NOTIFY aChanged)
		Q_PROPERTY(qreal b READ b WRITE setB NOTIFY bChanged)
		Q_PROPERTY(QList<qreal> * x READ x NOTIFY xyChanged)
		Q_PROPERTY(QList<qreal> * y READ y NOTIFY xyChanged)
		Q_PROPERTY(QList<qreal> * t READ t NOTIFY cpTChanged)
		Q_PROPERTY(QList<qreal> * cp READ cp NOTIFY cpTChanged)

		explicit Transition(QObject * parent = 0);

		~Transition() override;

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

		Q_INVOKABLE void updateXY(const QString & expr, qreal spanX, int samples);

		Q_INVOKABLE qreal calculateA(qreal enthalpy, bool alignToZero) const;

	public slots:
		void deleteLater();

	signals:
		void temperatureBeginChanged();

		void temperatureEndChanged();

		void enthalpyChanged();

		void functionExprChanged();

		void samplesChanged();

		void bAlignChanged();

		void aChanged();

		void bChanged();

		void xyChanged();

		void cpTChanged();

	private:
		void connectUpdateSignals();

		void disconnectUpdateSignals();

		qreal calculateA() const;

	private:
		qreal m_temperatureBegin;
		qreal m_temperatureEnd;
		qreal m_enthalpy;
		QString m_functionExpr;
		int m_samples;
		bool m_bAlign;
		qreal m_a;
		qreal m_b;
		QList<qreal> m_t;
		QList<qreal> m_cp;
		QList<qreal> m_x;
		QList<qreal> m_y;
};

#endif
