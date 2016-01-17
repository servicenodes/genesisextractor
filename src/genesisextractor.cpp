#include "genesisextractor.h"
#include "ui_genesisextractor.h"

#include <QFileDialog>
#include <QProcess>
#include <QStandardItemModel>

#include <qDebug>

#include <QTime>

void delay( int millisecondsToWait )
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

GenesisExtractor::GenesisExtractor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GenesisExtractor),
    index(0),
    status(STATUS::QUEUE_IDLE),
    datadir(QDir::currentPath() + "\\temp")
{
    //qDebug() << STATUS::STOPPED;
    //qDebug() << STATUS::KILLED;

    ui->setupUi(this);

    daemon = initializeDaemonList(getAvailableDaemons());//available_daemons);

    ui->startButton->setEnabled(daemon.length() > 0 ? true : false);

    if(daemon.length() > 0){
        initializeTreeView();
        updateTreeView();
    }
    clearTempFolder();
}

GenesisExtractor::~GenesisExtractor()
{
    // clearTempFolder();

    //stopEverything();

    /*delete exec;
    delete exec2;

    delete watcher;*/

    delete ui;
}

QStringList GenesisExtractor::getAvailableDaemons()
{
    ui->plainTextEdit_2->appendPlainText("> /daemon folder scanned.");
    QStringList nameFilter("*.exe");
    QDir directory(QDir::currentPath()+"//daemons//");
    QStringList daemons_list = directory.entryList(nameFilter);
    return daemons_list;
}

QList<daemon_internals> GenesisExtractor::initializeDaemonList(QStringList available_daemons)
{
    QList<daemon_internals> daemon_;
    for(int i = 0;i<available_daemons.length();i++){
        daemon_.append(daemon_internals(available_daemons[i],"","","waiting in queue..."));
    }
    return daemon_;
}

void GenesisExtractor::updateDaemonList(int index, QString pid, QString port, QString status)
{
    daemon[index].pid = pid;
    daemon[index].port = port;
    daemon[index].status = status;
}

void GenesisExtractor::clearTempFolder()
{
    ui->plainTextEdit_2->appendPlainText("> /temp folder cleared.");
    QDir dir(datadir);
    dir.setFilter( QDir::NoDotAndDotDot | QDir::Files );
    foreach( QString dirItem, dir.entryList() )
        dir.remove( dirItem );

    dir.setFilter( QDir::NoDotAndDotDot | QDir::Dirs );
    foreach( QString dirItem, dir.entryList() )
    {
        QDir subDir( dir.absoluteFilePath( dirItem ) );
        subDir.removeRecursively();
    }
}

void GenesisExtractor::initializeTreeView()
{
    model = new QStandardItemModel(0,3,this);

    QStringList headerLabels;
    headerLabels << "Name" << "PID" << "Port" << "Status";
    model->setHorizontalHeaderLabels(headerLabels);

    ui->treeView->setModel(model);
    ui->treeView->header()->resizeSection(0,200);
    ui->treeView->header()->resizeSection(1,60);
    ui->treeView->header()->resizeSection(2,60);
    ui->treeView->header()->setSectionResizeMode(3,QHeaderView::Stretch);
}

void GenesisExtractor::updateTreeView()
{
    for(int i=0;i<daemon.length();i++){
        model->removeRow(0);
    }
    for(int i=0;i<daemon.length();i++){
        QList<QStandardItem*> list;
        list.append(new QStandardItem(daemon[i].name));
        list.append(new QStandardItem(daemon[i].pid));
        list.append(new QStandardItem(daemon[i].port));
        list.append(new QStandardItem(daemon[i].status));

        model->appendRow(list);
    }
}

void GenesisExtractor::on_startButton_clicked()
{
    if(status == STATUS::QUEUE_IDLE){
        ui->startButton->setText("STOP EXTRACTION");
        status = STATUS::QUEUE_STARTED;
        startQueue();
    } else{
        ui->startButton->setText("START EXTRACTION");
        status = STATUS::QUEUE_IDLE;
        stopEverything();
    }
}

void GenesisExtractor::startQueue()
{
    qDebug() << status << "startQueue()";
    ui->plainTextEdit_2->appendPlainText("> Queue started...");
    continueQueue();
}

void GenesisExtractor::continueQueue()
{
    qDebug() << status << "continueQueue()";
    if(status == STATUS::QUEUE_STARTED){
        QString file = QDir::currentPath() + "\\daemons\\" + daemon[index].name;

        //exec = new QProcess(this);

        disconnect(&exec,SIGNAL(started()),0,0);
        disconnect(&exec,SIGNAL(finished(int)),0,0);

        connect(&exec,SIGNAL(started()), this, SLOT(exec_started()));
        connect(&exec,SIGNAL(finished(int)), this, SLOT(exec_killed()));

        //exec->start(file,  QStringList() << QString("-datadir="+datadir) << "-daemon" << "-rpcuser=123" << "-rpcpassword=456");
        exec.start(file,  QStringList() << QString("-datadir="+datadir) << "-daemon=0" << "-listen=1" << "-rpcuser=123" << "-rpcpassword=456");
    }
}

void GenesisExtractor::stopEverything()
{
    qDebug() << status << "stopEverything()";

    //ui->startButton->setEnabled(false);
    status = STATUS::QUEUE_STOPPED;


    ui->plainTextEdit_2->appendPlainText("> Queue stopped!");

    disconnect(&exec,SIGNAL(started()),0,0);
    disconnect(&exec,SIGNAL(finished(int)),0,0);
    disconnect(&exec2,SIGNAL(started()),0,0);
    disconnect(&exec2,SIGNAL(finished(int)),0,0);

    exec.kill();
    exec2.kill();

    disconnect(&watcher, SIGNAL(directoryChanged(QString)),0,0);
    watcher.removePath(datadir);

    delay(1000);

    clearTempFolder();
}

void GenesisExtractor::closeEvent(QCloseEvent *)
{
    qDebug() << "close clicked";
    //stopEverything();

}

void GenesisExtractor::exec_started()
{
    qDebug() << status << "exec_started()";
    if(status == STATUS::QUEUE_STARTED){
        status = STATUS::PROCESS_STARTED;

        pid = exec.processId();

        updateDaemonList(index, QString::number(pid), "", "Process started");
        updateTreeView();

        ui->plainTextEdit_2->appendPlainText("> Process with PID " + QString::number(exec.processId()) + " started.");
        qDebug() << "process started" << exec.processId() << exec.program();

        //watcher = new QFileSystemWatcher(this);
        watcher.addPath(datadir);
        //disconnect(watcher, SIGNAL(directoryChanged(QString)),0,0);
        connect(&watcher, SIGNAL(directoryChanged(QString)), this, SLOT(blockDataCreated()));
    }
}

void GenesisExtractor::exec_killed()
{
    qDebug() << status << "exec_killed()";
    ui->startButton->setEnabled(true);
    //status = 0;

    //clearTempFolder();
    ui->plainTextEdit_2->appendPlainText("> Process with PID " + QString::number(pid) + " killed.");

}

void GenesisExtractor::blockDataCreated()
{
    qDebug() << status << "blockDataCreated()";
    if(status == STATUS::PROCESS_STARTED){
        status = STATUS::BLOCK_CREATION;

        disconnect(&watcher, SIGNAL(directoryChanged(QString)),0,0);
        watcher.removePath(datadir);

        updateDaemonList(index, QString::number(pid), "", "BlockData created");
        updateTreeView();

        ui->plainTextEdit_2->appendPlainText("> BlockData for "+daemon[index].name+" created.");

        blkdata_style = 0;

        timer.start();

        checkExistsBlockData();
    }
}

void GenesisExtractor::checkExistsBlockData()
{
    qDebug() << status << "checkExistsBlockDat()";
    if(status == STATUS::BLOCK_CREATION){
        //bitcoin style
        if(blkdata_style == 0){
            int blockdat_nr = 0;
            blockfile.setFileName(datadir+"//blocks//" +"blk"+QString("%5").arg(blockdat_nr, 5, 10, QChar('0'))+".dat");
        }

        //namecoin style
        if(blkdata_style == 1){
            int blockdat_nr = 1;
            blockfile.setFileName(datadir+"//" +"blk"+QString("%4").arg(blockdat_nr, 4, 10, QChar('0'))+".dat");
        }

        if(!blockfile.exists()){
            if(timer.elapsed() >= 3000){
                blkdata_style += 1;
                ui->plainTextEdit_2->appendPlainText("> trying different blk_dat location (style "+QString::number(blkdata_style)+")...");
                timer.start();
            }
            ui->plainTextEdit_2->appendPlainText("> waiting for GenesisBlock...");
            delay(500);
            checkExistsBlockData();
            return void();
        }

        status = STATUS::BLOCK_ANALYSIS;

        analyzeBlockData();
    }
}

void GenesisExtractor::analyzeBlockData()
{
    qDebug() << status << "analyzeBlockData()";
    if(status == STATUS::BLOCK_ANALYSIS){
        ui->plainTextEdit_2->appendPlainText("> analyzing GenesisBlock...");

        blockfile.open(QIODevice::ReadOnly);
        blockchain = blockfile.readAll();

        magic_bytes = blockchain.mid(0,4).toHex().toStdString().c_str();

        merkle_root = reverseBytesToString(blockchain.mid(44,32));

        hex_time = reverseBytesToString(blockchain.mid(76, 4));

        unsigned int blocklength;
        memcpy(&blocklength, blockchain.mid(4,4), sizeof(unsigned int));

        unsigned int blocktime;
        memcpy(&blocktime, blockchain.mid(76, 4),sizeof(unsigned int));

        genesisblock_time = QString::number(blocktime);

        blockfile.close();

        status = STATUS::PORT_INIT;

        scanPortUsed();
    }
}
void GenesisExtractor::scanPortUsed()
{
    qDebug() << status << "scanPortUsed()";
    if(status == STATUS::PORT_INIT){
        status = STATUS::PORT_TRY;

        ui->plainTextEdit_2->appendPlainText("> scan used Port");

        //C:\Windows\system32>netstat -ano | findstr 2124 | findstr 0.0.0.0: | findstr /V 127.0.0.1

        updateDaemonList(index, QString::number(pid), "", "Trying Connection...");
        updateTreeView();

        //exec2 = new QProcess(this);

        disconnect(&exec2,SIGNAL(started()),0,0);
        disconnect(&exec2,SIGNAL(finished(int)),0,0);

        connect(&exec2,SIGNAL(started()), this, SLOT(netstat_started()));
        connect(&exec2,SIGNAL(finished(int)), this, SLOT(netstat_killed()));

        exec2.start("cmd.exe", QStringList() << "/C" << QString("netstat -ano | findstr " + QString::number(pid)+" | findstr 0.0.0.0 | findstr /V 127.0.0.1"));
    }
}

void GenesisExtractor::netstat_started()
{
    ui->plainTextEdit_2->appendPlainText("> Port-Scan started...");
}

void GenesisExtractor::netstat_killed()
{
    qDebug() << status << "netstat_killed()";
    if(status == STATUS::PORT_TRY){
        ui->plainTextEdit_2->appendPlainText("> Port-Scan finished...");
        QString result = exec2.readAll().toStdString().c_str();
        qDebug() << result;
        qDebug() << "before port.splitting";
        if(result.length() > 0){
            status = STATUS::PORT_SUCCESS;
            port = result.split(":")[1].split(" ")[0];

            updateDaemonList(index, "", port, "Processed.");
            updateTreeView();

            //ExplorerID; DaemonName; MagicBytes; MerkleRoot; Date of Creation; Port;
            ui->plainTextEdit->appendPlainText(QString(magic_bytes+hex_time+";"+daemon[index].name.split(".exe")[0]+";"+magic_bytes+";"+merkle_root+";"+genesisblock_time+";"+port+";"));

            qDebug() << "before cleanUp()";
            cleanUp();
        } else{

            //status = 6;
            ui->plainTextEdit_2->appendPlainText("> Port-Scan retry...");

            delay(1000);
            disconnect(&exec2,SIGNAL(started()),0,0);
            disconnect(&exec2,SIGNAL(finished(int)),0,0);

            connect(&exec2,SIGNAL(started()), this, SLOT(netstat_started()));
            connect(&exec2,SIGNAL(finished(int)), this, SLOT(netstat_killed()));

            exec2.start("cmd.exe", QStringList() << "/C" << QString("netstat -ano | findstr " + QString::number(pid)+" | findstr 0.0.0.0 | findstr /V 127.0.0.1"));

        }
    }
}

void GenesisExtractor::cleanUp()
{
    qDebug() << status << "cleanUp()";
    if(status == STATUS::PORT_SUCCESS){

        status = STATUS::CLEANUP;
        disconnect(&exec,SIGNAL(started()),0,0);
        disconnect(&exec,SIGNAL(finished(int)),0,0);
        disconnect(&exec2,SIGNAL(started()),0,0);
        disconnect(&exec2,SIGNAL(finished(int)),0,0);

        exec.kill();
        exec2.kill();

        disconnect(&watcher, SIGNAL(directoryChanged(QString)),0,0);
        watcher.removePath(datadir);

        delay(1000);

        clearTempFolder();

        delay(500);

        if(index <daemon.length()-1){
            index ++;
            //QModelIndex index = model->index(0, 0, QModelIndex());
            ui->treeView->scrollTo(model->index(index, 0, QModelIndex()));
            status = STATUS::QUEUE_STARTED;
            continueQueue();
        } else{
            ui->plainTextEdit_2->appendPlainText("> All jobs finished...");
            ui->startButton->setText("START EXTRACTION");
            status = STATUS::QUEUE_IDLE;
        }
    }
}


QString GenesisExtractor::reverseBytesToString(QByteArray byte_ar){
    QByteArray rev = byte_ar;
    std::reverse(rev.begin(), rev.end());//, rev.begin());
    return QString(rev.toHex());
}


