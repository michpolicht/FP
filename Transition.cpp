#include "Transition.hpp"
#include "functions.hpp"

#include <QJSEngine>
#include <QtDebug>
#include <QXYSeries>
#include <QQmlEngine>

#include <memory>

Transition::Transition(QObject * parent):
	QObject(parent),
	m_temperatureBegin(0.0),
	m_temperatureEnd(10.0),
	m_enthalpy(10.0),
	m_functionExpr("function(x) { return -x; }"),
	m_samples(10),
	m_bAlign(true),
	m_a(qreal()),
	m_b(qreal())
{
	qDebug("Transition::Transition(QObject *) [%p]", this);
	update();
	connectUpdateSignals();
}

Transition::~Transition()
{
	qDebug("Transition::~Transition() [%p]", this);
}

qreal Transition::temperatureBegin() const
{
	return m_temperatureBegin;
}

void Transition::setTemperatureBegin(qreal temperature)
{
	if (m_temperatureBegin != temperature) {
		m_temperatureBegin = temperature;
		emit temperatureBeginChanged();
	}
}

qreal Transition::temperatureEnd() const
{
	return m_temperatureEnd;
}

void Transition::setTemperatureEnd(qreal temperature)
{
	if (m_temperatureEnd != temperature) {
		m_temperatureEnd = temperature;
		emit temperatureEndChanged();
	}
}

qreal Transition::enthalpy() const
{
	return m_enthalpy;
}

void Transition::setEnthalpy(qreal enthalpy)
{
	if (m_enthalpy != enthalpy) {
		m_enthalpy = enthalpy;
		emit enthalpyChanged();
	}
}

QString Transition::functionExpr() const
{
	return m_functionExpr;
}

void Transition::setFunctionExpr(const QString & functionExpr)
{
	if (m_functionExpr != functionExpr) {
		m_functionExpr = functionExpr;
		emit functionExprChanged();
	}
}

void Transition::setSamples(int samples)
{
	if (m_samples != samples) {
		m_samples = samples;
		emit samplesChanged();
	}
}

int Transition::samples() const
{
	return m_samples;
}

bool Transition::bAlign() const
{
	return m_bAlign;
}

void Transition::setBAlign(bool bAlign)
{
	if (m_bAlign != bAlign) {
		m_bAlign = bAlign;
		emit bAlignChanged();
	}
}

qreal Transition::a() const
{
	return m_a;
}

void Transition::setA(qreal a)
{
	if (m_a != a) {
		m_a = a;
		emit aChanged();
	}
}

qreal Transition::b() const
{
	return m_b;
}

void Transition::setB(qreal b)
{
	if (m_b != b) {
		m_b = b;
		emit bChanged();
	}
}

QList<qreal> * Transition::x()
{
	return & m_x;
}

QList<qreal> * Transition::y()
{
	return & m_y;
}

QList<qreal> * Transition::t()
{
	return & m_t;
}

QList<qreal> * Transition::cp()
{
	return & m_cp;
}

Transition * Transition::clone() const
{
	qDebug("Transition::clone() [%p]", this);
	Transition * clone = new Transition;
	clone->m_temperatureBegin = m_temperatureBegin;
	clone->m_temperatureEnd = m_temperatureEnd;
	clone->m_enthalpy = m_enthalpy;
	clone->m_functionExpr = m_functionExpr;
	clone->m_samples = m_samples;
	clone->m_bAlign = m_bAlign;
	clone->m_a = m_a;
	clone->m_b = m_b;
	clone->m_x = m_x;
	clone->m_y = m_y;
	clone->m_t = m_t;
	clone->m_cp = m_cp;

	return clone;
}

void Transition::copy(Transition * other)
{
	disconnectUpdateSignals();

	setTemperatureBegin(other->m_temperatureBegin);
	setTemperatureEnd(other->m_temperatureEnd);
	setEnthalpy(other->m_enthalpy);
	setFunctionExpr(other->m_functionExpr);
	setSamples(other->m_samples);
	setBAlign(other->m_bAlign);
	m_a = other->m_a;
	emit aChanged();
	m_b = other->m_b;
	emit bChanged();
	m_x = other->m_x;
	m_y = other->m_y;
	emit xyChanged();
	m_t = other->m_t;
	m_cp = other->m_cp;
	emit cpTChanged();

	connectUpdateSignals();
}

bool Transition::isExprValid(const QString & expr) const
{
	return !QJSEngine().evaluate(expr).isError();
}

void Transition::updateSeries(QtCharts::QAbstractSeries * series)
{
	if (series) {
		QtCharts::QXYSeries * xySeries = static_cast<QtCharts::QXYSeries *>(series);
		xySeries->clear();
		// note: QList::size() returns int...
		for (int i = 0; i < m_t.size(); i++)
			xySeries->append(m_t.at(i), m_cp.at(i));
	}
}

void Transition::update()
{
	m_x.clear();
	m_y.clear();

	QJSEngine engine;
	QJSValue function = engine.evaluate(functionExpr());
	if (function.isError()) {
		qWarning("Invalid expression!");
		return;
	}

	qreal temperatureRange = temperatureEnd() - temperatureBegin();
	qreal dT = temperatureRange / samples();
	qDebug("dT = %f", dT);
	/// @todo Qt has wrong implementation of machine epsilon... :/
	if (qFuzzyIsNull(dT)) {
		qWarning("dT is too small!");
		return;
	}

	if (bAlign())
		m_b = function.call({temperatureEnd()}).toNumber();
	else
		m_b = 0.0;
	emit bChanged();

	qreal x = temperatureBegin();
	for (int i = 0; i <= samples(); i++) {
		m_x.append(x);
		m_y.append(function.call({x}).toNumber() - m_b);
		qDebug("appended point (%f, %f)", m_x.last(), m_y.last());
		x += dT;
	}
//	emit xyChanged();

	m_a = calculateA();
	emit aChanged();

	m_t.clear();
	m_cp.clear();
	qreal t = temperatureBegin();
	for (int i = 0; i < m_x.count(); i++) {
		m_t.append(t);
		m_cp.append(m_y.at(i) * m_a);
		t += dT;
	}
	emit xyChanged();
	emit cpTChanged();

#ifndef QT_NO_DEBUG
	if (!qFuzzyCompare(::integrate(m_t, m_cp, m_t.first(), m_t.last()), enthalpy()))
		qWarning("Target enthalpy (%f) and integral (%f) do not match!", enthalpy(), ::integrate(m_t, m_cp, m_t.first(), m_t.last()));
#endif
}

void Transition::updateXY(const QString & expr, qreal spanX, int samples)
{
	m_x.clear();
	m_y.clear();

	QJSEngine engine;
//	QJSValue function = engine.evaluate("function(x) { return x; }");
	qDebug() << expr;
	QJSValue function = engine.evaluate(expr);
	if (function.isError()) {
		qWarning("Invalid expression!");
		return;
	}

//	qDebug("error: % d", function.isError());
//	qreal dx = (temperatureEnd() - temperatureBegin()) / samples;

	qreal dx = spanX / samples;
	qDebug("dx = %f", dx);
	// @todo Qt has wrong implementation of machine epsilon... :/
	if (qFuzzyIsNull(dx)) {
		qWarning("dx is too small!");
		return;
	}

	qreal x = 0.0;
	for (int i = 0; i <= samples; i++) {
		m_x.append(x);
		m_y.append(function.call({x}).toNumber());
		qDebug("appended point (%f, %f)", m_x.last(), m_y.last());
		x += dx;
	}

	m_b = m_y.last();
	emit bChanged();

	emit xyChanged();
//	m_t.clear();
//	m_cp.clear();
//	qreal dT = (temperatureEnd() - temperatureBegin()) / samples;
//	qreal t = temperatureBegin();
//	for (int i = 0; i < samples; i++) {
//		m_t.append(t);
//		m_cp.append(function.call({t}));
//		t += dT;
//	}
}

qreal Transition::calculateA(qreal enthalpy, bool alignToZero) const
{
	if (m_x.isEmpty())
		return 0.0;

	QList<qreal> y = m_y;
	if (alignToZero)
		for (int i = 0; i < y.count(); i++)
			y[i] -= m_b;

	qreal xyIntegral = ::integrate(m_x, y, m_x.first(), m_x.last());
	qDebug("xyIntegral = %f", xyIntegral);

	return enthalpy / xyIntegral;
}

void Transition::deleteLater()
{
	QObject::deleteLater();
}

void Transition::connectUpdateSignals()
{
	connect(this, & Transition::temperatureBeginChanged, this, & Transition::update);
	connect(this, & Transition::temperatureEndChanged, this, & Transition::update);
	connect(this, & Transition::enthalpyChanged, this, & Transition::update);
	connect(this, & Transition::functionExprChanged, this, & Transition::update);
	connect(this, & Transition::samplesChanged, this, & Transition::update);
	connect(this, & Transition::bAlignChanged, this, & Transition::update);
}

void Transition::disconnectUpdateSignals()
{
	disconnect(this);
}

qreal Transition::calculateA() const
{
	if (m_x.isEmpty())
		return 0.0;

	qreal xyIntegral = ::integrate(m_x, m_y, m_x.first(), m_x.last());
	qDebug("xyIntegral = %f", xyIntegral);

	return enthalpy() / xyIntegral;
}
