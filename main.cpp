#include "widget.h"

#include <QApplication>//这是一个应用程序类
//       命令行变量数     命令行变量数组
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);//这是应用程序对象
    Widget w;//窗口对象
    w.show();
    return a.exec();//进入循环
}
