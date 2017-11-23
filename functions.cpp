#include "functions.hpp"

qreal integrate(const QList<qreal> & x, const QList<qreal> & y, qreal from, qreal to)
{
	qreal result = 0.0;
	// note: QList::size() returns int...
	for (int i = 0; i < x.size() - 1; i++) {
		if (x.at(i + 1) > to)
			break;
		if (x.at(i) < from)
			continue;
		result += 0.5 * (x.at(i + 1) - x.at(i)) * (y.at(i) + y.at(i + 1));
	}

	return result;
}
