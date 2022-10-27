#include "createHTML.h"
#include <QCoreApplication>
#include <QTextStream>
using namespace std;

createHTML::createHTML(QString url, QObject *parent) : QObject(parent)
{
    htmlPath = QDir::tempPath();
    QString path = htmlPath + "/nav.htm";
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);
    out<<"<html>\n";
    out<<"  <head>\n";
    out<<"    <meta http-equiv='content-type' content='text/html; charset=utf-8'>\n";
    out<<"    <title></title>\n";
    out<<"  </head>\n";
    out<<"  <body>\n";
    //out<<"  <FORM><INPUT type='image' src='back.png' alt='Back' size=32 width=32 onClick='history.back();return true;'></FORM>\n";
    out<<"<a href='"<<url.toStdString().c_str()<<"' target='_top'><img src='back.png' size=32 width=32></a>\n";
    out<<"  </body>\n";
    out<<"</html>\n";
    file.close();
    QFile* backarrow = new QFile(QStringLiteral(":/back.png"));
    backarrow->copy(htmlPath + "/back.png");
    delete backarrow;
}

createHTML::~createHTML()
{
    QFile* backarrow = new QFile(htmlPath + "/back.png");
    QFile* out = new QFile(htmlPath + "/out.htm");
    QFile* nav = new QFile(htmlPath + "/nav.htm");
    backarrow->remove();
    out->remove();
    nav->remove();
    delete backarrow;
    delete out;
    delete nav;
}

void createHTML::getURL(QString url)
{
    QString path = htmlPath + "/out.htm";
    QFile file(path.toStdString().c_str());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);
    out<<"<html>\n";
    out<<"  <head>\n";
    out<<"    <meta http-equiv='content-type' content='text/html; charset=utf-8'>\n";
    out<<"    <title></title>\n";
    out<<"  </head>\n";
    out<<"  <frameset cols='44, *' border=0 frameborder=0 framespacing=0>\n";
    out<<"    <frame src='nav.htm' name=''>\n";
    out<<"    <frame src='"<<url.toStdString().c_str()<<"' name=''>\n";
    out<<"  </frameset>\n";
    out<<"</html>\n";
    file.close();
}
