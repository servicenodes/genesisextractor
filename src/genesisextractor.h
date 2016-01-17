#ifndef GENESISEXTRACTOR_H
#define GENESISEXTRACTOR_H

#include <QCloseEvent>
#include <QElapsedTimer>
#include <QFile>
#include <QFileSystemWatcher>
#include <QMainWindow>
#include <QProcess>
#include <QStandardItemModel>

namespace STATUS { enum
{
    QUEUE_STOPPED,
    QUEUE_IDLE,
    QUEUE_STARTED,
    PROCESS_STARTED,
    BLOCK_CREATION,
    BLOCK_ANALYSIS,
    PORT_INIT,
    PORT_TRY,
    PORT_SUCCESS,
    CLEANUP
};
}

struct daemon_internals{
    QString name;
    QString pid;
    QString port;
    QString status;

    daemon_internals(QString name_,
                     QString pid_,
                     QString port_,
                     QString status_):
        name(name_),
        pid(pid_),
        port(port_),
        status(status_)
    {}
};

namespace Ui {
class GenesisExtractor;
}

class GenesisExtractor : public QMainWindow
{
    Q_OBJECT

public:
    explicit GenesisExtractor(QWidget *parent = 0);
    ~GenesisExtractor();

    void clearTempFolder();

    QStringList getAvailableDaemons();
    QList<daemon_internals> initializeDaemonList(QStringList available_daemons);
    void updateDaemonList(int index, QString pid, QString port, QString status);

    void initializeTreeView();
    void updateTreeView();

    void startQueue();
    void continueQueue();

    void stopEverything();

    void closeEvent(QCloseEvent *);

private slots:
    void on_startButton_clicked();

    void exec_started();
    void exec_killed();

    void blockDataCreated();

    void checkExistsBlockData();

    void analyzeBlockData();

    QString reverseBytesToString(QByteArray byte_ar);

    void scanPortUsed();

    void netstat_started();
    void netstat_killed();

    void cleanUp();
private:
    Ui::GenesisExtractor *ui;
    QStandardItemModel *model;

    QElapsedTimer timer;

    QList<daemon_internals> daemon;
    QProcess exec;
    QProcess exec2;

    QFileSystemWatcher watcher;

    QFile blockfile;
    QByteArray blockchain;

    int index;
    int status;
    int pid;
    QString port;

    QString magic_bytes;
    QString merkle_root;
    QString genesisblock_hash;
    QString genesisblock_time;
    QString hex_time;

    int blkdata_style;

    QString datadir;
};

#endif // GENESISEXTRACTOR_H
