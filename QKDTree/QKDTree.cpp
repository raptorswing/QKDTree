#include "QKDTree.h"

#include <QQueue>
#include <QStack>
#include <QtDebug>
#include <limits>


const QString ERR_STRING_BAD_DIM = "Dimension of position does not match that of tree.";
const QString ERR_STRING_BAD_OUTPTR = "You didn't provide a pointer for output.";

QKDTree::QKDTree(uint dimension) :
    _dimension(dimension), _size(0), _root(0)
{
    _distanceMetric = new QKDTreeDistanceMetric();
}

QKDTree::~QKDTree()
{
    if (_size > 0)
    {
        QQueue<QKDTreeNode *> deleteQueue;
        deleteQueue.enqueue(_root);

        while (!deleteQueue.isEmpty())
        {
            QKDTreeNode * current = deleteQueue.dequeue();
            if (current->left())
                deleteQueue.enqueue(current->left());
            if (current->right())
                deleteQueue.enqueue(current->right());
            delete current;
        }

        _root = 0;
        _size = 0;
    }
    delete _distanceMetric;
}

uint QKDTree::dimension() const
{
    return _dimension;
}

qint64 QKDTree::size() const
{
    return _size;
}

bool QKDTree::add(QKDTreeNode *node, QString *resultOut)
{
    if (node == 0)
    {
        if (resultOut)
            *resultOut = "Cannot add null node";
        return false;
    }
    else if (node->position().dimension() != this->dimension())
    {
        if (resultOut)
            *resultOut = ERR_STRING_BAD_DIM;
        return false;
    }

    //Special case for first node in the tree!
    if (_root == 0)
    {
        _root = node;
        _size++;
        return true;
    }

    //Otherwise, normal insertion
    QKDTreeNode * potentialParent = _root;
    while (true)
    {
        const int divDim = potentialParent->dividingDimension();
        if (node->position().val(divDim) <= potentialParent->position().val(divDim))
        {
            if (potentialParent->left() != 0)
                potentialParent = potentialParent->left();
            else
            {
                potentialParent->setLeft(node);
                node->setDividingDimension((divDim + 1) % this->dimension());
                break;
            }
        }
        else
        {
            if (potentialParent->right() != 0)
                potentialParent = potentialParent->right();
            else
            {
                potentialParent->setRight(node);
                node->setDividingDimension((divDim + 1) % this->dimension());
                break;
            }
        }
    }

    _size++;
    return true;
}

bool QKDTree::add(const QVectorND &position, const QVariant &value, QString *resultOut)
{
    if (position.dimension() != this->dimension())
    {
        if (resultOut)
            *resultOut = ERR_STRING_BAD_DIM;
        return false;
    }

    QKDTreeNode * node = new QKDTreeNode(position, value);

    return this->add(node, resultOut);
}

bool QKDTree::add(const QPointF &position, const QVariant &value, QString *resultOut)
{
    if (this->dimension() != 2)
    {
        if (resultOut)
            *resultOut = ERR_STRING_BAD_DIM;
        return false;
    }

    QKDTreeNode * node = new QKDTreeNode(QVectorND(position), value);

    return this->add(node, resultOut);
}


bool QKDTree::nearest(const QVectorND &position, QKDTreeNode *output, QString *resultOut)
{
    if (output == 0)
    {
        if (resultOut)
            *resultOut = ERR_STRING_BAD_OUTPTR;
        return false;
    }
    else if (position.dimension() != this->dimension())
    {
        if (resultOut)
            *resultOut = ERR_STRING_BAD_DIM;
        return false;
    }
    else if (_size <= 0)
    {
        if (resultOut)
            *resultOut = "Tree is empty";
        return false;
    }

    QQueue<QKDTreeNode *> descend;
    QStack<QKDTreeNode *> unwindChecks;

    descend.enqueue(_root);

    QKDTreeNode * bestSoFar = 0;
    qreal bestDistSoFar = std::numeric_limits<qreal>::max();

    while (!descend.isEmpty() || !unwindChecks.isEmpty())
    {
        if (!descend.isEmpty())
        {
            QKDTreeNode * current = descend.dequeue();
            //qDebug() << "Descend" << current->position();
            unwindChecks.push(current);

            int divDim = current->dividingDimension();
            if (position.val(divDim) <= current->position().val(divDim))
            {
                if (current->left() != 0)
                    descend.enqueue(current->left());
                else
                {
                    const qreal dist = _distanceMetric->distance(current->position(), position);
                    if (dist < bestDistSoFar)
                    {
                        bestSoFar = current;
                        bestDistSoFar = dist;
                        //qDebug() << "Best on left" << current->position() << bestDistSoFar;
                    }
                }
            }
            else
            {
                if (current->right() != 0)
                    descend.enqueue(current->right());
                else
                {
                    const qreal dist = _distanceMetric->distance(current->position(), position);
                    if (dist < bestDistSoFar)
                    {
                        bestSoFar = current;
                        bestDistSoFar = dist;
                        //qDebug() << "Best on right" << current->position() << bestDistSoFar;
                    }
                }
            }
        }
        else
        {
            QKDTreeNode * current = unwindChecks.pop();
            //qDebug() << "Unwind" << current->position();
            const int divDim = current->dividingDimension();
            const qreal dist = _distanceMetric->distance(current->position(), position);
            if (dist < bestDistSoFar)
            {
                bestSoFar = current;
                bestDistSoFar = dist;
                //qDebug() << "Best unwound" << current->position() << bestDistSoFar;
            }

            //Do we need to check other side of hyperplane?
            //QVectorND temp = position;
            //temp[divDim] = current->position()[divDim];
            //qDebug() << temp << "to" << current->position() << _distanceMetric->distance(current->position(), temp);
            const qreal hyperplaneDistance = position.val(divDim) - current->position().val(divDim);
            if (hyperplaneDistance*hyperplaneDistance > bestDistSoFar)
                continue;

            if (position.val(divDim) <= current->position().val(divDim)
                    && current->right() != 0)
                descend.enqueue(current->right());
            else if (current->left() != 0)
                descend.enqueue(current->left());

        }
    }

    *output = *bestSoFar;

    return true;
}

bool QKDTree::nearest(QKDTreeNode *node, QKDTreeNode *output, QString *resultOut)
{
    if (output == 0)
    {
        if (resultOut)
            *resultOut = ERR_STRING_BAD_OUTPTR;
        return false;
    }

    return this->nearest(node->position(), output, resultOut);
}

bool QKDTree::setDistanceMetric(QKDTreeDistanceMetric *nMetric, QString *resultOut)
{
    if (nMetric == 0)
    {
        if (resultOut)
            *resultOut = "Can't set null distance metric.";
        return false;
    }

    //Delete old, set new
    delete _distanceMetric;
    _distanceMetric = nMetric;

    return true;
}

QKDTreeDistanceMetric *QKDTree::distanceMetric() const
{
    return _distanceMetric;
}

void QKDTree::debugPrint()
{
    if (_size <= 0)
        return;

    QQueue<QKDTreeNode *> q;
    q.enqueue(_root);

    while (!q.isEmpty())
    {
        QKDTreeNode * n = q.dequeue();
        qDebug() << n->position() << n->value();

        if (n->left())
            q.enqueue(n->left());
        if (n->right())
            q.enqueue(n->right());
    }
}
