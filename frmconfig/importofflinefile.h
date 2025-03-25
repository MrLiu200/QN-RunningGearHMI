#ifndef IMPORTOFFLINEFILE_H
#define IMPORTOFFLINEFILE_H

#include <QObject>
#include "predata.h"
#include <QFile>
#include <QFileInfo>
#include <QMutex>
class ImportOfflineFile : public QObject
{
    Q_OBJECT
public:
    static ImportOfflineFile *Instance();
    explicit ImportOfflineFile(QObject *parent = nullptr);

private:
    static ImportOfflineFile *self;

public :
    QStringList importconfig(QStringList filelist);
    bool importSql(QString dbfile);
    void importData(QStringList filelist);


private:
    void CopyOrigin(QString targetdir, QStringList files);
    void unzipOrigin(QString targetdir, QStringList files);
signals:

    void copyResult(bool ok);
    void unzipResult(bool ok);
    void importdataresult(bool ok);
};

#endif // IMPORTOFFLINEFILE_H
