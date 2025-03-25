#ifndef M_DIRINIT_H
#define M_DIRINIT_H

#include <QObject>
#include <QMutex>
class M_DirInit : public QObject
{
    Q_OBJECT
public:
    static M_DirInit *Instance();
    explicit M_DirInit(QObject *parent = nullptr);

    bool dirInit();
private:
    static M_DirInit *self;

signals:

};

#endif // M_DIRINIT_H
