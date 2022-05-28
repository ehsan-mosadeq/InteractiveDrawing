#pragma once

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QSet>

#include "drawables.h"
#include "drawablesscene.h"

class RenderArea : public QWidget
{
    Q_OBJECT

    public:
    using Shape = DrawablesScene::Shape;
    explicit RenderArea(QWidget* parent = nullptr);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    public slots:
    void setAction(Shape shape);
    void setPen(const QPen& pen);
    void setBrush(const QBrush& brush);

    protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    private:
    Shape m_currentShape;
    QPen pen;
    QBrush brush;
    QPixmap pixmap;
    DrawablesScene* m_drawablesScene;
};

