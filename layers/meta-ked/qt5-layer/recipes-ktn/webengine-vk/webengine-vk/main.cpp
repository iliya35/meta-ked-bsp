#include <QQuickView>
#include <QQmlEngine>
#include <QQmlContext>
#include <QtWebEngine/qtwebengineglobal.h>
#include <QDir>
#include <QString>
#include <QApplication>
#include <QDebug>
#include "createHTML.h"

/*
 * Usage: webengine-vk [rot] [angle=-90] [url]
 * webengine-vk -> will load http://www.kontron-electronics.de
 * webengine-vk url -> will load url
 * webengine-vk rot url -> will load url, window is rotated about -90 degree
 * webengine-vk rot angle url -> will load url, window is rotated about angle degree
 * webengine-vk remote port url-> will load url with profiling and debugging with chrome browser on port port
 * webengine-vk showBar url will show a status bar
 */

bool isNumeric(const char *str)
{
    while(*str != '\0')
    {
        if(*str < '0' || *str > '9') return false;
        str++;
    }
    return true;
}

bool ParseCommandLine(int argc, char *argv[],const char *arg, char *value, int valueSize, bool argIsValue)
{
    bool retval = false;
    for (int i=1;i<argc;i++)
    {
        if (strstr(argv[i], arg) != 0)
        {
            if (argIsValue)
            {
                strncpy(value, argv[i], valueSize - 1);
            }
            else
            {
                if ((++i)<argc) strncpy(value, argv[i], valueSize - 1);
            }
            retval = true;
            break;
        }
    }
    return retval;
}

int main(int argc, char *argv[])
{
    // Load virtualkeyboard input context plugin
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    QApplication app(argc, argv);
    QtWebEngine::initialize();
    int rota = 0;
    bool rot = false;
    // default url
    QString url = "http://www.kontron-electronics.de";
    //get remote port for debugging and profiling
    char remote[80] = {0};
    char remotePort[80] = {0};
    if (ParseCommandLine(argc, argv, "remote", remotePort, sizeof(remotePort), false))
    {
        sprintf(remote, "0.0.0.0:%s", remotePort);
        qputenv("QTWEBENGINE_REMOTE_DEBUGGING", remote);
    }
    // get rotation with backward compatibilty
    char srota[80] = {0};
    if (ParseCommandLine(argc, argv, "rot", srota, sizeof(srota), true))
    {
        rot = true;
        rota = -90;
        if (ParseCommandLine(argc, argv, "rot", srota, sizeof(srota), false))
        {
            if (isNumeric(srota)) rota = atoi(srota);
        }
    }
    // get url
    char surl[255] = {0};
    if (ParseCommandLine(argc, argv, "http://", surl, sizeof(surl), true)) url = surl;
    if (ParseCommandLine(argc, argv, "https://", surl, sizeof(surl), true)) url = surl;
    if (ParseCommandLine(argc, argv, "file://", surl, sizeof(surl), true)) url = surl;
    qDebug() << "info: using url: " + url;
    createHTML mycreateHTML(url);
    QQuickView view;
    view.engine()->rootContext()->setContextProperty("createHTML", &mycreateHTML);
    view.engine()->rootContext()->setContextProperty("htmlPath", "file://" + QDir::tempPath() + "/");
    view.engine()->rootContext()->setContextProperty("indexPath", url);
    view.engine()->rootContext()->setContextProperty("rot", rot);
    view.engine()->rootContext()->setContextProperty("rota", rota);
    view.setSource(QString("qrc:/main.qml"));
    view.setObjectName("QQuickView");
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.engine()->setObjectName("QQuickEngine");
    view.engine()->addImportPath(qApp->applicationDirPath());
    // Set size to 800 x 480 WXGA
    view.setWidth(800);
    view.setHeight(480);
    view.show();
    return app.exec();
}
