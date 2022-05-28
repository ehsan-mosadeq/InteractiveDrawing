#include "renderarea.h"
#include "drawables.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>


RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    m_currentShape = Shape::Polygon;
    pixmap.load(":/images/qt-logo.png");
    m_drawablesScene = new DrawablesScene(this);
    connect(m_drawablesScene, &DrawablesScene::Updated, this, [=]() {update(); });
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize RenderArea::sizeHint() const
{
    return QSize(400, 200);
}

void RenderArea::setAction(Shape action)
{
    this->m_currentShape = action;
    m_drawablesScene->SetCurrentShape(action);
    update();
}

void RenderArea::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void RenderArea::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}


void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    m_drawablesScene->Draw(&painter);
}
void RenderArea::mouseMoveEvent(QMouseEvent* ev)
{
    m_drawablesScene->MouseMoveHandler(ev);
}
void RenderArea::mousePressEvent(QMouseEvent* ev)
{
    m_drawablesScene->MousePressedHandler(ev);
}

void RenderArea::mouseReleaseEvent(QMouseEvent* ev) //TODO: connect to Grrabbed slot
{
    m_drawablesScene->MouseReleasedHandler(ev);
}
void RenderArea::keyPressEvent(QKeyEvent* event)
{
    m_drawablesScene->KeyPressedHandler(event);
}
void RenderArea::resizeEvent(QResizeEvent* event)
{
    m_drawablesScene->ResizeHandler(event);
}

