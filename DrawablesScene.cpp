#include <QApplication>
#include <QMenu>
#include <QResizeEvent>

#include "drawablesscene.h"
#include "DrawablesContextMenu.h"
#include "DrawablesInit.h"


DrawablesScene::DrawablesScene(QWidget* parent): m_parent(parent), Movable(QVector2D(0, 0))
{
    connect(this, &Movable::Moved, this, [=](const QPointF& fromPos, const QPointF& toPos)
        {
            if (m_sceneAction == SceneAction::Pan)
            {
                SetPosition(GetPosition() + (QVector2D(toPos - fromPos) / m_scale));
                SetStartPos(QVector2D(toPos));
            }
            if (m_sceneAction == SceneAction::Zoom)
            {
                m_scale *= (QVector2D(toPos - fromPos).y() < 0)? 1.05 : (1./1.05);
                SetStartPos(QVector2D(toPos));
            }
            if (m_sceneAction == SceneAction::Rotate)
            {
                //m_angle += (QVector2D(toPos - fromPos).y() < 0) ? 3. : -3;
                //m_startPos = QVector2D(toPos);
            }
            emit Updated();
        });

    auto floatText = new QTextEdit(m_parent);
    floatText->hide();
    m_textActor = std::make_shared<TextActor>(floatText, m_sceneMapper);
    connect(m_textActor.get(), &TextActor::TextCreated,
        this, [=](NodeModelRepPtr nmRep) {m_drawableActor->Add(nmRep); });
}

DrawablesScene::NodeModelRepPtr DrawablesScene::CreateShape(Shape shape, QPointF const& startPos)
{
    switch (shape)
    {
    case Shape::Rect:
        return DrawablesInit::InitRect(startPos);
        break;
    case Shape::Ellipse:
        return DrawablesInit::InitEllipse(startPos);
        break;
    case Shape::Line:
        return DrawablesInit::InitLine(startPos);
        break;
    case Shape::Node:
        return DrawablesInit::InitNode(startPos);
        break;

    default:
        return nullptr;
    }
}


bool DrawablesScene::IsPointOn(const QPointF& pos) const
{
    return true;
}

void DrawablesScene::MouseMoveHandler(QMouseEvent* ev)
{
    auto btn = ev->buttons();
    auto pos = ev->pos();

    if (btn == Qt::LeftButton)
    {
        this->SetExpectedPosition(pos);
        return;
    }

    // The inverse transformation of the scene should be applied on the mouse position
    if (btn != Qt::RightButton)
        return;

    auto mappedPos = m_sceneMapper->MapToScene(pos);
    m_movableActor->SetExpectedToGrabbed(mappedPos);
}

void DrawablesScene::MousePressedHandler(QMouseEvent* ev)
{
    auto btn = ev->buttons();
    auto pos = ev->pos();
    auto mod = ev->modifiers();

    this->GrabOn(pos);
    if (btn == Qt::LeftButton)
        m_sceneAction = SceneAction::Pan;

    if (btn == Qt::LeftButton && mod == Qt::KeyboardModifier::ShiftModifier)
        m_sceneAction = SceneAction::Rotate;

    if (btn == Qt::LeftButton && mod == Qt::KeyboardModifier::ControlModifier)
        m_sceneAction = SceneAction::Zoom;

    // The inverse transformation of the scene should be applied on the mouse position

    if (btn != Qt::RightButton)
        return;

    m_drawableActor->UnSelectAll();
    auto mappedPos = m_sceneMapper->MapToScene(pos);

    if (m_currentShape == Shape::None)
    {
        m_movableActor->GrabOn(mappedPos);

        if (mod == Qt::KeyboardModifier::ControlModifier)
        {
            DrawablesContextMenu::Show(pos, m_drawableActor, m_textActor, m_parent);
        }

        return;
    }

    if (m_currentShape == Shape::Text)
    {
        m_textActor->ShowTextEdit(pos);
    }

    auto drawable = CreateShape(m_currentShape, mappedPos);
    if (drawable != nullptr)
    {
        m_drawableActor->Add(drawable);
    }

    m_currentShape = Shape::None; // TODO: keep drawing or not..
}

void DrawablesScene::MouseReleasedHandler(QMouseEvent* ev)
{
    m_movableActor->ReleaseAll();
    Released();
    m_sceneAction = SceneAction::None;
}

void DrawablesScene::ResizeHandler(QResizeEvent* event)
{
    auto newSize = event->size();
    m_frameCentre = QPointF(newSize.width(), newSize.height()) / 2.;
}

void DrawablesScene::SetCurrentShape(Shape action)
{
    m_currentShape = action;
}

void DrawablesScene::Draw(QPainter* painter)
{
    painter->save();
    painter->translate(m_frameCentre);
    painter->scale(m_scale, m_scale);
    painter->translate(GetPosition().toPointF());
    m_sceneMapper->SetTransform(painter->transform());
    m_drawableActor->DrawAll(painter);
    painter->restore();
}

void DrawablesScene::KeyPressedHandler(QKeyEvent* ev)
{
    if (ev->key() == Qt::Key_Delete)
    {
        m_textActor->DeleteSelected();
        m_drawableActor->DeletSelected();
    }
}
