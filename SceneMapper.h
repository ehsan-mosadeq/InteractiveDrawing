#pragma once

#include <QTransform>

class SceneMapper
{
    public:
    void SetTransform(QTransform const& trans)
    {
        m_mapFromScene = trans;
    }

    QPoint MapToScene(QPointF const& point) const
    {
        return m_mapFromScene.inverted().map(point).toPoint();
    }

    QPointF MapFromScene(QPointF const& point) const
    {
        return m_mapFromScene.map(point);
    }

    private:
    QTransform m_mapFromScene;
};
