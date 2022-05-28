#include "DrawableActor.h"

DrawableActor::DrawableActor(
    MovableActorPtr const& movableActor,
    std::function<void()> updateHandler):
    m_movableActor(movableActor),
    m_updateHandler(updateHandler)
{
}

auto DrawableActor::getSelected()
{
    return std::find_if(m_drawables.cbegin(), m_drawables.cend(),
        [](const NodeModelRepPtr& drw) {
            return drw->GetModel()->IsSelected();
        });
}

DrawableActor::NodeModelRepPtr DrawableActor::GetSelected()
{
    return *getSelected();
}

void DrawableActor::DeletSelected()
{
    auto selectedDrw = getSelected();

    if (selectedDrw != m_drawables.cend())
    {
        auto selectedModel = selectedDrw->get()->GetModel();
        m_movableActor->RemoveNodeModel(selectedModel);
        m_drawables.erase(selectedDrw);
        refresh();
        m_updateHandler();
    }
}

void DrawableActor::BringSelectedToFront()
{
    auto selectedDrw = getSelected();
    if (selectedDrw >= m_drawables.cend() - 1)
        return;

    auto topDrw = m_drawables.cend() - 1;
    auto maxZ = topDrw->get()->GetModel()->GetZOrder();
    selectedDrw->get()->GetModel()->SetZOrder(maxZ + 1.0);
    refresh();
}

void DrawableActor::SendSelectedToBack()
{
    auto selectedDrw = getSelected();
    auto backDrw = m_drawables.cbegin();

    if (selectedDrw == backDrw)
        return;

    auto minZ = backDrw->get()->GetModel()->GetZOrder();
    selectedDrw->get()->GetModel()->SetZOrder(minZ - 1.0);
    refresh();
}

void DrawableActor::UnSelectAll()
{
    for (auto drawable : m_drawables)
    {
        drawable->GetModel()->SetSelected(false);
    }
    m_updateHandler();
}

void DrawableActor::SelectOn(QPointF const& pos)
{
    for (auto drawable : m_drawables)
    {
        if (!drawable->GetModel()->IsPointOn(pos))
            continue;

        drawable->GetModel()->SetSelected(true);
        m_updateHandler();
        break;
    }
}

// the text could/should have a parent
// relative position to the parent

// movables group // get next shape ? // add

bool DrawableActor::AnySelected()
{
    return getSelected() != m_drawables.cend();
}

void DrawableActor::Add(DrawableActor::NodeModelRepPtr const& drawable)
{
    QObject::connect(drawable->GetModel().get(), &NodeModel::Changed, m_updateHandler);
    drawable->GetModel()->SetParentToNodes(drawable->GetModel());
    auto topDrw = std::max_element(m_drawables.cbegin(), m_drawables.cend(),
        [](NodeModelRepPtr const& a, NodeModelRepPtr const& b)
        {
            return a->GetModel()->GetZOrder() < b->GetModel()->GetZOrder();
        });
    auto maxZ = (topDrw != m_drawables.cend()) ? topDrw->get()->GetModel()->GetZOrder() : 0.0;
    drawable->GetModel()->SetZOrder(maxZ + 1.0);

    m_drawables.push_back(drawable);
    m_movableActor->Add(drawable->GetModel());
    refresh();
    m_updateHandler();
}

void DrawableActor::DrawAll(QPainter* painter)
{
    for (auto drawable : m_drawables)
        drawable->Draw(painter);
}

void DrawableActor::Clear()
{
}

void DrawableActor::refresh()
{
    std::sort(m_drawables.begin(), m_drawables.end()); // sort to earase duplicates
    m_drawables.erase(
        std::unique(m_drawables.begin(), m_drawables.end()), m_drawables.end());

    std::sort(m_drawables.begin(), m_drawables.end(), 
        [](const NodeModelRepPtr& a, const NodeModelRepPtr& b)
        {
            // draw in ascending order
            return a->GetModel()->GetZOrder() < b->GetModel()->GetZOrder();
        });

    m_movableActor->Refresh();
    // refresh z-order values
    std::for_each(m_drawables.begin(), m_drawables.end(),
        [n = 0.0](NodeModelRepPtr& drawable) mutable
    {
        drawable->GetModel()->SetZOrder(n++);
    });
}
