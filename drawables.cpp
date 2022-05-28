#include "drawables.h"

Movable::Movable(const QVector2D& pos) : m_position(pos)
{
}

void Movable::SetExpectedPosition(const QPointF& expPos) const
{
    if ((m_position - QVector2D(expPos)).length() < .1)
        return;

    emit Moved(m_startPos.toPoint(), expPos);
}

void Movable::SetPosition(const QVector2D& pos)
{
    m_position = pos;
}

void Movable::SetPosition(const QPointF& pos)
{
    SetPosition(QVector2D(pos));
}

QVector2D Movable::GetPosition() const
{
    return m_position;
}

void Movable::SetStartPos(const QVector2D& pos)
{
    m_startPos = pos;
}

void Movable::SetStartPos(const QPointF& pos)
{
    SetStartPos(QVector2D(pos));
}

QVector2D Movable::GetStartPos() const
{
    return m_startPos;
}

bool Movable::IsGrabbed() const
{
    return m_isGrabbed;
}

void Movable::GrabOn(QPointF const& grabbedPos)
{
    m_isGrabbed = m_isSelected = true;

    auto parent = m_parent.lock();
    if (parent != nullptr)
        parent->SetSelected(true);

    m_startPos = QVector2D(grabbedPos);
}

void Movable::Released()
{
    m_isGrabbed = false;
}

double Movable::GetZOrder()
{
    return m_zOrder;
}

void Movable::SetZOrder(double zOrder)
{
    m_zOrder = zOrder;
}

bool Movable::IsSelected()
{
    return m_isSelected;
}

void Movable::SetSelected(bool selected)
{
    m_isSelected = selected;
}

Movable::MovablePtr Movable::Parent()
{
    return m_parent;
}

void Movable::SetParent(MovablePtr parent)
{
    m_parent = parent;
}
//----------------------------------------------------------------
//----------------------------------------------------------------

NodeModel::NodeModel(QVector2D const& pos) : Movable{ pos }
{
}

void NodeModel::SetZOrder(double zOrder)
{
    Movable::SetZOrder(zOrder);
    for (auto node : m_nodes)
        node->SetZOrder(zOrder + .1);
}

void NodeModel::SetParentToNodes(std::shared_ptr<Movable> parent)
{
    for (auto node : m_nodes)
        node->SetParent(parent);
}

//----------------------------------------------------------------
//----------------------------------------------------------------

IntNode::IntNode(const std::shared_ptr<Node>& node) :
    NodeModel{ node->GetPosition() }, m_node(node)
{
    m_nodes.insert(m_node);
}

void IntNode::Free()
{
    connect(m_node.get(), &Node::Moved, this,
        [=](const QPointF& fromPos, const QPointF& toPos) // if it is grabbed as a Node
        {
            SetPosition(toPos - fromPos + GetPosition().toPointF());
            SetStartPos(toPos);
            m_node->SetPosition(GetPosition());
            m_node->SetStartPos(toPos);

            emit Changed();
        });

    connect(this, &NodeModel::Moved, this,
        [=](const QPointF& fromPos, const QPointF& toPos) // if it is grabbed as a NodeModel
        {
            SetPosition(toPos - fromPos + GetPosition().toPointF());
            SetStartPos(toPos);
            m_node->SetPosition(GetPosition());
            m_node->SetStartPos(toPos);

            emit Changed();
        });
}

bool IntNode::IsPointOn(const QPointF& pos) const
{
    return m_node->IsPointOn(pos);
}

//----------------------------------------------------------------
//----------------------------------------------------------------

IntNodeRep::IntNodeRep(const SingleNodePtr& singleNode) : m_singleNode(singleNode)
{
}

void IntNodeRep::Draw(QPainter* painter) const
{
    painter->drawEllipse(m_singleNode->m_node->GetPosition().toPointF(), 10, 10);
    //auto topLeft = m_singleNode->m_node->GetPosition() - QPointF(5, 5);
    //painter->drawRect(QRectF(topLeft, QSizeF(10., 10.)));
}

std::shared_ptr<NodeModel> IntNodeRep::GetModel() const
{
    return m_singleNode;
}

//----------------------------------------------------------------
//----------------------------------------------------------------

IntVector::IntVector(const NodePtr& nodeA, const NodePtr& nodeB) :
    m_nodeA(nodeA), m_nodeB(nodeB), NodeModel{ QVector2D{} }
{
    m_nodes.insert(nodeA);
    m_nodes.insert(nodeB);
    SetPosition((nodeA->GetPosition() + nodeB->GetPosition()) / 2);

    connect(this, &NodeModel::Moved, this,
        [=](const QPointF& fromPos, const QPointF& toPos)
        {
            SetPosition(toPos - fromPos + GetPosition().toPointF());
            SetStartPos(toPos);

            nodeA->SetPosition(toPos - fromPos + nodeA->GetPosition().toPointF());
            nodeA->SetStartPos(toPos);

            nodeB->SetPosition(toPos - fromPos + nodeB->GetPosition().toPointF());
            nodeB->SetStartPos(toPos);
            emit Changed();
        });
}

bool IntVector::IsPointOn(const QPointF& pos) const
{
    auto vec = (m_nodeA->GetPosition() - m_nodeB->GetPosition()).normalized();
    auto ap = QVector2D(pos) - m_nodeA->GetPosition();
    auto apProj = QVector2D::dotProduct(vec, ap) * vec;

    return (ap - apProj).length() < 3;
}

void IntVector::FixOnDirection()
{
    auto lineVec = QVector2D(m_nodeB->GetPosition() - m_nodeA->GetPosition()).normalized();

    auto connectMovements = [=](NodePtr baseNode, NodePtr movingNode)
    {
        connect(movingNode.get(), &Node::Moved, this,
            [=](const QPointF& fromPos, const QPointF& toPos)
            {
                auto newPointVec = QVector2D(toPos) - baseNode->GetPosition();
                auto proj = QVector2D::dotProduct(lineVec, newPointVec) * lineVec;
                movingNode->SetPosition(proj + baseNode->GetPosition());
                emit Changed();
            });
    };

    connectMovements(m_nodeA, m_nodeB);
    connectMovements(m_nodeB, m_nodeA);
}

void IntVector::ParallelToDirection()
{
    auto lineVec = QVector2D(m_nodeB->GetPosition() - m_nodeA->GetPosition()).normalized();
    auto connectMovements = [=](NodePtr baseNode, NodePtr movingNode)
    {
        connect(movingNode.get(), &Node::Moved, this,
            [=](const QPointF& fromPos, const QPointF& toPos)
            {
                auto newPointVec = QVector2D(toPos) - baseNode->GetPosition();
                auto projVec = QVector2D::dotProduct(lineVec, newPointVec) * lineVec;

                baseNode->SetPosition((newPointVec - projVec) + baseNode->GetPosition());
                movingNode->SetPosition(toPos);
                emit Changed();
            });
    };

    connectMovements(m_nodeA, m_nodeB);
    connectMovements(m_nodeB, m_nodeA);
}

void IntVector::FreeVector()
{
    connect(m_nodeA.get(), &Node::Moved, this,
        [=](const QPointF& fromPos, const QPointF& toPos)
        {
            m_nodeA->SetPosition(toPos);
            emit Changed();
        });

    connect(m_nodeB.get(), &Node::Moved, this,
        [=](const QPointF& fromPos, const QPointF& toPos)
        {
            m_nodeB->SetPosition(toPos);
            emit Changed();
        });
}

double IntVector::Length() const
{
    auto ab = m_nodeB->GetPosition() - m_nodeA->GetPosition();
    return QVector2D(ab).length();
}


//----------------------------------------------------------------
//----------------------------------------------------------------

VectorRep::VectorRep(const VecPtr& vector) : m_vector(vector)
{
    m_nodeARep = std::make_shared<IntNodeRep>(std::make_shared<IntNode>(vector->m_nodeA));
    m_nodeBRep = std::make_shared<IntNodeRep>(std::make_shared<IntNode>(vector->m_nodeB));
}

void VectorRep::Draw(QPainter* painter) const
{
    painter->drawLine(m_vector->m_nodeA->GetPosition().toPointF(),
        m_vector->m_nodeB->GetPosition().toPointF());

    if (!m_vector->IsSelected())
        return;

    m_nodeARep->Draw(painter);
    m_nodeBRep->Draw(painter);
}

std::shared_ptr<NodeModel> VectorRep::GetModel() const
{
    return m_vector;
}

//----------------------------------------------------------------
//----------------------------------------------------------------

IntPath::IntPath() : NodeModel{ QVector2D(0, 0) } //TODO
{
}

void IntPath::AddPoint(const QPointF& newPoint)
{
    AddNode(std::make_shared<Node>(newPoint));
}

void IntPath::AddNode(const NodePtr& newNode)
{
    m_nodes.insert(newNode);
    if (prevAddedNode == nullptr)
    {
        prevAddedNode = newNode;
        firstNode = newNode;
        return;
    }
    auto newVec = std::make_shared<IntVector>(prevAddedNode, newNode);
    m_vectors.append(newVec);
    prevAddedNode = newNode;
}

void IntPath::Close()
{
    m_vectors.append(std::make_shared<IntVector>(prevAddedNode, firstNode));
}

bool IntPath::IsPointOn(const QPointF& pos) const
{
    return false;
}

//----------------------------------------------------------------
//----------------------------------------------------------------

PathRep::PathRep(const PathPtr& path) : m_path(path)
{
    for (auto vec : path->m_vectors)
        m_vecReps.append(std::make_shared<VectorRep>(vec));
}

void PathRep::Draw(QPainter* painter) const
{
    for (auto vecRep : m_vecReps)
        vecRep->Draw(painter);
}

std::shared_ptr<NodeModel> PathRep::GetModel() const
{
    return m_path;
}

//----------------------------------------------------------------
//----------------------------------------------------------------

IntRect::IntRect(QRectF initialRect) : NodeModel{ QVector2D{} }
{
    m_nodeA = std::make_shared<Node>(initialRect.topLeft());
    m_nodeB = std::make_shared<Node>(initialRect.topRight());
    m_nodeC = std::make_shared<Node>(initialRect.bottomRight());
    m_nodeD = std::make_shared<Node>(initialRect.bottomLeft());

    m_diaVecA = QVector2D(initialRect.topLeft() - initialRect.center());
    m_diaVecB = QVector2D(initialRect.topRight() - initialRect.center());

    SetPosition(initialRect.center());

    m_nodeR = std::make_shared<Node>(
        (GetPosition() + m_diaVecA + 20.0 * m_diaVecA.normalized()).toPointF());

    m_nodeM = std::make_shared<Node>(GetPosition());

    m_nodes.insert(m_nodeA);
    m_nodes.insert(m_nodeB);
    m_nodes.insert(m_nodeC);
    m_nodes.insert(m_nodeD);
    m_nodes.insert(m_nodeR);
    m_nodes.insert(m_nodeM);

    connect(this, &NodeModel::Moved, this,
        [=](const QPointF& fromPos, const QPointF& toPos)
        {
            SetPosition(toPos - fromPos + GetPosition().toPointF());
            SetStartPos(toPos);
            UpdateNodes();
        });

    connect(m_nodeA.get(), &Node::Moved, this,
        [=](const QPointF& fromPos, const QPointF& toPos)
        {
            if ((GetPosition() - QVector2D(toPos)).length() < 5.0)
                return;

            SetPosition((QVector2D(toPos) + m_nodeC->GetPosition()) / 2.0);
            auto ab = (m_diaVecB - m_diaVecA);

            auto midVecN = (ab.length() > 1.0) ?
                QVector2D(ab.y(), -ab.x()).normalized()
                : ((m_diaVecB + m_diaVecA) / 2.).normalized();

            m_diaVecA = QVector2D(toPos) - GetPosition();
            auto midVec = QVector2D::dotProduct(midVecN, m_diaVecA) * midVecN;
            m_diaVecB = (2.0 * midVec) - m_diaVecA;

            UpdateNodes();
        });


    connect(m_nodeB.get(), &Node::Moved, this,
        [=](const QPointF& fromPos, const QPointF& toPos)
        {
            if ((GetPosition() - QVector2D(toPos)).length() < 5.0)
                return;

            SetPosition((QVector2D(toPos) + m_nodeD->GetPosition()) / 2.0);
            auto ab = (m_diaVecB - m_diaVecA);

            auto midVecN = (ab.length() > 1.0) ?
                QVector2D(ab.y(), -ab.x()).normalized()
                : ((m_diaVecB + m_diaVecA) / 2.).normalized();

            m_diaVecB = QVector2D(toPos) - GetPosition();
            auto midVec = QVector2D::dotProduct(midVecN, m_diaVecB) * midVecN;
            m_diaVecA = (2.0 * midVec) - m_diaVecB;

            UpdateNodes();
        });

    //TODO: Refactor and optimize move handlers
    connect(m_nodeC.get(), &Node::Moved, this,
        [=](const QPointF& fromPos, const QPointF& toPos)
        {
            if ((GetPosition() - QVector2D(toPos)).length() < 5.0)
                return;

            SetPosition((QVector2D(toPos) + m_nodeA->GetPosition()) / 2.0);
            auto ab = (m_diaVecB - m_diaVecA);

            auto midVecN = (ab.length() > 1.0) ?
                QVector2D(ab.y(), -ab.x()).normalized()
                : ((m_diaVecB + m_diaVecA) / 2.).normalized();

            m_diaVecA = -(QVector2D(toPos) - GetPosition());
            auto midVec = QVector2D::dotProduct(midVecN, m_diaVecA) * midVecN;
            m_diaVecB = (2.0 * midVec) - m_diaVecA;

            UpdateNodes();
        });

    connect(m_nodeD.get(), &Node::Moved, this,
        [=](const QPointF& fromPos, const QPointF& toPos)
        {
            if ((GetPosition() - QVector2D(toPos)).length() < 5.0)
                return;

            SetPosition((QVector2D(toPos) + m_nodeB->GetPosition()) / 2.0);
            auto ab = (m_diaVecB - m_diaVecA);

            auto midVecN = (ab.length() > 1.0) ?
                QVector2D(ab.y(), -ab.x()).normalized()
                : ((m_diaVecB + m_diaVecA) / 2.).normalized();

            m_diaVecB = -(QVector2D(toPos) - GetPosition());
            auto midVec = QVector2D::dotProduct(midVecN, m_diaVecB) * midVecN;
            m_diaVecA = (2.0 * midVec) - m_diaVecB;

            UpdateNodes();
        });

    connect(m_nodeR.get(), &Node::Moved, this,
        [=](const QPointF& fromPos, const QPointF& toPos)
        {

            auto devD = (QVector2D(toPos) - GetPosition()).normalized();
            auto angle = asin(QVector3D::crossProduct(QVector3D(devD),
                QVector3D(m_diaVecA.normalized())).z()) * 180.0 / M_PI;

            RotateBy(-angle);
        });

    connect(m_nodeM.get(), &Node::Moved, this,
        [=](const QPointF& fromPos, const QPointF& toPos)
        {
            SetPosition(toPos);
            UpdateNodes();
        });
}

bool IntRect::IsPointOn(const QPointF& pos) const
{
    auto pVec = QVector2D(pos) - GetPosition();
    auto midY = ((m_diaVecA + m_diaVecB) / 2.).normalized();
    auto midX = ((m_diaVecB - m_diaVecA) / 2.).normalized();

    return abs(QVector2D::dotProduct(pVec, midY)) < Height() / 2.f &&
        abs(QVector2D::dotProduct(pVec, midX)) < Width() / 2.f;
}

float IntRect::AngleZ() const
{
    auto midXN = ((m_diaVecB - m_diaVecA) / 2.).normalized();
    auto angle = atan2(midXN.y(), midXN.x()) * 180. / M_PI;
    return angle;
}

float IntRect::Height() const
{
    return QVector2D(m_nodeA->GetPosition() - m_nodeD->GetPosition()).length();
}

float IntRect::Width() const
{
    return QVector2D(m_nodeA->GetPosition() - m_nodeB->GetPosition()).length();
}

void IntRect::UpdateNodes()
{
    auto centre = GetPosition();
    m_nodeA->SetPosition(centre + m_diaVecA);
    m_nodeC->SetPosition(centre - m_diaVecA);
    m_nodeB->SetPosition(centre + m_diaVecB);
    m_nodeD->SetPosition(centre - m_diaVecB);
    m_nodeR->SetPosition(centre + m_diaVecA + 20.0 * m_diaVecA.normalized());
    m_nodeM->SetPosition(centre);
    emit Changed();
}

void IntRect::RotateBy(double angle)
{
    QTransform trans;
    trans.rotate(angle);
    m_diaVecA = QVector2D(trans.map(m_diaVecA.toPointF()));
    m_diaVecB = QVector2D(trans.map(m_diaVecB.toPointF()));
    UpdateNodes();
}

//----------------------------------------------------------------
//----------------------------------------------------------------

RectRep::RectRep(RectPtr const& rect) : m_rect(rect)
{
    m_nodeRRep = std::make_shared<IntNodeRep>(std::make_shared<IntNode>(rect->m_nodeR));
    m_nodeMRep = std::make_shared<IntNodeRep>(std::make_shared<IntNode>(rect->m_nodeM));
    m_nodeARep = std::make_shared<IntNodeRep>(std::make_shared<IntNode>(rect->m_nodeA));
    m_nodeBRep = std::make_shared<IntNodeRep>(std::make_shared<IntNode>(rect->m_nodeB));
    m_nodeCRep = std::make_shared<IntNodeRep>(std::make_shared<IntNode>(rect->m_nodeC));
    m_nodeDRep = std::make_shared<IntNodeRep>(std::make_shared<IntNode>(rect->m_nodeD));
}

void RectRep::Draw(QPainter* painter) const
{
    painter->save();
    painter->translate(m_rect->GetPosition().toPointF());
    painter->rotate(m_rect->AngleZ());
    painter->drawRect(
        QRectF(-QPointF(m_rect->Width() / 2., m_rect->Height() / 2.),
        QPointF(m_rect->Width() / 2., m_rect->Height() / 2.)));
    painter->restore();

    if (!m_rect->IsSelected())
        return;

    painter->save();
    painter->setBrush(Qt::BrushStyle::NoBrush);
    m_nodeRRep->Draw(painter);
    m_nodeMRep->Draw(painter);
    m_nodeARep->Draw(painter);
    m_nodeBRep->Draw(painter);
    m_nodeCRep->Draw(painter);
    m_nodeDRep->Draw(painter);
    painter->restore();
}

std::shared_ptr<NodeModel> RectRep::GetModel() const
{
    return m_rect;
}

//----------------------------------------------------------------
//----------------------------------------------------------------


EllipseRep::EllipseRep(RectPtr const& rect) :
    m_rect(rect), m_rectRep(std::make_shared<RectRep>(rect))
{
}

void EllipseRep::Draw(QPainter* painter) const
{
    painter->save();
    painter->translate(m_rect->GetPosition().toPointF());
    painter->rotate(m_rect->AngleZ());
    painter->drawEllipse(QPointF(0., 0.), m_rect->Width() / 2., m_rect->Height() / 2.);
    painter->restore();

    if (!m_rect->IsSelected())
        return;

    painter->save();
    painter->setBrush(Qt::BrushStyle::NoBrush);
    m_rectRep->Draw(painter);
    painter->restore();
}

std::shared_ptr<NodeModel> EllipseRep::GetModel() const
{
    return m_rect;
}

//----------------------------------------------------------------
//----------------------------------------------------------------

TextRep::TextRep(QString text, TextRep::RectPtr rect): 
    m_text(text),
    m_rect(rect),
    m_rectRep(std::make_shared<RectRep>(rect))
{
}

void TextRep::Draw(QPainter* painter) const
{
    painter->save();
    painter->translate(m_rect->m_nodeA->GetPosition().toPointF());
    painter->rotate(m_rect->AngleZ());
    painter->drawText(QRect(0,0, m_rect->Width(), m_rect->Height()), m_text);
    painter->restore();

    if (!m_rect->IsSelected())
        return;

    painter->save();
    painter->setBrush(Qt::BrushStyle::NoBrush);
    m_rectRep->Draw(painter);
    painter->restore();
}

std::shared_ptr<NodeModel> TextRep::GetModel() const
{
    return m_rect;
}

QString TextRep::GetText() const
{
    return m_text;
}

void TextRep::SetText(QString const& text)
{
    m_text = text;
}