#ifndef WIDGET_H
#define WIDGET_H
#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QMovie>
#include <QMouseEvent>
#include <QMenu>
#include <QList>
#include <QVBoxLayout>
#include <QRandomGenerator>
#include <QScreen>
#include <QFileDialog>
QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE
class Widget ;
class SettingWidget : public QWidget
{
    Q_OBJECT
public:
    static SettingWidget* getSettingWidget(Widget *target);
    ~SettingWidget();
private slots:
    void on_sizeButton_clicked();

    void on_closeButton_clicked();

    void on_sizeSlider_valueChanged(int value);

    void on_skinButton_clicked();

    void on_fileButton1_clicked();

    void on_commitButton1_clicked();

    void on_fileButton2_clicked();

    void on_commitButton2_clicked();

    void on_fileButton3_clicked();

    void on_commitButton3_clicked();

private:
    SettingWidget(Widget *target);
    static SettingWidget *settingWidget_Instance;
    Widget *target;
    QString filePath;
    Ui::Widget *ui;
};
class MyGifLabel:public QLabel
{
    Q_OBJECT
public:
    MyGifLabel(QWidget *parent = nullptr): QLabel(parent) {
        setScaledContents(true);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);//设置可拉伸
        //setStyleSheet("background-color:green;");
    }
    ~MyGifLabel(){};
protected:
    void resizeEvent(QResizeEvent* event) override {
        QLabel::resizeEvent(event);
        QMovie* currentMovie = movie(); // 获取当前的QMovie指针
        if (currentMovie) {
            currentMovie->setScaledSize(this->size());
            this->setMinimumSize(150,150);
        }
    }

};
class RandomMoving:public QObject
{
    Q_OBJECT
public:
    RandomMoving(QWidget* target);
    ~RandomMoving(){}
    void stop_move(){
        moveTimer->stop();
        changetimer->stop();
    }
    void continue_move(){
        moveTimer->start(20);
        changetimer->start(1000);
    }
private:
    int speed=50;
    QWidget* target;
    int isMovingRight;
    QTimer *moveTimer;
    QTimer *changetimer;
    QRect screenRect;
    void updatePosition();
};
class Widget : public QWidget
{
    Q_OBJECT//一个宏，允许使用信号和槽
    friend class SettingWidget;
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
public slots:
    void showMenu(const QPoint& pos );
protected:
    void catWalk();
    void catIdle();
    void catSmoke();
    void catRide();
    // void loadPicture(QPixmap* spriteSheet,QList<QPixmap> &frames,int r,int c,int width,int height,int num);
private:
    Ui::Widget *ui;
    QTimer *Move_Timer;
    // QTimer *walkTimer;
    QTimer *smokeTimer;
    QTimer *rideTimer;
    // QPixmap *oriPicture;
    // QList<QPixmap> frames;
    // int frameIndex=0;
    // void updateFrame();
    RandomMoving *mover=nullptr;

    QMenu *menu;
    QTimer *dragTimer;
    bool isDragging = false;
    bool isMoving = false;
    bool isPausing = false;
    QPoint startPos;
    MyGifLabel *gifLabel;
    QMovie *idleMovie;   QString idlefile;
    QMovie *smokeMovie; QString smokefile;
    QMovie *rideMovie; QString ridefile;
     QMovie *walkMovie; QString walkfile;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event)override;
    void mouseReleaseEvent(QMouseEvent *event)override;

    SettingWidget* setting=nullptr;
    QVBoxLayout *layout;
public:
    static int widget_size;
};
#endif // WIDGET_H
