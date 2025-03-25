#include "corehelper.h"
#include <QGraphicsOpacityEffect>
#include <QLabel>
CoreHelper::CoreHelper(QObject *parent) : QObject(parent)
{

}

QString CoreHelper::APPName()
{
    QString name = qApp->applicationFilePath();
    QStringList list = name.split("/");
    name = list.at(list.count() - 1).split(".").at(0);
    return name;
}

QString CoreHelper::APPPath()
{
    return qApp->applicationDirPath();
}

QString CoreHelper::byteArrayToHexStr(const QByteArray &data)
{
    QString temp = "";
    QString hex = data.toHex();

    for (int i = 0; i < hex.length(); i = i + 2) {
        temp += hex.mid(i, 2) + " ";
    }

    return temp.trimmed().toUpper();
}

void CoreHelper::newDir(const QString &dirName)
{
    QString strDir = dirName;

    //如果路径中包含斜杠字符则说明是绝对路径
    //linux系统路径字符带有 /  windows系统 路径字符带有 :/
    if (!strDir.startsWith("/") && !strDir.contains(":/")) {
        strDir = QString("%1/%2").arg(CoreHelper::APPPath()).arg(strDir);
    }

    QDir dir(strDir);

    if (!dir.exists()) {
        dir.mkpath(strDir);
    }
}

void CoreHelper::SetSystemTime(int year, int mounth, int day, int hour, int minutes, int secord)
{
#ifdef Q_OS_WIN
    QProcess p(0);
    p.start("cmd");
    p.waitForStarted();
    p.write(QString("date %1-%2-%3\n").arg(year).arg(mounth).arg(day).toLatin1());
    p.closeWriteChannel();
    p.waitForFinished(1000);
    p.close();
    p.start("cmd");
    p.waitForStarted();
    p.write(QString("time %1:%2:%3.00\n").arg(hour).arg(minutes).arg(secord).toLatin1());
    p.closeWriteChannel();
    p.waitForFinished(1000);
    p.close();
#else
    QString cmd = QString("date %1%2%3%4%5.%6").arg(mounth).arg(day).arg(hour).arg(minutes).arg(year).arg(secord);
    system(cmd.toLatin1());
    system("hwclock -w");
#endif
}

int CoreHelper::Getcapacity(QString DriveName)
{
    int ret = 0;
    QProcess process;
    QStringList cmd;
    cmd << "-h";
    cmd << DriveName;
    process.start("df", cmd);

    // 等待进程完成
    process.waitForFinished();

    // 获取命令输出
    QByteArray output = process.readAllStandardOutput();

    // 将输出转换为字符串并输出
    QString outputString(output);

    QRegularExpression regex("(\\d+)\\%");//正则表达式，找出当前句子%前的数字
    QRegularExpressionMatch match = regex.match(outputString);

    if (match.hasMatch()) {
        ret = match.captured(1).toInt();
    }else{
        ret = -1;
    }
    return ret;
}

void CoreHelper::deleteDirectory(const QString &path)
{
    QDir dir(path);

    if (!dir.exists()) {
        return;
    }

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();

    foreach (QFileInfo fi, fileList) {
        if (fi.isFile()) {
            fi.dir().remove(fi.fileName());
        } else {
            deleteDirectory(fi.absoluteFilePath());
            dir.rmdir(fi.absoluteFilePath());
        }
    }
}

void CoreHelper::Reboot()
{
#ifdef __arm__
    system("reboot");
#else
    qApp->exit();
    QProcess::startDetached(qApp->applicationFilePath(), QStringList());
#endif
}

QString CoreHelper::getFileName(const QString &filter, QString defaultDir)
{
    return QFileDialog::getOpenFileName(0, "选择文件", defaultDir , filter);
}

QString CoreHelper::getSaveName(const QString &filter, QString defaultDir)
{
    return QFileDialog::getSaveFileName(0, "选择文件", defaultDir , filter);
}

bool CoreHelper::folderIsExist(const QString &strFolder)
{
    QDir tempFolder(strFolder);
    return tempFolder.exists();
}

bool CoreHelper::fileIsExist(const QString &strFile)
{
    QFile tempFile(strFile);
    return tempFile.exists();
}

bool CoreHelper::copyFile(const QString &sourceFile, const QString &targetFile)
{
    bool ok;
    ok = QFile::copy(sourceFile, targetFile);
    //将复制过去的文件只读属性取消
    ok = QFile::setPermissions(targetFile, QFile::WriteOwner);
    return ok;
}

QVector<float> CoreHelper::CalculateDimension(QVector<double> data)
{
    int len = data.size();
    Eigen::VectorXd vec(len);
    QVector <float> Qvret;

    for (int i = 0; i < len; ++i) {
        vec(i) = data.at(i);
    }
    // 计算最大值和最小值
    float max_value = vec.maxCoeff();
    float min_value = vec.minCoeff();

    // 计算均值
    float mean = vec.mean();

    // 计算方差和标准差
    float variance = (vec.array() - mean).square().sum() / (vec.size() - 1);
    float std_deviation = sqrt(variance);

    // 计算均方根
    float rms = sqrt((vec.array() * vec.array()).sum() / vec.size());

    // 计算整流平均值
    Eigen::VectorXd abs_vec = vec.array().abs();
    float rectified_mean = abs_vec.mean();

    // 计算偏斜度
    //    double skewness = (vec.array() - mean).pow(3).mean() / pow(std_deviation, 3);

    // 计算峭度
    float kurtosis = (vec.array() - mean).pow(4).mean() / pow(variance, 2);

    // 计算峰峰值
    float peak_to_peak = max_value - min_value;

    // 计算波形因子
    float crest_factor = rms / rectified_mean;

    // 计算峰值因子
    float peak_factor = max_value / rms;

    // 计算脉冲因子
    float impulse_factor = max_value / rectified_mean;

    // 计算裕度因子square
    float square_mean = (sqrt(vec.array().abs()).sum()) / vec.size();
    float root_mean = square_mean * square_mean;
    float margin_factor = peak_to_peak / root_mean;
    //峭度因子
    //    double kurtosis_factor = vec.array().pow(4).sum()/sqrt(vec.array().pow(2).sum());

    Qvret.append(max_value);
    Qvret.append(min_value);
    Qvret.append(mean);
    Qvret.append(variance);
    Qvret.append(std_deviation);
    Qvret.append(rms);
    Qvret.append(rectified_mean);
    Qvret.append(kurtosis);
    Qvret.append(peak_to_peak);
    Qvret.append(crest_factor);
    Qvret.append(peak_factor);
    Qvret.append(impulse_factor);
    Qvret.append(margin_factor);

    return Qvret;
}

bool CoreHelper::ExternalStorageInit()
{
    bool IsMount = false;
    QDir mediaDir("/udisk");
    QStringList entryList = mediaDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    if(entryList.isEmpty()){//如果该目录为空或者目录不存在才进行挂载
        QString command = "lsblk -b -o NAME,SIZE,FSTYPE,MOUNTPOINT /dev/sda"; // 获取 /dev/sda 下的所有设备、大小和文件系统类型
        QProcess process;
        process.start(command);
        process.waitForFinished(-1);

        QByteArray output = process.readAllStandardOutput();
        QString outputString = QString::fromLatin1(output);
        QStringList lines = outputString.split('\n');

        /*遍历结果找出空间最大的盘符*/
        QString device,fsType,mountDir;
        qlonglong size = 0;
        for (const QString &line : lines) {
            QStringList fields = line.trimmed().split(QRegExp("\\s+"));
            if (fields.length() >= 3){
                qlonglong temp = fields[1].toLongLong();
                if(temp > size){
                    device = fields[0];
                    size = fields[1].toLongLong();
                    fsType = fields[2];
                    if(fields.length() == 4)
                        mountDir = fields[3];
                }
            }
        }//找到了空间最大的盘符

        //提取盘符名称有效信息
        QRegularExpression pattern("sda\\d+");
        QRegularExpressionMatch match = pattern.match(device);

        // 检查是否有匹配
        if (match.hasMatch()) {
            device = match.captured(0);
        }else{
            IsMount = false;
            return IsMount;
        }

        //检查是否已经挂载，若挂载目录非指定目录，需要卸载
        if(!mountDir.isEmpty()){
            if(mountDir != "/udisk"){
                int out = process.execute("umount",QStringList()<<mountDir);
                if(out == -1){
                    IsMount = false;
                    return IsMount;
                }
            }
        }
        QString deviceName = QString("/dev/%1").arg(device);
        QFileInfo checkFile(deviceName);
        QString CmdProgram = "ntfs-3g";
        if(checkFile.exists()){
            CoreHelper::newDir("/udisk");
            //依据不同的文件系统进行挂载
            if(fsType == "ntfs"){
                CmdProgram = "ntfs-3g";
            }else if(fsType == "vfat"){
                CmdProgram = "mount";
            }else if(fsType == "exfat"){
                //需要先格式化，目前只能格式化为ext4或者vfat，暂时无法支持 ntfs
                int ret = process.execute("mkfs.vfat", QStringList() << deviceName);
                if(ret == -1){
                    IsMount = false;
                    return IsMount;
                }
            }else{
                CmdProgram = "mount";
            }
            int out = process.execute(CmdProgram, QStringList() << deviceName << "/udisk");
            if(out != -1){
                IsMount = true;
            }
        }
    }
    return IsMount;
}

quint32 CoreHelper::GetrCRC32_MPEG_2(QString fileName)
{
    QFile file(fileName);
    QByteArray array;
    quint32 ret = 0;
    if(file.open(QFile::ReadOnly)){
        array = file.readAll();
        ret = GetCRC32_MPEG_2(array);
    }
    file.close();
    return ret;
}

quint32 CoreHelper::GetCRC32_MPEG_2(const QByteArray array)
{
    static const uint32_t crc32_tab[] = {
        0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
        0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
        0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
        0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
        0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039, 0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
        0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
        0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
        0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
        0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
        0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
        0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
        0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
        0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
        0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6, 0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
        0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
        0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
        0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
        0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
        0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
        0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff, 0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
        0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
        0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
        0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7, 0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
        0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
        0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
        0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
        0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
        0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
        0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
        0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
        0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
        0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668, 0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4,
    };
    quint32 crc32 = 0xFFFFFFFF;
#if 1
    for (int i = 0; i < array.size(); ++i) {

        char byte = array.at(i);
        crc32 = (crc32 << 8) ^ crc32_tab[((crc32 >> 24) ^ byte) & 0xFF];
    }
#else
    //STM32F407、STM32F103、STM32G042、STM32H750
#endif
    return crc32;
}

quint32 CoreHelper::GetCRC32_STM32H750(QString filename)
{
    static const uint32_t H750_tab[] = {
        0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
        0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
        0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
        0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
        0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039, 0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
        0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
        0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
        0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
        0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
        0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
        0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
        0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
        0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
        0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6, 0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
        0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
        0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
        0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
        0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
        0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
        0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff, 0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
        0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
        0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
        0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7, 0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
        0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
        0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
        0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
        0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
        0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
        0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
        0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
        0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
        0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668, 0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4,
    };
    QFile file(filename);
    QByteArray array;
    quint32 ret = 0;
    if(file.open(QFile::ReadOnly)){
        array = file.readAll();
        quint32 crc32 = 0xFFFFFFFF;
        for(int i=0;i < array.size();i+=4){
            char byte0 = array.at(i);
            char byte1 = array.at(i+1);
            char byte2 = array.at(i+2);
            char byte3 = array.at(i+3);
            crc32 = (crc32 << 8) ^ H750_tab[((crc32 >> 24) ^ byte3) & 0xFF];
            crc32 = (crc32 << 8) ^ H750_tab[((crc32 >> 24) ^ byte2) & 0xFF];
            crc32 = (crc32 << 8) ^ H750_tab[((crc32 >> 24) ^ byte1) & 0xFF];
            crc32 = (crc32 << 8) ^ H750_tab[((crc32 >> 24) ^ byte0) & 0xFF];
        }
        ret = crc32;
        file.close();
    }

    return ret;
}

quint8 CoreHelper::CheckSum(const uint8_t *data, int Start, int end)
{
    qint8 ret = 0;
    if (Start < 0 || end <= Start) {
        return ret;
    }
    for(int i=Start;i<end;i++){
        ret += *(data+i);
    }
    return ret;
}

quint8 CoreHelper::CheckSum(const QByteArray array, int Start, int end)
{
    qint8 ret = 0;
    if (Start < 0 || end <= Start) {
        return ret;
    }
    for(int i=Start;i<end;i++){
        qint8 temp = array.at(i);
        ret += temp;
    }
    return ret;
}

double CoreHelper::DS18B20ByteToTemperature(uint8_t High, uint8_t Low)
{
    //高位的高5个字节表示符号，若都为1则表示为负数，都为0表示为正数
    uint8_t flag = High >> 3;
    double ret = 0.0000;
    uint16_t temp = (High<<8) | Low;
    if(flag == 31){//负数，按位取反后+1
        temp = ~temp + 1;
        ret = -0.0625 * temp;
    }else if(flag == 0){//正数
        ret = temp * 0.0625;
    }
    return ret ;
}

double CoreHelper::M117ByteToTemperature(uint8_t High, uint8_t Low)
{
    uint8_t flag = High >> 7;
    double ret = 0.0000;
    uint16_t temp = (High<<8) | Low;
    if(flag == 1){//负数
        temp = ~temp + 1;
        ret = 40 + (-(temp/256));
    }else{
        ret = 40 +(temp/256);
    }
    return ret;
}

void CoreHelper::FFT(QVector<double> &envelope)
{
    int len = envelope.size();
    fftw_complex *in = NULL;
    fftw_complex *out = NULL;
    fftw_plan p;
    //分配内存空间
    in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * len);
    out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * len);
    // 创建句柄
    p = fftw_plan_dft_1d(len, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    // 输入
    for (int i=0; i<len; i++)
    {
        in[i][0] = envelope[i];
        in[i][1] = 0;
    }
    //执行
    fftw_execute(p);

    // 输出
    envelope[0] = 0.0;
    for (int i=1; i<len; i++)
    {
        envelope[i]= sqrt((out[i][0])*(out[i][0])+(out[i][1])*(out[i][1]));
    }

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
}

uint8_t CoreHelper::ObtainTemAlarmLevel(double ambient, double channel)
{
    uint8_t grade = 0xFF;
    double temp = 0.0;
    if(channel > ambient){//温升
        temp = channel-ambient;
    }

    //    if((channel > APP::AbsoluteTemperatureThreshold) || (temp > APPSetting::RelativeTemperatureThreshold)){
    //        grade = 2;
    //    }else if((channel > APPSetting::AbsoluteTemperatureWarning) || (temp > APPSetting::AbsoluteTemperatureWarning)){
    //        grade = 1;
    //    }
    //    return grade;
}

bool CoreHelper::Zip_extract(QString TargerPath, QString zipfile)
{
    // 创建 QuaZip 对象，指向目标 zip 文件
    QuaZip zip(zipfile);

    // 打开 zip 文件
    if (!zip.open(QuaZip::mdUnzip)) {
        qDebug() << "无法打开 zip 文件:" << zipfile;
        return false;
    }

    // 创建目标文件夹（如果不存在）,若存在则清空其中内容
    QDir dir(TargerPath);

    if (!dir.exists()) {
        dir.mkpath(TargerPath);
    }/*else{
        deleteDirectory(TargerPath);
    }*/
    //    newDir(TargerPath);

    // 获取 zip 文件中的文件列表
    QuaZipFile zipFile(&zip);
    for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile()) {
        // 获取当前文件的文件名
        QString fileName = zip.getCurrentFileName();

        // 设置解压文件的路径
        QString fullPath = TargerPath + "/" + fileName;

        // 打开 zip 中的当前文件
        if (!zipFile.open(QIODevice::ReadOnly)) {
            qDebug() << "无法打开 zip 中的文件:" << fileName;
            return false;
        }

        // 创建解压目标文件
        QFile outFile(fullPath);
        if(outFile.exists()){
            outFile.remove();
        }
        if (!outFile.open(QIODevice::WriteOnly)) {
            qDebug() << "无法创建文件:" << fullPath;
            return false;
        }

        // 将 zip 文件内容写入目标文件
        outFile.write(zipFile.readAll());

        // 关闭文件
        outFile.close();
        zipFile.close();
    }

    // 关闭 zip 文件
    zip.close();
    return true;
}

bool CoreHelper::Zip_compress(QString TargerFile, QStringList files)
{
    // 创建QuaZip对象，表示目标压缩包
    QuaZip zip(TargerFile);

    // 尝试打开压缩包，如果打开失败返回false
    if (!zip.open(QuaZip::mdCreate)) {
        qDebug() << "无法打开压缩包：" << TargerFile;
        return false;
    }

    // 遍历文件列表，将每个文件添加到压缩包
    for (const QString &file : files) {
        QFile inputFile(file);
        if (!inputFile.open(QIODevice::ReadOnly)) {
            qDebug() << "无法打开文件：" << file;
            return false;
        }

        // 获取文件名而不是路径
        QString fileName = QFileInfo(file).fileName();

        // 创建一个新的压缩包文件
        QuaZipFile zipFile(&zip);

        // 打开压缩包文件进行写入
        if (!zipFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileName, file)))
        {
            qDebug() << "Failed to add file to ZIP:" << file;
            return false;
        }

        // 读取文件数据并写入到压缩包
        QByteArray fileData = inputFile.readAll();
        zipFile.write(fileData);

        // 关闭压缩包文件
        zipFile.close();
        inputFile.close();
    }

    // 关闭压缩包
    zip.close();
    return true;
}

void CoreHelper::showFadingMessage(QWidget *parent, const QString &message)
{
    QLabel *label = new QLabel(parent);
    label->setText(message);
    label->setStyleSheet("background-color: rgba(0, 0, 0, 150); color: white; padding: 10px; border-radius: 5px;");
    label->setAlignment(Qt::AlignCenter);

    // 设置大小并放置在父窗口中央
    label->adjustSize();
    int x = (parent->width() - label->width()) / 2;
    int y = (parent->height() - label->height()) / 2;
    label->setGeometry(x, y, label->width(), label->height());

    // 透明度效果
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(label);
    label->setGraphicsEffect(effect);

    // 淡出动画
    QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(1500);
    animation->setStartValue(0.8);
    animation->setEndValue(0.0);

    // 2 秒后删除 label
    QTimer::singleShot(2000, label, [label]() {
        label->deleteLater();
    });

    label->show();
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

bool CoreHelper::isIP(const QString &ip)
{
    QRegExp RegExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    return RegExp.exactMatch(ip);
}




