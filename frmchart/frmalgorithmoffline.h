#ifndef FRMALGORITHMOFFLINE_H
#define FRMALGORITHMOFFLINE_H

#include <QWidget>
#include <QLineEdit>
#include <QGroupBox>
namespace Ui {
class FrmAlgorithmOffline;
}

class FrmAlgorithmOffline : public QWidget
{
    Q_OBJECT

public:
    explicit FrmAlgorithmOffline(QWidget *parent = nullptr);
    ~FrmAlgorithmOffline();

private:
    Ui::FrmAlgorithmOffline *ui;
    struct DIAGNOSTICRESULTS {
        QString filename;
        QString results;
        QStringList faultFre;
        QStringList DBValues;
        QString Oldresults;
        QString BearingInfo;
    };
    QString folderPath;
    QList<QLineEdit *> txt_frequency;
    QList<QLineEdit *> txt_dbbaoluo;
    QList<QLineEdit *> txt_dbpinpu;
    QList<QLineEdit *> txt_dbother;
    QList<DIAGNOSTICRESULTS> DiagnosticResults;
//    QList<QGroupBox *> Groups_bearing;

private:
    void InitFrm();
    void LoadFile();

    void LoadingResults(QString fileName);
    void clearAll();

private slots:
    void on_RunbtnClicked();
    void on_EchoDBClichek();
    QString RunAlgorithm(QString fileName);

    void on_btn_reimport_clicked();
    void on_btn_allrun_clicked();
};

#endif // FRMALGORITHMOFFLINE_H
