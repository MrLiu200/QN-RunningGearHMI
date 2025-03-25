#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <QObject>
//#include <QMutex>
#include <QtCore>
#include <QUrl>
#include <QFile>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "app.h"
class FtpClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString m_DownloadFile READ GetDownloadFile WRITE SetDownloadFile);
    Q_PROPERTY(QString m_UploadFile READ GetUploadFile WRITE SetUploadFile);
    Q_PROPERTY(QString m_FtpServerFile READ GetFtpServerFile WRITE SetFtpServerFile);
public:
    static FtpClient *Instance();
    explicit FtpClient(QObject *parent = nullptr);
    //读取or设置下载的文件名
    QString GetDownloadFile() const;
    void SetDownloadFile(QString filename);

    //读取or设置上传的文件名
    QString GetUploadFile() const;
    void SetUploadFile(QString filename);

    //读取or设置服务器的文件名
    QString GetFtpServerFile() const;
    void SetFtpServerFile(QString filename);

    // 上传文件
    void put();
    // 下载文件
    void get();

private:
    static FtpClient *self;
    QNetworkAccessManager m_manager;
    QString m_DownloadFile;         //下载的文件
    QString m_UploadFile;           //上传的文件
    QString m_FtpServerFile;        //服务器的文件名
    QString HostIP;                 //服务器IP
    int HostPort;                   //服务器端口
    QString username;               //用户名
    QString Password;               //密码
    QUrl m_url;
    QFile *m_file_up;               //上传文件指针
    QFile *m_file_down;             //下载文件指针
    bool m_IsUploading;               //是否在上传
    bool m_IsDownloading;             //是否在下载

private slots:
    //事件请求完成
    void ManagerFinished(QNetworkReply *reply);
    //上传时发生错误
    void onUploadError(QNetworkReply::NetworkError error);
    //下载进度处理
    void UpdatedownloadProcess(uint64_t current, uint64_t Total);
    //上传完成
    void uploadFinished();
    //下载完成
    void downloadFinished();
signals:
    void ManagerReturnError();
    void ManagerSSLError();
    void OpenLocalFileError(QString filename);
    void UploadProcess(uint64_t current, uint64_t Total);
    void DownloadProcess(uint64_t current, uint64_t Total);
    void DownloadFinish();
};

#endif // FTPCLIENT_H
