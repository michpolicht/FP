#include "Model.hpp"
#include "Transition.hpp"

#include <QApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

	qmlRegisterType<Model>("FP", 1, 0, "Model");
	qmlRegisterType<Transition>("FP", 1, 0, "Transition");

	QQmlApplicationEngine engine;
	engine.load(QUrl(QLatin1String("qrc:/main.qml")));

	return app.exec();
}
