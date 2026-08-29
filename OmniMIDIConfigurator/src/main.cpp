#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QStyleFactory>
#include <QFile>
#include <QFontDatabase>

#include "utils.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Important to generate/fix config before starting the configurator
    // Temporary until a new API is implemented
    // try {
    //     Utils::InitializeKdmapi();
    // } catch (const std::exception &e) {
    //     QMessageBox::warning(nullptr, WARNING_TITLE, e.what());
    // }


    QFontDatabase::addApplicationFont(":/font/theme/fonts/MomoTrustSans-Regular.ttf");


    QFont font("MomoTrustSans");
    font.setPointSize(14);
    a.setFont(font);

    QFile styleFile(":/style/theme/defaultTheme.qss");
    
   
    if(styleFile.open(QFile::ReadOnly | QFile::Text))
        a.setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    else
        qWarning() << "Could not load stylesheet:" << styleFile.errorString();
    
    
    int r = 0;
    try {
        MainWindow w;
        w.show();
        r = a.exec();
    } catch (const std::exception &e) {
        QMessageBox::warning(nullptr, FATAL_ERROR_TITLE, e.what());
    }
    return r;
}
