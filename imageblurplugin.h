#ifndef IMAGE_BLUR_PLUGIN_H
#define IMAGE_BLUR_PLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "PhotoEditorPluginInterface.h"

class ImageBlurPlugin: public QObject, public PhotoEditorPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PHOTO_EDITOR_PLUGIN_INTERFACE_IID)
    Q_INTERFACES(PhotoEditorPluginInterface)
public:
    QString name() override;
    void edit(const cv::Mat &input, cv::Mat &output) override;
};

#endif // IMAGE_BLUR_PLUGIN_H
