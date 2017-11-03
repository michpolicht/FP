#include "Model.hpp"

#include <QFile>
#include <QDir>
#include <QtDebug>
#include <QXYSeries>

#include <exception>

Model::Model(QObject * parent):
	QObject(parent),
	m_integral(0.0),
	m_minT(0.0),
	m_maxT(0.0),
	m_minCp(0.0),
	m_maxCp(0.0),
	m_t1List(new T1ListModel)
{
//	connect(this, & Model::sourceChanged, this, & Model::parseCSV);
}

QString Model::source() const
{
	return m_source;
}

void Model::setSource(const QString & source)
{
	if (m_source != source) {
		m_source = source;
		emit sourceChanged();
	}
}

QList<qreal> * Model::t()
{
	return & m_t;
}

QList<qreal> * Model::cp()
{
	return & m_cp;
}

qreal Model::integral() const
{
	return m_integral;
}

qreal Model::minT() const
{
	return m_minT;
}

qreal Model::maxT() const
{
	return m_maxT;
}

qreal Model::minCp() const
{
	return m_minCp;
}

qreal Model::maxCp() const
{
	return m_maxCp;
}

QAbstractListModel * Model::t1List() const
{
	return m_t1List.get();
}

void Model::setSourceAsUrl(const QString & url)
{
	setSource(QUrl(url).toLocalFile());
}

void Model::parseCSV()
{
	static const char * COULD_NOT_CONVERT_ARG = "Could not convert '%1' to double";

	qDebug() << "source: " << source();
	QDir path(source());
	qDebug() << "Opening: " << path.absolutePath();
	QFile dataFile(path.absolutePath());
	if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qWarning(dataFile.errorString().toLocal8Bit().constData());
		return;
	}

	QTextStream dataStream(& dataFile);
	QString line;
	try {
		m_t.clear();
		m_cp.clear();
		bool ok;
		while (dataStream.readLineInto(& line)) {
			QStringList splitLine = line.split(',');
			qreal tVal = splitLine.at(0).toDouble(& ok);
			if (!ok)
				throw  std::runtime_error(QString(COULD_NOT_CONVERT_ARG).arg(splitLine.at(0)).toStdString());
			qreal cpVal = splitLine.at(1).toDouble(& ok);
			if (!ok)
				throw  std::runtime_error(QString(COULD_NOT_CONVERT_ARG).arg(splitLine.at(1)).toStdString());
			qDebug().nospace() << "Appending: (" << tVal << ", " << cpVal << ")";
			m_t.append(tVal);
			m_cp.append(cpVal);
		}
	} catch (const std::runtime_error & e) {
		qWarning(e.what());
	}
	emit tChanged();
	emit cpChanged();

	findMinMaxT();
	findMinMaxCp();
}

void Model::integrate(qreal from, qreal to)
{
	m_integral = 0.0;
	// note: QList::size() returns int...
	for (int i = 0; i < m_t.size() - 1; i++) {
		if (m_t.at(i + 1) > to)
			break;
		if (m_t.at(i) < from)
			continue;
		m_integral += 0.5 * (m_t.at(i + 1) - m_t.at(i)) * (m_cp.at(i) + m_cp.at(i + 1));
	}

	m_integral += integrateT1(from, to);

	emit integralChanged();
}

void Model::updateSeries(QtCharts::QAbstractSeries * series)
{
    if (series) {
        QtCharts::QXYSeries * xySeries = static_cast<QtCharts::QXYSeries *>(series);
        xySeries->clear();
        // note: QList::size() returns int...
        for (int i = 0; i < m_t.size(); i++)
            xySeries->append(m_t.at(i), m_cp.at(i));
    }
}

void Model::findMinMaxT()
{
	qreal minT = std::numeric_limits<qreal>::infinity();
	qreal maxT = -std::numeric_limits<qreal>::infinity();
	for (int i = 0; i < m_t.size(); i++) {
		minT = qMin(minT, m_t.at(i));
		maxT = qMax(maxT, m_t.at(i));
	}
	if (maxT != -std::numeric_limits<qreal>::infinity() && maxT != m_maxT) {
		m_maxT = maxT;
		emit maxTChanged();
	}
	if (minT != std::numeric_limits<qreal>::infinity() && minT != m_minT) {
		m_minT = minT;
		emit minTChanged();
	}
}

void Model::findMinMaxCp()
{
	qreal minCp = std::numeric_limits<qreal>::infinity();
	qreal maxCp = -std::numeric_limits<qreal>::infinity();
	for (int i = 0; i < m_cp.size(); i++) {
		minCp = qMin(minCp, m_cp.at(i));
		maxCp = qMax(maxCp, m_cp.at(i));
	}
	if (maxCp != -std::numeric_limits<qreal>::infinity() && maxCp != m_maxCp) {
		m_maxCp = maxCp;
		emit maxCpChanged();
	}
	if (minCp != std::numeric_limits<qreal>::infinity() && minCp != m_minCp) {
		m_minCp = minCp;
		emit minCpChanged();
	}
}

qreal Model::integrateT1(qreal from, qreal to)
{
	qreal result = 0.0;

	for (int i = 0; i < m_t1List->rowCount(QModelIndex()); i++) {
		T1 t1 = m_t1List->at(i);
		if (t1.temperature >= from && t1.temperature <= to) {
			result += t1.enthalpy;
		}
	}

	return result;
}
