#include "FileReader.hpp"

#include <QFile>
#include <QDir>
#include <QtDebug>

FileReader::FileReader(QObject * parent):
	QObject(parent)
{
	connect(this, & FileReader::sourceChanged, this, & FileReader::parseCSV);
}

QString FileReader::source() const
{
	return m_source;
}

void FileReader::setSource(const QString & source)
{
	if (m_source != source) {
		m_source = source;
		emit sourceChanged();
	}
}

QList<qreal> * FileReader::t()
{
	return & m_t;
}

void FileReader::parseCSV()
{
	QDir path(source());
	qDebug() << "Opening: " << path.absolutePath();
	QFile dataFile(path.absolutePath());
	if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
		qFatal(dataFile.errorString().toLocal8Bit().constData());

//	QStringList List;
//	QList<QByteArray> splitLine = line.split(',');
	QTextStream dataStream(& dataFile);
	QString line;
	while (dataStream.readLineInto(& line)) {
		QStringList splitLine = line.split(',');
		qDebug() << splitLine.at(0) << " " << splitLine.at(1);
	}

//	qDebug() << wordList;
}

