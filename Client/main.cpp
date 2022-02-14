#include "mainwindow.h"

#include "qdebug.h"
#include <QUrl>
#include <QDir>
#include <QMessageBox>

#include <QApplication>
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QtAndroid>

bool checkPermission()
{//安装程序授权读写权限，没有权限则无法读写创建文件
    QtAndroid::PermissionResult r = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
    if(r == QtAndroid::PermissionResult::Denied) {
        QtAndroid::requestPermissionsSync( QStringList() << "android.permission.WRITE_EXTERNAL_STORAGE" );
        r = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
        if(r == QtAndroid::PermissionResult::Denied) {
             return false;
        }
   }
   return true;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    /*-------------------------------------*/
    if(checkPermission())
    {///确定权限
        /* 创建folder文件夹*/
        QDir *folder = new QDir;
        QUrl url("file:///storage/emulated/0/UIDdate");//文件的存储路径
        QString path = url.toLocalFile();  //获取到的路径
        bool exist = folder->exists(path);

        if(exist)
        {
           // QMessageBox::information(0,"found","Dir already exits！");//在最初的调试时可以通过这个判断是否创建成功
            qDebug()<<"folder"<<exist;
        }
        else
        {//创建folder文件夹
            bool ok = folder->mkdir(path);
            if(ok)
                QMessageBox::warning(0, "CreateDir", "Create Dir success!");
            else
                QMessageBox::warning(0, "CreateDir", "Create Dir fail");
        }
    }
    /*-------------------------------------*/
    w.show();
    return a.exec();
}
