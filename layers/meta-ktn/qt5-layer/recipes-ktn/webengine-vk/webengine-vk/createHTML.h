#ifndef CREATEHTML_H
#define CREATEHTML_H

#include <QObject>
#include <QString>
#include <QDir>

class createHTML : public QObject
{

    Q_OBJECT

public:
    explicit createHTML(QString url, QObject *parent = 0);
    ~createHTML();
    Q_INVOKABLE void getURL(QString url);

private:
    QString htmlPath;
};

#endif
