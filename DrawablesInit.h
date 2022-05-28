#pragma once

#include <memory>

#include "drawables.h"

static class DrawablesInit
{
    public:
    inline static std::shared_ptr<RectRep> InitRect(QPointF const& pos)
    {
        auto rect = std::make_shared<IntRect>(QRect(pos.x(), pos.y(), 1.0, 1.0));
        auto rectRep = std::make_shared<RectRep>(rect);
        rect->m_nodeC->GrabOn(pos);
        return rectRep;
    }

    inline static std::shared_ptr<EllipseRep> InitEllipse(QPointF const& pos)
    {
        auto rect = std::make_shared<IntRect>(QRect(pos.x(), pos.y(), 1.0, 1.0));
        auto rectRep = std::make_shared<EllipseRep>(rect);
        rect->m_nodeC->GrabOn(pos);
        return rectRep;
    }

    inline static std::shared_ptr<VectorRep> InitLine(QPointF const& pos)
    {
        auto line = std::make_shared<IntVector>(
            std::make_shared<Node>(pos), std::make_shared<Node>(pos));

        line->FreeVector();
        auto lineRep = std::make_shared<VectorRep>(line);
        line->m_nodeB->GrabOn(pos);
        return lineRep;
    }

    inline static std::shared_ptr<IntNodeRep> InitNode(QPointF const& pos)
    {
        auto node = std::make_shared<IntNode>(std::make_shared<Node>(pos));
        node->SetParentToNodes(node);
        node->Free();
        auto nodeRep = std::make_shared<IntNodeRep>(node);
        return nodeRep;
    }

    inline static std::shared_ptr<TextRep> InitText(QPointF const& pos, QString const& txt)
    {
        auto text = std::make_shared<TextRep>(txt,
            std::make_shared<IntRect>(QRectF(pos, QSizeF(160, 80))));
        return text;
    }
};
