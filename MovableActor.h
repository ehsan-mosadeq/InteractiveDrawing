#pragma once

#include "drawables.h"

class MovableActor
{
    public:
    using MovablePtr = std::shared_ptr<Movable>;
    MovableActor();
    void Add(std::shared_ptr<NodeModel> nodeModel);
    void SetExpectedToGrabbed(const QPointF& expectedPos);
    void ReleaseAll();
    void GrabOn(QPointF const& pos);
    void Refresh();
    void RemoveNodeModel(std::shared_ptr<NodeModel> nodeModel);

    private:
    std::vector<MovablePtr> m_movables;
};