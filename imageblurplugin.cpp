#include "imageblurplugin.h"

QString ImageBlurPlugin::name() {
    return "Blur";
}

static QPixmap MatToQPixmap(const cv::Mat &mat) {
    QImage img;
    if (mat.channels() == 3) {
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
        img = QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
    } else if (mat.channels() == 4) {
        cv::Mat rgba;
        cv::cvtColor(mat, rgba, cv::COLOR_BGRA2RGBA);
        img = QImage(rgba.data, rgba.cols, rgba.rows, rgba.step, QImage::Format_RGBA8888).copy();
    }
    return QPixmap::fromImage(img);
}

void ImageBlurPlugin::edit(const cv::Mat &input, cv::Mat &output, QWidget *parent) {
    cv::Mat original = input.clone();
    cv::Mat current = original.clone();

    QDialog dialog(parent);
    dialog.setWindowTitle("Adjust Blur");
    dialog.setMinimumSize(400, 600);
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    QGraphicsView *previewView = new QGraphicsView(&dialog);
    QGraphicsScene *scene = new QGraphicsScene(previewView);
    previewView->setScene(scene);
    QPixmap pixmap = MatToQPixmap(original);
    QGraphicsPixmapItem *item = scene->addPixmap(pixmap);
    scene->setSceneRect(pixmap.rect());
    mainLayout->addWidget(previewView);

    QTimer::singleShot(0, &dialog, [&]() {
        previewView->fitInView(item, Qt::KeepAspectRatio);
    });

    QGroupBox *blurTypeGroup = new QGroupBox("Blur Type");
    QVBoxLayout *radioLayout = new QVBoxLayout(blurTypeGroup);
    QRadioButton *boxBlur = new QRadioButton("Box Blur");
    QRadioButton *gaussianBlur = new QRadioButton("Gaussian Blur");
    boxBlur->setChecked(true);
    radioLayout->addWidget(boxBlur);
    radioLayout->addWidget(gaussianBlur);
    mainLayout->addWidget(blurTypeGroup);

    QWidget *row = new QWidget;
    QHBoxLayout *sliderLayout = new QHBoxLayout(row);
    QLabel *label = new QLabel("Kernel Size: 9");
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(1, 49);
    slider->setValue(9);
    sliderLayout->addWidget(label);
    sliderLayout->addWidget(slider);
    mainLayout->addWidget(row);

    auto update = [&]() {
        int ksize = slider->value();
        if (ksize % 2 == 0) ++ksize;
        label->setText(QString("Kernel Size: %1").arg(ksize));

        cv::Mat blurred;
        if (boxBlur->isChecked()) {
            cv::blur(original, blurred, cv::Size(ksize, ksize));
        } else {
            cv::GaussianBlur(original, blurred, cv::Size(ksize, ksize), 0);
        }

        current = blurred;
        item->setPixmap(MatToQPixmap(current));
    };

    QObject::connect(slider, &QSlider::valueChanged, &dialog, update);
    QObject::connect(boxBlur, &QRadioButton::toggled, &dialog, update);
    QObject::connect(gaussianBlur, &QRadioButton::toggled, &dialog, update);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    QPushButton *ok = new QPushButton("OK");
    QPushButton *cancel = new QPushButton("Cancel");
    btnLayout->addWidget(ok);
    btnLayout->addWidget(cancel);
    mainLayout->addLayout(btnLayout);

    QObject::connect(ok, &QPushButton::clicked, &dialog, [&]() {
        output = current.clone();
        dialog.accept();
    });
    QObject::connect(cancel, &QPushButton::clicked, &dialog, [&]() {
        dialog.reject();
    });

    update();
    dialog.exec();
}
