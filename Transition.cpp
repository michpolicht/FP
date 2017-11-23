#include "Transition.hpp"
#include "functions.hpp"

#include <QJSEngine>
#include <QtDebug>
#include <QXYSeries>
#include <QQmlEngine>

#include <memory>

Transition::Transition(QObject * parent):
	QObject(parent),
	m_name(tr("Bez nazwy")),
	m_temperatureBegin(0.0),
	m_temperatureEnd(10.0),
	m_enthalpy(10.0),
	m_functionExpr("function(x) { return -x; }"),
	m_samples(10),
	m_bAlign(true),
	m_subtractBeingTemperature(false),
	m_subtractEndTemperature(false),
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

QString Transition::name() const
{
	return m_name;
}

void Transition::setName(const QString & name)
{
	if (m_name != name) {
		m_name = name;
		emit nameChanged();
	}
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

bool Transition::subtractBeginTemperature() const
{
	return m_subtractBeingTemperature;
}

void Transition::setSubtractBeginTemperature(bool flag)
{
	if (m_subtractBeingTemperature != flag) {
		m_subtractBeingTemperature = flag;
		emit subtractBeginTemperatureChanged();
	}
}

bool Transition::subtractEndTemperature() const
{
	return m_subtractEndTemperature;
}

void Transition::setSubtractEndTemperature(bool flag)
{
	if (m_subtractEndTemperature != flag) {
		m_subtractEndTemperature = flag;
		emit subtractEndTemperatureChanged();
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
	clone->m_name = m_name;
	clone->m_temperatureBegin = m_temperatureBegin;
	clone->m_temperatureEnd = m_temperatureEnd;
	clone->m_enthalpy = m_enthalpy;
	clone->m_functionExpr = m_functionExpr;
	clone->m_samples = m_samples;
	clone->m_bAlign = m_bAlign;
	clone->m_subtractBeingTemperature = m_subtractBeingTemperature;
	clone->m_subtractEndTemperature = m_subtractEndTemperature;
	clone->m_a = m_a;
	clone->m_b = m_b;
	clone->m_t = m_t;
	clone->m_cp = m_cp;

	return clone;
}

void Transition::copy(Transition * other)
{
	disconnectUpdateSignals();

	setName(other->m_name);
	setTemperatureBegin(other->m_temperatureBegin);
	setTemperatureEnd(other->m_temperatureEnd);
	setEnthalpy(other->m_enthalpy);
	setFunctionExpr(other->m_functionExpr);
	setSamples(other->m_samples);
	setBAlign(other->m_bAlign);
	setSubtractBeginTemperature(other->m_subtractBeingTemperature);
	setSubtractEndTemperature(other->m_subtractEndTemperature);
	m_a = other->m_a;
	emit aChanged();
	m_b = other->m_b;
	emit bChanged();
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

	if (bAlign()) {
		m_b = function.call({getSubtractedTemperature(temperatureEnd())}).toNumber();
	} else
		m_b = 0.0;
	emit bChanged();

	qreal t = getSubtractedTemperature(temperatureBegin());

	m_t.clear();
	m_cp.clear();
	for (int i = 0; i <= samples(); i++) {
		m_t.append(t);
		m_cp.append(function.call({t}).toNumber() - m_b);
		qDebug("appended point (%f, %f)", m_t.last(), m_cp.last());
		t += dT;
	}

	m_a = calculateA(m_t, m_cp);
	emit aChanged();

	for (int i = 0; i < m_t.count(); i++) {
		if (subtractBeginTemperature())
			m_t[i] += temperatureBegin();
		if (subtractEndTemperature())
			m_t[i] += temperatureEnd();
		m_cp[i] *= m_a;
	}

#ifndef QT_NO_DEBUG
	if (!qFuzzyCompare(::integrate(m_t, m_cp, m_t.first(), m_t.last()), enthalpy()))
		qWarning("Target enthalpy (%f) and integral (%f) do not match!", enthalpy(), ::integrate(m_t, m_cp, m_t.first(), m_t.last()));
#endif

	emit cpTChanged();
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
	connect(this, & Transition::subtractBeginTemperatureChanged, this, & Transition::update);
	connect(this, & Transition::subtractEndTemperatureChanged, this, & Transition::update);
}

void Transition::disconnectUpdateSignals()
{
	disconnect(this);
}

qreal Transition::calculateA(const QList<qreal> & x, const QList<qreal> & y) const
{
	if (x.isEmpty())
		return 0.0;

	qreal xyIntegral = ::integrate(x, y, x.first(), x.last());
	qDebug("calculateA integral = %f", xyIntegral);

	return enthalpy() / xyIntegral;
}

qreal Transition::getSubtractedTemperature(qreal t) const
{
	if (subtractBeginTemperature())
		t -= temperatureBegin();
	if (subtractEndTemperature())
		t -= temperatureEnd();
	return t;
}
