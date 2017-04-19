#ifndef GENERATEPANEL_H
#define GENERATEPANEL_H

#include <QWidget>
#include <memory>

#include <worldapi/mesh.h>

#include "scene.h"
#include "resource.h"

class GeneratePanel : public QWidget
{
    Q_OBJECT
public:
    explicit GeneratePanel(QWidget *parent = 0);

    virtual const Scene * getMeshes();

    virtual std::vector<std::unique_ptr<Resource>> getResources();
signals:
    void meshesChanged(const Scene * objects);
    void imageChanged(const QImage * image);

public slots:
    virtual void generate() = 0;

protected:
    std::unique_ptr<Scene> _myScene;
};

#endif // GENERATEPANEL_H
