#ifndef QKDTREE_H
#define QKDTREE_H

#include "QKDTree_global.h"

#include "QKDTreeNode.h"
#include "QKDTreeDistanceMetric.h"
#include "QVectorND.h"

class QKDTREESHARED_EXPORT QKDTree
{
public:
    QKDTree(uint dimension);
    ~QKDTree();

    uint dimension() const;
    qint64 size() const;

    bool add(QKDTreeNode * node, QString * resultOut = 0);
    bool add(const QVectorND& position, const QVariant& value, QString * resultOut = 0);
    bool add(const QPointF& position, const QVariant& value, QString * resultOut = 0);

    bool nearest(const QVectorND& position, QKDTreeNode * output, QString * resultOut = 0);
    bool nearest(QKDTreeNode * node, QKDTreeNode * output, QString * resultOut = 0);

    bool setDistanceMetric(QKDTreeDistanceMetric * nMetric, QString * resultOut = 0);
    QKDTreeDistanceMetric * distanceMetric() const;

    void debugPrint();

private:
    uint _dimension;
    qint64 _size;

    QKDTreeNode * _root;

    QKDTreeDistanceMetric * _distanceMetric;
};

#endif // QKDTREE_H
