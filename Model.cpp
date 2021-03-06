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
	m_baseEnthalpy(0.0),
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

qreal Model::baseEnthalpy() const
{
	return m_baseEnthalpy;
}

void Model::setBaseEnthalpy(qreal enthalpy)
{
	if (m_baseEnthalpy != enthalpy) {
		m_baseEnthalpy = enthalpy;
		emit baseEnthalpyChanged();
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
	emit cpTChanged();

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

	m_integral += baseEnthalpy();

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
	// Uggliest code ever :/

	qDebug("updating cumulative series");
	if (series) {
		QtCharts::QXYSeries * xySeries = static_cast<QtCharts::QXYSeries *>(series);
		xySeries->clear();

		// Initialize cumulative data.
		m_cumulative.clear();
		for (int i = 0; i < m_t.count(); i++)
			m_cumulative.append(std::make_pair(m_t.at(i), m_cp.at(i)));

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
			for (int cumulativeIt = 0; cumulativeIt < m_cumulative.count() - 1; cumulativeIt++) {
				while (transitionT->at(itTransitionT) >= m_cumulative.at(cumulativeIt).first && transitionT->at(itTransitionT) < m_cumulative.at(cumulativeIt + 1).first) {
					qDebug("Appending point %d", itTransitionT);
					qreal t1 = m_cumulative.at(cumulativeIt).second;
//					qDebug("t1 %f", t1);
					qreal t2 = m_cumulative.at(cumulativeIt + 1).second;
//					qDebug("t2 %f", t2);
					qreal base = m_cumulative.at(cumulativeIt + 1).first - m_cumulative.at(cumulativeIt).first;
//					qDebug("base %f", base);
					qreal cpPos = transitionT->at(itTransitionT) - m_cumulative.at(cumulativeIt).first;
//					qDebug("cpPos %f", cpPos);
//					cumulativeTail.append(std::make_pair(transitionT->at(itTransitionT), transitionCp->at(itTransitionT) + m_cumulative.at(cumulativeIt).second));
					cumulativeTail.append(std::make_pair(transitionT->at(itTransitionT), transitionCp->at(itTransitionT) + trapezePoint(t1, t2, base, cpPos)));
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
			// Adjust cumulative points.
			int cumulativeIt = 0;
			while (cumulativeIt < m_cumulative.count() && m_cumulative.at(cumulativeIt).first < transitionT->at(0))
				cumulativeIt++;
			if (cumulativeIt < m_cumulative.count())
				for (int transitionIt = 0; transitionIt < transitionT->count() - 1; transitionIt++) {
					qDebug("m_cumulative.at(cumulativeIt).first   %f ", m_cumulative.at(cumulativeIt).first);
					while (m_cumulative.at(cumulativeIt).first  >= transitionT->at(transitionIt) && m_cumulative.at(cumulativeIt).first < transitionT->at(transitionIt + 1)) {
						qreal t1 = transitionCp->at(transitionIt);
//						qDebug("t1 %f", t1);
						qreal t2 = transitionCp->at(transitionIt + 1);
//						qDebug("t2 %f", t2);
						qreal base = transitionT->at(transitionIt + 1) - transitionT->at(transitionIt);
//						qDebug("base %f", base);
						qreal cpPos = m_cumulative.at(cumulativeIt).first - transitionT->at(transitionIt);
//						qDebug("cpPos %f", cpPos);
						m_cumulative[cumulativeIt].second += trapezePoint(t1, t2, base, cpPos);
	//					qDebug("cpPos %f", cpPos);
	//					cumulativeTail.append(std::make_pair(transitionT->at(itTransitionT), transitionCp->at(itTransitionT) + m_cumulative.at(cumulativeIt).second));
	//					cumulativeTail.append(std::make_pair(transitionT->at(itTransitionT), transitionCp->at(itTransitionT) + trapezePoint(t1, t2, base, cpPos)));
						cumulativeIt++;
						if (cumulativeIt >= m_cumulative.count())
							break;
					}
					if (cumulativeIt >= m_cumulative.count())
						break;
				}

			m_cumulative += cumulativeTail;
			qSort(m_cumulative.begin(), m_cumulative.end(), CumulativeLessThan());


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

		for (auto it = m_cumulative.begin(); it != m_cumulative.end(); ++it)
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
	findMinMaxCp();
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
	for (int i = 0; i < m_cumulative.size(); i++) {
		minCp = qMin(minCp, m_cumulative.at(i).second);
		maxCp = qMax(maxCp, m_cumulative.at(i).second);
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

qreal Model::integrateTransitions(qreal from, qreal to) const
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

qreal Model::trapezePoint(qreal t1, qreal t2, qreal base, qreal pos) const
{
	qDebug("trapeze point delta  %f", pos * (t2 - t1) / base);
	return t1 + pos * (t2 - t1) / base;
}
