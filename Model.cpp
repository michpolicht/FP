#include "Model.hpp"
#include "functions.hpp"

#include <QFile>
#include <QDir>
#include <QtDebug>
#include <QXYSeries>
#include <QVector>

#include <exception>

Model::Model(QObject * parent):
	QObject(parent),
	m_integral(0.0),
	m_minT(0.0),
	m_maxT(0.0),
	m_minCp(0.0),
	m_maxCp(0.0),
	m_transitionList(new TransitionList)
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

QAbstractListModel * Model::transitionList() const
{
	return m_transitionList.get();
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
//	m_integral = 0.0;
//	// note: QList::size() returns int...
//	for (int i = 0; i < m_t.size() - 1; i++) {
//		if (m_t.at(i + 1) > to)
//			break;
//		if (m_t.at(i) < from)
//			continue;
//		m_integral += 0.5 * (m_t.at(i + 1) - m_t.at(i)) * (m_cp.at(i) + m_cp.at(i + 1));
//	}

	m_integral = ::integrate(m_t, m_cp, from, to);

	m_integral += integrateTransitions(from, to);

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

void Model::updateCumulativeSeries(QtCharts::QAbstractSeries * series)
{
	qDebug("updating cumulative series");
	if (series) {
		QtCharts::QXYSeries * xySeries = static_cast<QtCharts::QXYSeries *>(series);
		xySeries->clear();

		// Initialize cumulative data.
		QList<std::pair<qreal, qreal>> cumulative;
		for (int i = 0; i < m_t.count(); i++)
			cumulative.append(std::make_pair(m_t.at(i), m_cp.at(i)));

		// Append transition points to the set, we'll sort them later.
		int transitionsCount = m_transitionList->rowCount(QModelIndex());
		for (int nthTransition = 0; nthTransition < transitionsCount; nthTransition++) {
			QList<qreal> * transitionT = m_transitionList->at(nthTransition)->t();
			if (transitionT->count() == 0)
				continue;
			QList<qreal> * transitionCp = m_transitionList->at(nthTransition)->cp();
			int itTransitionT = 0;
			QList<std::pair<qreal, qreal>> cumulativeTail;
//			bool updateCumulativePoint = false;
			for (int cumulativeIt = 0; cumulativeIt < cumulative.count() - 1; cumulativeIt++) {

				while (transitionT->at(itTransitionT) >= cumulative.at(cumulativeIt).first && transitionT->at(itTransitionT) < cumulative.at(cumulativeIt + 1).first) {
					qDebug("Appending point %d", itTransitionT);
					cumulativeTail.append(std::make_pair(transitionT->at(itTransitionT), transitionCp->at(itTransitionT) + cumulative.at(cumulativeIt).second));
					itTransitionT++;
					if (itTransitionT >= transitionT->count())
						break;
				}
				if (itTransitionT >= transitionT->count())
					break;
//				if (itTransitionT > 0) {
//					if (updateCumulativePoint)
//						cumulative[cumulativeIt].second += transitionCp->at(itTransitionT);
//					else
//						updateCumulativePoint = true;
//				}
			}
			cumulative += cumulativeTail;
			qSort(cumulative.begin(), cumulative.end(), CumulativeLessThan());


//			for (int itT = 0; itT < m_t.count(); itT++) {
//				if (transitionT->at(itTransitionT) < m_t.at(itT))
//					continue;
//				if (transitionT->at(itTransitionT) >= m_t.at(itT)) {
//					cumulative.append(std::make_pair(transitionT->at(itTransitionT), transitionCp->at(itTransitionT) + m_cp.at(itT)));
//					itTransitionT++;
//				}
//				if (itTransitionT >= transitionT->count())
//					break;
//			}
		}


//		qSort(cumulative.begin(), cumulative.end(), CumulativeLessThan());

		for (auto it = cumulative.begin(); it != cumulative.end(); ++it)
			xySeries->append(it->first, it->second);

//		qreal lowestT = std::numeric_limits<qreal>::infinity();
//		int transitionIndex;
//		for (int i = 0; i < transitionsCount; i++) {
//			Transition * transition = m_transitionList->at(i);

//		}

//		for (int i = 0; i < m_t.count() - 1; i++) {
//		}

//		for (int i = 0; i < m_transitionList->rowCount(QModelIndex()); i++) {
//			Transition * transition = m_transitionList->at(i);
//			QList<qreal> * transitionT = transition->t();
//			QList<qreal> * transitionCp = transition->cp();

//			int start = transition->temperatureBegin()

////			for (int j = 0; j < transitionT->count(); j++) {

//			}

//			if (t->temperatureBegin() >= from && t->temperatureBegin() <= to) {
//				result += t->enthalpy();
//			}
//		}

		// note: QList::size() returns int...
//		for (int i = 0; i < t.size(); i++) {
//			xySeries->append(t.at(i), cp.at(i));
//		}
	}
}

bool Model::CumulativeLessThan::operator ()(std::pair<qreal, qreal> p1, std::pair<qreal, qreal> p2) const
{
	return p1.first < p2.first;
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

qreal Model::integrateTransitions(qreal from, qreal to)
{
	qreal result = 0.0;

	for (int i = 0; i < m_transitionList->rowCount(QModelIndex()); i++) {
		Transition * t = m_transitionList->at(i);
		result += ::integrate(*t->t(), *t->cp(), from, to);
//		if (t->temperatureBegin() >= from && t->temperatureBegin() <= to) {
//			result += t->enthalpy();
//		}
	}

	return result;
}

void Model::insertTransitionToCumulative(const Transition & transition, QList<qreal> & t, QList<qreal> & cp)
{
//	QList<qreal> * transitionT = transition->t();
//	QList<qreal> * transitionCp = transition->cp();

//	int startIndex = 0;
//	while (startIndex < t.size() && t.at(startIndex) < transition->temperatureBegin())
//		startIndex++;

//	for (int i = 0; i < t.size(); i++) {
//		t.at(i)
//	}

//	int startIndex = transition->temperatureBegin();
}
