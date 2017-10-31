#include "Model.hpp"

#include <QFile>
#include <QDir>
#include <QtDebug>
#include <QXYSeries>

#include <exception>

Model::Model(QObject * parent):
	QObject(parent),
	m_integral(0.0)
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

void Model::parseCSV()
{
	static const char * COULD_NOT_CONVERT_ARG = "Could not convert '%1' to double";

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
}

void Model::integrate()
{
	m_integral = 0.0;
	// note: QList::size() returns int...
	for (int i = 0; i < m_t.size() - 1; i++)
		m_integral += 0.5 * (m_t.at(i + 1) - m_t.at(i)) * (m_cp.at(i) + m_cp.at(i + 1));
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

