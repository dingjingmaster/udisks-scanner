//
// Created by dingjing on 23-6-13.
//
#include <QApplication>
#include "../3thrd/singleton/singleton-app-gui.h"

#include <QTextCodec>

#include <gio/gio.h>

#include <QFile>
#include <QTimer>
#include <QDebug>
#include <QProcess>
#include <QPushButton>
#include <QStyleFactory>

#include "../common/log.h"
#include "ui/main-window.h"
#include "ui/message-box.h"


const char*             gLogPath = "/tmp/udisk-scanner.log";
char*                   gDBPath = nullptr;

void messageOutput (QtMsgType type, const QMessageLogContext& context, const QString& msg);

int main (int argc, char* argv[])
{
    // 初始化日志输出
    g_log_set_writer_func (log_handler, nullptr, nullptr);
    qInstallMessageHandler (messageOutput);

    QCoreApplication::setApplicationVersion (PACKAGE_VERSION);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
    QTextCodec::setCodecForLocale (QTextCodec::codecForName ("utf-8"));

    INFO("%s start running...", PACKAGE_NAME);

    static struct sigaction sigAction;
    sigAction.sa_sigaction = signal_handler;
    sigAction.sa_flags |= SA_SIGINFO;

    if (sigaction(SIGSEGV, &sigAction, nullptr)) {
        LOG_ERROR ("sigaction error: %s", strerror (errno));
        return errno;
    }

    SingletonApp app (argc, argv, PACKAGE_NAME"");
    SingletonApp::setStyle ("Fusion");
    SingletonApp::setApplicationName(PACKAGE_NAME);
    SingletonApp::setApplicationDisplayName (PACKAGE_NAME);

    QPalette palette = QApplication::palette();
    QColor red = qRgb(255, 138, 140);
    palette.setColor (QPalette::All, QPalette::Highlight, red);
    palette.setColor (QPalette::All, QPalette::HighlightedText, Qt::white);
    QApplication::setPalette (palette);


    g_autofree char* path1 = nullptr;
    if (g_str_has_prefix(argv[0], "/")) {
        path1 = g_strdup(argv[0]);
    }
    else {
        path1 = g_strdup_printf("%s/%s", g_get_current_dir(), argv[0]);
    }

    g_return_val_if_fail(path1, -1);
    g_autofree char* pathDir = g_path_get_dirname (path1);
    g_return_val_if_fail(pathDir, -2);
    g_autofree char* cmd = g_strdup_printf ("cd %s && %s/policyfilter.sh &> /dev/null", pathDir, pathDir);
    LOG_WARNING("launch pf cmd: %s", cmd);
    system (cmd);
    gDBPath = g_strdup_printf ("%s/../dat/db_task/EstDlpSEDataBase.db", pathDir);
    LOG_DEBUG("DB Path: %s", gDBPath);

    // 检查授权文件
    g_autofree char* licFile = g_strdup_printf ("%s/../dat/lic_result", pathDir);

    auto* tim = new QTimer;
    tim->setInterval (1000);
    QTimer::connect (tim, &QTimer::timeout, [=] () {
        if (QFile::exists (licFile)) {
            tim->stop();
            QFile file(licFile);
            file.open (QIODevice::ReadOnly);
            if (file.readAll().trimmed().contains ("1")) {
                // 未授权
                file.close();
                file.remove();
                MessageBox msg ("提示", "工具无法在未受权的介质中运行!");
                auto btn = msg.addButton ("确定");
                QObject::connect (btn, &QPushButton::clicked, [&] (bool) {
                    msg.done (0);
                });
                msg.exec();
                ::exit (0);
            }
            else {
                file.close();
                file.remove();
                tim->deleteLater();
            }
        }
    });
    tim->start ();

    MainWindow ew;
    ew.setWindowTitle ("数据安全检查工具——单机版");
    QFile file ("://data/stylesheet.qss");
    if (file.open (QFile::ReadOnly)) {
        ew.setStyleSheet (file.readAll ());
        file.close ();
    }

    ew.show();

    int ret = SingletonApp::exec();

    INFO("%s exited!\n", PACKAGE_NAME);

    return ret;
}

void messageOutput (QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    GLogLevelFlags  level = G_LOG_LEVEL_DEBUG;

    switch (type) {
        case QtDebugMsg: {
            level = G_LOG_LEVEL_DEBUG;
            break;
        }
        case QtWarningMsg: {
            level = G_LOG_LEVEL_WARNING;
            break;
        }
        case QtFatalMsg:
        case QtCriticalMsg: {
            level = G_LOG_LEVEL_ERROR;
            break;
        }
        case QtInfoMsg: {
            level = G_LOG_LEVEL_INFO;
            break;
        }
        default: {
            level = G_LOG_LEVEL_DEBUG;
            break;
        }
    }

    unsigned int line = context.line;
    const char* file = context.file ? context.file : "";
    const char* function = context.function ? context.function : "";

    LOG_RAW(level, file, line, function, msg.toUtf8().constData());
}
