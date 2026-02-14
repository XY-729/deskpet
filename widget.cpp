#include "widget.h"
#include "./ui_widget.h"
SettingWidget *SettingWidget::settingWidget_Instance=nullptr;
int Widget::widget_size=150;
void Widget::updateFrame()
{
    gifLabel->setPixmap(frames[frameIndex]);
    frameIndex++;
    if(frameIndex==15)
        frameIndex=0;
}
void RandomMoving::updatePosition(){
    int newX = target->pos().x();
    int newY = target->pos().y();
    if (isMovingRight) {
        newX += speed;
    } else {
        newX -= speed;
    }
    // 左边界
    if (newX <= screenRect.left()) {
        newX = screenRect.left();
        isMovingRight = true;
    }
    // 右边界
    else if (newX + Widget::widget_size >= screenRect.right()) {
        newX = screenRect.right() - Widget::widget_size;
        isMovingRight = false;
    }
    target->move(newX,newY);
}
RandomMoving::RandomMoving(QWidget* target):target(target),QObject(){
    QScreen *screen = QApplication::primaryScreen();
    screenRect = screen->availableGeometry();
    speed=10;
    changetimer=new QTimer(this);
    moveTimer=new QTimer(this);
    isMovingRight = QRandomGenerator::global()->generate() % 2 == 0;
    moveTimer->start(20);
    changetimer->start(1000);
    connect(changetimer,&QTimer::timeout,this,[=](){isMovingRight = QRandomGenerator::global()->generate() % 2 == 0;
    });
    connect(moveTimer, &QTimer::timeout, this, &RandomMoving::updatePosition);
}
void Widget::catRide()
{
    gifLabel->clear();
    gifLabel->setMovie(rideMovie);
    rideMovie->start();
    mover=new RandomMoving(this);
    resize(Widget::widget_size,Widget::widget_size);
}
void Widget::catMove()
{
    resize(500*Widget::widget_size/150,97*Widget::widget_size/150);
    frameIndex=6;
    gifLabel->clear();
    walkTimer->start(100);
}
void Widget::catSmoke()
{

    walkTimer->stop();
    gifLabel->clear();
    gifLabel->setMovie(smokeMovie);
    smokeMovie->setSpeed(90);
    smokeMovie->start();
    resize(Widget::widget_size,Widget::widget_size);
}
void Widget::catIdle()
{
    gifLabel->clear();
    gifLabel->setMovie(idleMovie);
    idleMovie->start();
}
void Widget::showMenu(const QPoint& pos)
{
    menu->clear();
    menu->addAction("关闭宠物", this, &Widget::close);
    if(idleMovie->state()==QMovie::Running)
    {
        menu->addAction("暂停",this,[=](){
        idleMovie->setPaused(true);
        isPausing=true;
        });}
    else if(idleMovie->state()==QMovie::Paused)
    {
        menu->addAction("继续",this,[=](){
        idleMovie->setPaused(false);
        isPausing=false;
        });}
    menu->addAction("设置",this,[=](){
        SettingWidget::getSettingWidget(this);});
    menu->move(QCursor::pos());
    menu->show();
}
void Widget::loadPicture(QPixmap* oriPicture,
QList<QPixmap> &frames,int r,int c,int width,int height,int num)
{
    int cnt=0;
    frames.clear();
    for(int i=0;i<r;i++)
    {
        for(int j=0;j<c;j++)
        {
            if(cnt==num)
                return;
            int x=j*width;int y=i*height;
            QPixmap frame = oriPicture->copy(x+50, y, width, height);
            //frame.save(QString("%1.png").arg(cnt));
            frames.append(frame);
            cnt++;
        }
    }
}
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    resize(150,150);//初始尺寸
    // move(1000,500);
    QVBoxLayout *layout = new QVBoxLayout(this);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    this->setWindowFlag(Qt::FramelessWindowHint);//设置窗口无边框
    setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明
    gifLabel = new MyGifLabel(this);
    idleMovie = new QMovie(this);
    idleMovie->setFileName(":/image/catIdle.gif");
    smokeMovie = new QMovie(this);
    smokeMovie->setFileName(":/image/smoke.gif");
    rideMovie=new QMovie(this);
    rideMovie->setFileName(":/image/catRide.gif");
    menu=new QMenu(gifLabel);
    oriPicture=new QPixmap(":/image/catMove.png");
    oriPicture->scaled(400, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    Widget::loadPicture(oriPicture,frames,8,14,400,97,15);//加载精灵图
    layout->addWidget(gifLabel);
    layout->setContentsMargins(0, 0, 0, 0);

    Move_Timer=new QTimer(this);
    walkTimer=new QTimer(this);
    smokeTimer=new QTimer(this);
    dragTimer=new QTimer(this);
    rideTimer=new QTimer(this);

    connect(dragTimer,&QTimer::timeout,this,[=](){isDragging=true;});
    Widget::catIdle();//初始待机
    rideTimer->start(2000);
    connect(rideTimer,&QTimer::timeout,this,[=](){
        isMoving=true;
        Widget::catRide();
        rideTimer->stop();
        Move_Timer->start(6000);
    });
    connect(smokeTimer,&QTimer::timeout,this,[=](){
        smokeMovie->stop();
        smokeTimer->stop();
        if(isMoving==false)
        {Widget::catMove();isMoving=true;Move_Timer->start(1900);rideTimer->stop();}
        else
        {Widget::catIdle();isMoving=false;Move_Timer->stop();rideTimer->start(2000);};
    });
    connect(walkTimer,&QTimer::timeout,this,&Widget::updateFrame);
    connect(Move_Timer,&QTimer::timeout,this,[=](){
        if(mover!=nullptr)
        {delete mover;mover=nullptr;}
        Widget::catSmoke();
        smokeTimer->start(300);
        resize(Widget::widget_size,Widget::widget_size);
    });

    gifLabel->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(gifLabel, &QLabel::customContextMenuRequested,this, &Widget::showMenu);//右键显示菜单

}
Widget::~Widget()
{
    delete ui;
    delete oriPicture;
}

//鼠标拖动实现
void Widget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging=false;
        dragTimer->start(200);
        idleMovie->setPaused(true);
        startPos=event->pos();
        event->accept();
        QWidget::mousePressEvent(event);
    }
}
void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if(isDragging==true)
    {
        QPoint delta = event->pos() - startPos; // 鼠标移动的偏移量
        move(pos() + delta);
        event->accept();
        QWidget::mouseMoveEvent(event);
    }
}
void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    dragTimer->stop();
    if (event->button() == Qt::LeftButton) {
        if(isDragging==true){
            if(isPausing!=true)
                idleMovie->setPaused(false);
        }
        else if(isDragging==false&&isMoving==false){
            Widget::catSmoke();
            smokeTimer->start(300);
        }
    }
    else if(event->button() == Qt::RightButton){}
    event->accept();
    QWidget::mouseReleaseEvent(event);
}

SettingWidget* SettingWidget::getSettingWidget(Widget *target){
    if(settingWidget_Instance==nullptr)
    {SettingWidget::settingWidget_Instance=new SettingWidget(target);
    SettingWidget::settingWidget_Instance->show();}
    return settingWidget_Instance;
}
SettingWidget::SettingWidget(Widget *target) :QWidget(),ui(new Ui::Widget),target(target){
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    resize(600, 400);
    setWindowTitle("设置界面");
    ui->sizeSlider->hide();
    ui->skinwidget->hide();
    //setStyleSheet("background-color: white;");}
    connect(this, &QObject::destroyed, [](){
        SettingWidget::settingWidget_Instance = nullptr;
    });
}

SettingWidget::~SettingWidget(){}
void SettingWidget::on_sizeButton_clicked(){
    ui->skinwidget->hide();
    ui->sizeSlider->show();
}
void SettingWidget::on_closeButton_clicked(){
    this->close();
}
void SettingWidget::on_skinButton_clicked()
{
    ui->sizeSlider->hide();
    ui->skinwidget->show();
}
void SettingWidget::on_sizeSlider_valueChanged(int value)
{
    if (!target) return;
    const int minSize = 150;   // 最小尺寸
    const int maxSize = 400;   // 最大尺寸
    double scale = static_cast<double>(value) / 99;  // 0.0 ~ 1.0
    int size = minSize + static_cast<int>((maxSize - minSize) * scale);
    Widget::widget_size=size;
    target->resize(size, size);
}
void SettingWidget::on_fileButton1_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "选择皮肤文件",
        "....",
        "文件(*.qss *.png *.jpg *.gif)"
        );

    // 2. 如果用户选择了文件（不是点击取消），就把路径填充到 LineEdit
    if (!filePath.isEmpty()) {
        ui->filetext1->setText(filePath);
    }
}

