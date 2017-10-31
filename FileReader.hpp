#ifndef FILEREADER_HPP
#define FILEREADER_HPP

#include <QObject>
#include <QString>
#include <QList>

class FileReader : public QObject
{
	Q_OBJECT

	public:
		Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
		Q_PROPERTY(QList<qreal> * t READ t NOTIFY tChanged)
//		Q_PROPERTY(QList<qreal> * cp READ cp NOTIFY cpChanged)

		explicit FileReader(QObject * parent = 0);

		QString source() const;

		void setSource(const QString & source);

		QList<qreal> * t();

	signals:
		void sourceChanged();

		void tChanged();

	public slots:

	private slots:
		void parseCSV();

	private:
		QString m_source;
		QList<qreal> m_t;
};

#endif // FILEREADER_HPP
