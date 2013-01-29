#ifndef QKDTREEDISTANCEMETRIC_H
#define QKDTREEDISTANCEMETRIC_H

#include "QKDTreeNode.h"

class QKDTreeDistanceMetric
{
public:
    QKDTreeDistanceMetric();
    virtual ~QKDTreeDistanceMetric();

    qreal distance(const QKDTreeNode * const a, const QKDTreeNode * const b);
    virtual qreal distance(const QVectorND& a, const QVectorND& b);
};

#endif // QKDTREEDISTANCEMETRIC_H
