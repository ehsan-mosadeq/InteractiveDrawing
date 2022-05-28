#include "MovableActor.h"

MovableActor::MovableActor()
{
}

void MovableActor::Add(std::shared_ptr<NodeModel> nodeModel)
{
    m_movables.push_back(nodeModel);
    for (auto node : nodeModel->m_nodes)
        m_movables.push_back(node);
}

void MovableActor::SetExpectedToGrabbed(const QPointF& expectedPos)
{
    for (auto movable : m_movables)
    {
        if (!movable->IsGrabbed())
            continue;

        movable->SetExpectedPosition(expectedPos);
        break;
    }
}

void MovableActor::ReleaseAll()
{
    for (auto movable : m_movables)
        movable->Released();
}

void MovableActor::GrabOn(QPointF const& pos)
{
    for (auto movable : m_movables)
    {
        if (!movable->IsPointOn(pos))
            continue;

        movable->GrabOn(pos);
        break;
    }
}

void MovableActor::Refresh()
{
    std::sort(m_movables.begin(), m_movables.end());
    m_movables.erase(std::unique(m_movables.begin(), m_movables.end()), m_movables.end());

    std::sort(m_movables.begin(), m_movables.end(), [](const MovablePtr& a, const MovablePtr& b)
        {
            return a->GetZOrder() > b->GetZOrder(); //grab in descending order
        });
}

void MovableActor::RemoveNodeModel(std::shared_ptr<NodeModel> nodeModel)
{
    auto nodes = nodeModel->m_nodes.values();
    m_movables.erase(
        std::remove_if(m_movables.begin(), m_movables.end(),
        [&nodes, &nodeModel](MovablePtr& const movable) {
            return (movable == nodeModel) || std::find_if(nodes.begin(), nodes.end(),
                [&movable](std::shared_ptr<Node>& const node) {
                    return movable == node;
                }) != nodes.end(); }), m_movables.end());
}
