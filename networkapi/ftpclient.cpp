#include "ftpclient.h"
#include "corehelper.h"
#include <QMessageBox>

FtpClient *FtpClient::self = 0;
FtpClient *FtpClient::Instance()
{
    if(!self){
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if(!self){
            self = new FtpClient;
        }
    }
    return self;
}

FtpClient::FtpClient(QObject *parent) : QObject(parent)
{
    m_FtpServerFile = App::FtpDirName;
    m_DownloadFile = App::TempDir;
    m_UploadFile = App::TempDir;
    this->HostIP = App::FtpHostIP;
    this->HostPort = App::FtpPort;
    this->username = App::FtpUser;
    this->Password = App::FtpPasswd;
    m_IsUploading = false;
    m_IsDownloading = false;
    m_file_down = new QFile;
    m_file_up = new QFile;
    m_url = QUrl();
    m_url.setScheme("ftp");
    m_url.setHost(HostIP);
    m_url.setPort(HostPort);
    m_url.setUserName(username);
    m_url.setPassword(Password);
    connect(&m_manager, &QNetworkAccessManager::finished, this, &FtpClient::ManagerFinished);
}

QString FtpClient::GetDownloadFile() const
{
    return this->m_DownloadFile;
}

void FtpClient::SetDownloadFile(QString filename)
{
    this->m_DownloadFile = filename;
}

QString FtpClient::GetUploadFile() const
{
    return this->m_UploadFile;
}

void FtpClient::SetUploadFile(QString filename)
{
    this->m_UploadFile = filename;
}

QString FtpClient::GetFtpServerFile() const
{
    return this->m_FtpServerFile;
}

void FtpClient::SetFtpServerFile(QString filename)
{
    this->m_FtpServerFile = filename;
}

void FtpClient::put()
{
    if(m_FtpServerFile.isEmpty() || m_UploadFile.isEmpty()) return;
    if(m_IsUploading){
        QMessageBox::warning(nullptr,"上传失败","当前存在上传任务，请稍后重试");
        return;
    }
    m_IsUploading = true;
    m_file_up->setFileName(m_UploadFile);
    if(!m_file_up->open(QIODevice::ReadOnly)){
        Q_EMIT OpenLocalFileError(m_UploadFile);
        return;
    }
    m_url.setPath(m_FtpServerFile);
    QNetworkReply *putReply = m_manager.put(QNetworkRequest(m_url), m_file_up);
    connect(putReply, SIGNAL(finished()), this, SLOT(uploadFinished()));
    connect(putReply,&QNetworkReply::uploadProgress,this,&FtpClient::UploadProcess);
    connect(putReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onUploadError(QNetworkReply::NetworkError)));
}

void FtpClient::get()
{
    if(m_FtpServerFile.isEmpty() || m_DownloadFile.isEmpty()) return;

    if(m_IsDownloading){
        QMessageBox::warning(nullptr,"下载失败","当前存在下载任务，请稍后重试");
        return;
    }
    m_IsDownloading = true;
    m_file_down->setFileName(m_DownloadFile);

    if(!m_file_down->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Truncate)){
        Q_EMIT OpenLocalFileError(m_DownloadFile);
    }
    m_url.setPath(m_FtpServerFile);

    QNetworkRequest request(m_url);
    QNetworkReply *getReply = m_manager.get(request);
    connect(getReply, SIGNAL(finished()), this, SLOT(downloadFinished()));
    connect(getReply,&QNetworkReply::downloadProgress,this,&FtpClient::UpdatedownloadProcess);
    connect(getReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onUploadError(QNetworkReply::NetworkError)));
}

void FtpClient::ManagerFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Manager run successfully.";
    } else {
        qWarning() << "Manager run failed:" << reply->errorString();
        Q_EMIT ManagerReturnError();
    }
    reply->deleteLater();
}

void FtpClient::onUploadError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error);
    // 打印详细的错误信息
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply) {
        qWarning() << "Error details:" << reply->errorString();
    }
}

void FtpClient::UpdatedownloadProcess(uint64_t current, uint64_t Total)
{
    QNetworkReply *pReply = qobject_cast<QNetworkReply *>(sender());
    if(pReply->error()==QNetworkReply::NoError) {
        m_file_down->write(pReply->readAll());
    }
    Q_EMIT DownloadProcess(current,Total);
}

void FtpClient::uploadFinished()
{
    QNetworkReply *pReply = qobject_cast<QNetworkReply *>(sender());
    if(pReply->error()==QNetworkReply::NoError){
        m_file_up->close();
        m_IsUploading = false;
        pReply->close();
        pReply->deleteLater();
    }
}

void FtpClient::downloadFinished()
{
    QNetworkReply *pReply = qobject_cast<QNetworkReply *>(sender());
    if(pReply->error()==QNetworkReply::NoError){
        m_file_down->flush();
        m_file_down->close();
        m_IsDownloading = false;
        pReply->close();
        pReply->deleteLater();
        qDebug() << "download success";
        Q_EMIT DownloadFinish();
    }
}
