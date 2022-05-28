#pragma once

#include <algorithm>
#include <vector>

#include <QMouseEvent>
#include <QKeyEvent>
#include <QWidget>
#include <QTextEdit>

#include "drawables.h"
#include "DrawableActor.h"
#include "DrawablesInit.h"
#include "SceneMapper.h"
#include "TextActor.h"

class DrawablesScene : public Movable
{
    Q_OBJECT

    public:
    using NodeModelRepPtr = std::shared_ptr<NodeModelRep>;
    using MovablePtr = std::shared_ptr<Movable>;
    using Shape = DrawableActor::Shape;
    using SceneMapperPtr = std::shared_ptr<SceneMapper>;
    using TextAgenPtr = std::shared_ptr<TextActor>;

    enum class SceneAction {
        None, Pan, Zoom, Rotate
    };

    DrawablesScene(QWidget* parent);
    NodeModelRepPtr CreateShape(Shape shape, QPointF const& startPos);
    void MouseMoveHandler(QMouseEvent* ev);
    void MousePressedHandler(QMouseEvent* ev);
    void MouseReleasedHandler(QMouseEvent* ev);
    void ResizeHandler(QResizeEvent* event);
    void KeyPressedHandler(QKeyEvent* ev);
    void SetCurrentShape(Shape action);
    void Draw(QPainter* painter);
    bool IsPointOn(const QPointF& pos) const override;

    signals: 
    void Updated();

    private:
    std::shared_ptr<MovableActor> m_movableActor = std::make_shared<MovableActor>();
    std::shared_ptr<DrawableActor> m_drawableActor = 
        std::make_shared<DrawableActor>(m_movableActor, [=]() { emit Updated(); });


    QWidget* m_parent = nullptr;
    Shape m_currentShape = Shape::None;
    SceneAction m_sceneAction = SceneAction::None;
    double m_scale = 1.0;
    QPointF m_frameCentre;
    SceneMapperPtr m_sceneMapper = std::make_shared<SceneMapper>();
    TextAgenPtr m_textActor;
};