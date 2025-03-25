#include "m_dirinit.h"
#include "app.h"
#include "corehelper.h"
#include <QDir>

M_DirInit *M_DirInit::self = 0;
M_DirInit *M_DirInit::Instance()
{
    if (!self) {
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self) {
            self = new M_DirInit;
        }
    }
    return self;
}

M_DirInit::M_DirInit(QObject *parent) : QObject(parent)
{

}

bool M_DirInit::dirInit()
{

    QDir tempDir(App::TempDir);

    if(!tempDir.exists()){
        if(!tempDir.mkdir(tempDir.absolutePath())){
            return false;
        }
    }

    QDir DataDir(App::DownLoadDataDir);
    if(!DataDir.exists()){
        if(!DataDir.mkdir(DataDir.absolutePath())){
            return false;
        }
    }

    QDir configdir(App::DownLoadConfigDir);
    if(!configdir.exists()){
        if(!configdir.mkdir(configdir.absolutePath())){
            return false;
        }
    }

    QDir dbdir(App::DownLoadDBDir);
    if(!dbdir.exists()){
        if(!dbdir.mkdir(dbdir.absolutePath())){
            return false;
        }
    }

    QDir tempdata(App::DownLoadTempDataDir);
    if(!tempdata.exists()){
        if(!tempdata.mkdir(tempdata.absolutePath())){
            return false;
        }
    }
    return true;
}
