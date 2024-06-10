#include "MainWindow.h"
#include <qapplication.h>

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);
	MainWindow w = MainWindow();
	w.setMinimumSize(640, 480);
	w.show();
	return a.exec();
}