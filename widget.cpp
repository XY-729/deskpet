#include "widget.h"
#include "./ui_widget.h"
SettingWidget *SettingWidget::settingWidget_Instance=nullptr;
int Widget::widget_size=150;
// void Widget::updateFrame()
// {
//     gifLabel->setPixmap(frames[frameIndex]);
//     frameIndex++;
//     if(frameIndex==15)
//         frameIndex=0;
// }
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
    speed=5;
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
void Widget::catWalk()
{
    gifLabel->clear();
    gifLabel->setMovie(walkMovie);
    walkMovie->start();
    resize(Widget::widget_size,Widget::widget_size);
}
void Widget::catSmoke()
{
    gifLabel->clear();
    gifLabel->setMovie(smokeMovie);
    smokeMovie->setSpeed(90);
    smokeMovie->start();
    resize(Widget::widget_size,Widget::widget_size);
}
void Widget::catIdle()
{
    if(mover)
        mover->stop_move();
    gifLabel->clear();
    gifLabel->setMovie(idleMovie);
    idleMovie->start();
}
void Widget::showMenu(const QPoint& pos)
{
    menu->clear();
    menu->addAction("关闭宠物", this, &Widget::close);
    if(idleMovie->state()==QMovie::Running||rideMovie->state()==QMovie::Running)
    {
        menu->addAction("暂停",this,[=](){
        idleMovie->setPaused(true);
        rideMovie->setPaused(true);
        if(mover!=nullptr)
            mover->stop_move();
        isPausing=true;
        rideTimer->stop();
        Move_Timer->stop();
        });}
    else if(idleMovie->state()==QMovie::Paused||rideMovie->state()==QMovie::Running)
    {
        menu->addAction("继续",this,[=](){
        idleMovie->setPaused(false);
        rideMovie->setPaused(false);
        if(mover!=nullptr)
            mover->continue_move();
        isPausing=false;
        if(gifLabel->movie()==idleMovie)
            rideTimer->start(8000);
        else if(gifLabel->movie()==rideMovie)
            Move_Timer->start(6000);
        });}
    menu->addAction("设置",this,[=](){
        SettingWidget::getSettingWidget(this);});
    menu->move(QCursor::pos());
    menu->show();
}
// void Widget::loadPicture(QPixmap* oriPicture,
// QList<QPixmap> &frames,int r,int c,int width,int height,int num)
// {
//     int cnt=0;
//     frames.clear();
//     for(int i=0;i<r;i++)
//     {
//         for(int j=0;j<c;j++)
//         {
//             if(cnt==num)
//                 return;
//             int x=j*width;int y=i*height;
//             QPixmap frame = oriPicture->copy(x+50, y, width, height);
//             //frame.save(QString("%1.png").arg(cnt));
//             frames.append(frame);
//             cnt++;
//         }
//     }
// }
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    idlefile=":/image/catIdle.gif";
    smokefile=":/image/smoke.gif";
    ridefile=":/image/catRide.gif";
    walkfile=":/image/catwalk.gif";
    resize(150,150);//初始尺寸
    // move(1000,500);
    QVBoxLayout *layout = new QVBoxLayout(this);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    this->setWindowFlag(Qt::FramelessWindowHint);//设置窗口无边框
    setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明
    gifLabel = new MyGifLabel(this);
    idleMovie = new QMovie(this);
    idleMovie->setFileName(idlefile);
    smokeMovie = new QMovie(this);
    smokeMovie->setFileName(smokefile);
    rideMovie=new QMovie(this);
    rideMovie->setFileName(ridefile);
    walkMovie=new QMovie(this);
    walkMovie->setFileName(walkfile);
    menu=new QMenu(gifLabel);
    // oriPicture=new QPixmap(":/image/catMove.png");
    // oriPicture->scaled(400, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    // Widget::loadPicture(oriPicture,frames,8,14,400,97,15);//加载精灵图
    layout->addWidget(gifLabel);
    layout->setContentsMargins(0, 0, 0, 0);

    Move_Timer=new QTimer(this);
    smokeTimer=new QTimer(this);

    dragTimer=new QTimer(this);//区别单击和拖拽
    connect(dragTimer,&QTimer::timeout,this,[=](){isDragging=true;});

    Widget::catIdle();//初始待机
    rideTimer=new QTimer(this);//静置触发
    rideTimer->start(8000);//静置阈值
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
        {Widget::catWalk();isMoving=true;Move_Timer->start(1900);rideTimer->stop();}
        else
        {Widget::catIdle();isMoving=false;Move_Timer->stop();rideTimer->start(8000);};
    });
    // connect(walkTimer,&QTimer::timeout,this,&Widget::updateFrame);
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
    filePath = QFileDialog::getOpenFileName(
        this,
        "选择皮肤文件",
        "",
        "文件(*.qss *.png *.jpg *.gif)"
        );
    if (!filePath.isEmpty()) {
        ui->filetext1->setText(filePath);
    }
}
void SettingWidget::on_commitButton1_clicked()
{
    if (!filePath.isEmpty()) {
        target->idlefile=filePath;
        target->idleMovie->setFileName(target->idlefile);
        target->catIdle();
    }
}


void SettingWidget::on_fileButton2_clicked()
{
    filePath = QFileDialog::getOpenFileName(
        this,
        "选择皮肤文件",
        "",
        "文件(*.qss *.png *.jpg *.gif)"
        );
    if (!filePath.isEmpty()) {
        ui->filetext2->setText(filePath);
    }
}

void SettingWidget::on_commitButton2_clicked()
{
    if (!filePath.isEmpty()) {
        target->walkfile=filePath;
        target->walkMovie->setFileName(target->walkfile);
        target->catIdle();
    }
}


void SettingWidget::on_fileButton3_clicked()
{
    filePath = QFileDialog::getOpenFileName(
        this,
        "选择皮肤文件",
        "",
        "文件(*.qss *.png *.jpg *.gif)"
        );
    if (!filePath.isEmpty()) {
        ui->filetext3->setText(filePath);
    }
}


void SettingWidget::on_commitButton3_clicked()
{
    if (!filePath.isEmpty()) {
        target->ridefile=filePath;
        target->rideMovie->setFileName(target->ridefile);
        target->catIdle();
    }
}

