#ifndef KM4TEMPLATE_H
#define KM4TEMPLATE_H

#include <QObject>
#include <QPainter>
#include <QPrinter>
#include <QTextDocument>
#include <QTextCursor>
#include "reporthelp.h"
class KM4Template : public QObject
{
    Q_OBJECT
public:
    explicit KM4Template(QObject *parent = nullptr);

private:
    // 数据定义
    struct CellData {
        QString value;
        QColor color;
    };

    struct RowData {
        QString carNumber;
        QString axisNumber;
        CellData side1;
        CellData side2;
        CellData gearbox;
        CellData motorDrive;
        CellData motorNonDrive;
    };

    QList<ReportHelp::PDFDEVICE> DeviceInfoList;
    QStringList WagonList;
    QString StartTime;
    QString EndTime;
    QString CarNum;
    QString SavePath;
    QVector<QVector<QString>> resultdata;//诊断的结果
private:
    void insertHeader(QTextCursor &cursor);
    void insertParagraph(QTextCursor &cursor, QString dropName);
    void DrawCover(QTextCursor &cursor,QString Device);
    void DrawNotice(QTextCursor &cursor);
    void DrawTableInfo(QTextCursor &cursor, QString Device, QString StartDate, QString EndData);
    void DrawResultOverview(QTextCursor &cursor, QTextDocument *doc, QString Device);
    void DrawResultTable(QTextCursor &cursor, const QVector<QVector<QString> > &ResultData);
    void DrawResultSuggestions(QTextCursor &cursor, const QVector<QVector<QString> > &ResultData);
    void DrawDisclaimers(QTextCursor &cursor);

    QColor getStatusColor(const QString &status);
    RowData createRowData(const QString &carNumber, const QString &axisNumber,
                          const QString &side1Status, const QString &side2Status,
                          const QString &gearboxStatus, const QString &motorDriveStatus,
                          const QString &motorNonDriveStatus);
    QString getHandlingSuggestions(QVector<QString>Result,QStringList HeadList);

public:
    void SetData(QString carnumber,QString StartDate,QString EndData,QString savefile, QStringList wagonlist, QList<ReportHelp::PDFDEVICE> devicelist);
    bool drawReport();
signals:

};

#endif // KM4TEMPLATE_H
