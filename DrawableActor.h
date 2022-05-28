#pragma once
#include <vector>

#include "drawables.h"
#include "MovableActor.h"

class DrawableActor
{
    public:
    using NodeModelRepPtr = std::shared_ptr<NodeModelRep>;
    using MovableActorPtr = std::shared_ptr<MovableActor>;
    enum class Shape {
        None, Line, Points, Polyline, Polygon, Rect, RoundedRect, Ellipse, Arc,
        Chord, Pie, Path, Text, Pixmap, Node
    };

    DrawableActor(MovableActorPtr const& movableActor, std::function<void()> updateHandler);
    void DeletSelected();
    void BringSelectedToFront();
    void SendSelectedToBack();
    bool AnySelected();
    void Add(NodeModelRepPtr const& drawable);
    void DrawAll(QPainter* painter);
    void UnSelectAll();
    void SelectOn(QPointF const& pos);
    void Clear();
    NodeModelRepPtr GetSelected();

    private:
    auto getSelected();
    void refresh();

    std::vector<NodeModelRepPtr> m_drawables;
    MovableActorPtr m_movableActor;
    std::function<void()> m_updateHandler;
};