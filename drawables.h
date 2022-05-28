#pragma once

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QVector2D>
#include <QVector3D>
#include <QDebug>
#include <QPainter>
#include <QGenericMatrix>
#include <QTransform>
#include <QMatrix4x4>

class Movable : public QObject
{
    Q_OBJECT
    public:

    using MovablePtr = std::weak_ptr<Movable>;
    Movable(const QVector2D& pos);
    virtual void SetExpectedPosition(const QPointF& pos) const;
    virtual void SetPosition(const QVector2D& pos);
    virtual void SetPosition(const QPointF& pos);
    virtual QVector2D GetPosition() const;
    virtual void SetStartPos(const QVector2D& pos);
    virtual void SetStartPos(const QPointF& pos);
    virtual QVector2D GetStartPos() const;
    virtual bool IsGrabbed() const;
    virtual void GrabOn(QPointF const& grabbedPos);
    virtual void Released();
    virtual double GetZOrder();
    virtual void SetZOrder(double zOrder);
    virtual bool IsSelected();
    virtual void SetSelected(bool selected);
    virtual MovablePtr Parent();
    virtual void SetParent(MovablePtr parent);
    virtual bool IsPointOn(const QPointF& pos) const = 0;

    signals:
    void Moved(const QPointF fromPos, const QPointF toPos) const;

    private:
    QVector2D m_position;
    QVector2D m_startPos;
    double m_zOrder = 0.0;
    bool m_isGrabbed = false;
    bool m_isSelected = true;
    MovablePtr m_parent;
};

class Node : public Movable
{
    Q_OBJECT
    public:

    Node(const QVector2D& pos) : Movable{pos}
    {
    }

    Node(const QPointF& pos) : Movable{ QVector2D(pos) }
    {
    }

    virtual bool IsPointOn(const QPointF& pos) const
    {
        return (GetPosition() - QVector2D(pos)).length() < 10;
    }
};

class NodeModel : public Movable
{
    Q_OBJECT
    public:
    NodeModel(const QVector2D& pos);
    virtual void SetZOrder(double zOrder);
    virtual void SetParentToNodes(std::shared_ptr<Movable> parent);
    virtual ~NodeModel() = default;
    QSet<std::shared_ptr<Node>> m_nodes;

    signals:
    void Changed();
};

class NodeModelRep
{
    public:
    virtual void Draw(QPainter* painter) const = 0;
    virtual std::shared_ptr<NodeModel> GetModel() const = 0;
    //virtual void SetText(QString const& text) = 0;
    //virtual QString GetText() const = 0;
    //virtual bool HasText() const = 0;
    virtual ~NodeModelRep() = default;
};

class IntNode : public NodeModel
{
    Q_OBJECT

    public:
    IntNode(const std::shared_ptr<Node>& node);
    void Free();
    virtual bool IsPointOn(const QPointF& pos) const;
    std::shared_ptr<Node> m_node;
};

class IntNodeRep : public NodeModelRep
{
    public:
    using SingleNodePtr = std::shared_ptr<IntNode>;
    IntNodeRep(const SingleNodePtr& singleNode);
    void Draw(QPainter* painter) const override;

    std::shared_ptr<NodeModel> GetModel() const override;
    private:
    SingleNodePtr m_singleNode;
};

class IntVector : public NodeModel
{
    Q_OBJECT

    public:
    using NodePtr = std::shared_ptr<Node>;

    IntVector(const NodePtr& nodeA, const NodePtr& nodeB);

    virtual bool IsPointOn(const QPointF& pos) const;

    void FixOnDirection();
    void ParallelToDirection();
    void FreeVector();
   
    NodePtr m_nodeA;
    NodePtr m_nodeB;

    private:
    QVector2D m_vec;
    double Length() const;
};

class VectorRep : public NodeModelRep
{
    public:
    using VecPtr = std::shared_ptr<IntVector>;
    using NodeRepPtr = std::shared_ptr<IntNodeRep>;
    VectorRep(const VecPtr& vector);

    virtual void Draw(QPainter* painter) const override;

    std::shared_ptr<NodeModel> GetModel() const override;

    private:
    VecPtr m_vector;
    NodeRepPtr m_nodeARep;
    NodeRepPtr m_nodeBRep;
};

class IntPath : public NodeModel
{
    Q_OBJECT
    public:
    using NodePtr = std::shared_ptr<Node>;
    using VecPtr = std::shared_ptr<IntVector>;

    IntPath();
    virtual bool IsPointOn(const QPointF& pos) const;
    void AddPoint(const QPointF& newPoint);
    void AddNode(const NodePtr& newNode);
    void Close();

    NodePtr prevAddedNode = nullptr;
    NodePtr firstNode = nullptr;

    QList<VecPtr> m_vectors = QList<VecPtr>();
};

class PathRep : public NodeModelRep
{
    public:
    using PathPtr = std::shared_ptr<IntPath>;
    using VecRepPtr = std::shared_ptr<VectorRep>;

    PathRep(const PathPtr& path);
    virtual void Draw(QPainter* painter) const override;
    std::shared_ptr<NodeModel> GetModel() const override;

    private:
    PathPtr m_path;
    QList<VecRepPtr> m_vecReps;
};

class IntRect : public NodeModel
{
    Q_OBJECT

    public:

    IntRect(QRectF initialRect);
    virtual bool IsPointOn(const QPointF& pos) const;
    float AngleZ() const;
    float Height() const;
    float Width() const;

    std::shared_ptr<Node> m_nodeA;
    std::shared_ptr<Node> m_nodeB;
    std::shared_ptr<Node> m_nodeC;
    std::shared_ptr<Node> m_nodeD;
    std::shared_ptr<Node> m_nodeR;
    std::shared_ptr<Node> m_nodeM;

    private:

    void UpdateNodes();
    void RotateBy(double angle);
    
    QVector2D m_diaVecA;
    QVector2D m_diaVecB;
};

class RectRep : public NodeModelRep
{
    public:
    using NodeRepPtr = std::shared_ptr<IntNodeRep>;
    using PathRepPtr = std::shared_ptr<PathRep>;
    using RectPtr = std::shared_ptr<IntRect>;

    RectRep(RectPtr const& rect);
    virtual void Draw(QPainter* painter) const override;
    std::shared_ptr<NodeModel> GetModel() const override;

    NodeRepPtr m_nodeRRep;
    NodeRepPtr m_nodeMRep;
    NodeRepPtr m_nodeARep;
    NodeRepPtr m_nodeBRep;
    NodeRepPtr m_nodeCRep;
    NodeRepPtr m_nodeDRep;
    RectPtr m_rect;
};

class TextRep : public NodeModelRep
{
    public:
    using RectPtr = std::shared_ptr<IntRect>;
    using RectRepPtr = std::shared_ptr<RectRep>;

    TextRep(QString text, RectPtr rect);
    virtual void Draw(QPainter* painter) const override;
    std::shared_ptr<NodeModel> GetModel() const override;
    QString GetText() const;
    void SetText(QString const& text);

    private:
    RectPtr m_rect;
    RectRepPtr m_rectRep;
    QString m_text = "";
    
};

class EllipseRep: public NodeModelRep
{
    public:
    using RectRepPtr = std::shared_ptr<RectRep>;
    using RectPtr = std::shared_ptr<IntRect>;

    EllipseRep(RectPtr const& rect);
    virtual void Draw(QPainter* painter) const override;
    std::shared_ptr<NodeModel> GetModel() const override;

    RectPtr m_rect;
    RectRepPtr m_rectRep;
};