#include "tst_QKDTreeTests.h"

#include "QKDTree.h"

#include <limits>

const uint size1 = 16000;
const uint size2 = 32000;

QKDTreeTests::QKDTreeTests()
{
}

//private test
void QKDTreeTests::insertionTest()
{
    QKDTree tree(2);
    tree.add(QPointF(0,0), 1);
    tree.add(QPointF(2,5), 3);
    tree.add(QPointF(-1,5), 2);
    tree.add(QPointF(1,6), 7);
    tree.add(QPointF(1,-2), 6);
    tree.add(QPointF(-3,7), 5);
    tree.add(QPointF(-1,4), 4);

    QKDTreeNode nearest;
    tree.nearest(QPointF(-1,5), &nearest);
    QVERIFY(nearest.position() == QVectorND(QPoint(-1,5)));

    //tree.debugPrint();
}

//private test
void QKDTreeTests::containsTest()
{
    QList<QVectorND> ref;

    QKDTree tree(3);

    for (int i = 0; i < 5000; i++)
    {
        QVectorND random = _randomNDimensional(3);

        ref.append(random);
        QVERIFY(tree.add(random,i));
    }

    for (int i = 0; i < 5000; i++)
        QVERIFY(tree.contains(ref[qrand() % ref.size()]));

    for (int i = 0; i < 5000; i++)
    {
        QVectorND random = _randomNDimensional(3);
        QVERIFY(tree.contains(random) == ref.contains(random));
    }
}

//private test
void QKDTreeTests::bigNearestTest()
{
    QList<QVectorND> backupList;
    QKDTree tree(4);

    for (int i = 0; i < 6000; i++)
    {
        QVectorND pos = _randomNDimensional(4);

        backupList.append(pos);
        QVERIFY(tree.add(pos, i));
    }

    QVERIFY(backupList.size() == tree.size());

    bool wasError = false;
    for (int i = 0; i < 5000; i++)
    {
        QVectorND pos = _randomNDimensional(4);

        QKDTreeNode nearest;
        tree.nearest(pos, &nearest);
        qreal treeDistance = tree.distanceMetric()->distance(pos,nearest.position());

        QVectorND backupNearest(4);
        qreal backupDist = 500000;
        foreach(const QVectorND& vec, backupList)
        {
            qreal dist = tree.distanceMetric()->distance(vec, pos);
            if (dist < backupDist)
            {
                backupNearest = vec;
                backupDist = dist;
            }
        }
        qreal listDistance = tree.distanceMetric()->distance(pos,nearest.position());

        if (nearest.position() != backupNearest && treeDistance != listDistance)
        {
            qDebug() << "Tree: Nearest to" << pos << "is" << nearest.position() << treeDistance;
            qDebug() << "List: Nearest to" << pos << "is" << backupNearest << listDistance;
            wasError = true;
        }
    }

    if (wasError)
    {
        //tree.debugPrint();
        QVERIFY(false);
    }
}

//private test
void QKDTreeTests::benchmarkTreeAdd1()
{
    QKDTree tree(2);
    for (int i = 0; i < size1; i++)
    {
        QVectorND pos = _randomNDimensional(2);

        tree.add(pos, i);
    }

    QBENCHMARK
    {
        QVectorND pos = _randomNDimensional(2);

        tree.add(pos, "final");
    }
}

//private test
void QKDTreeTests::benchmarkTreeAdd2()
{
    QKDTree tree(2);
    for (int i = 0; i < size2; i++)
    {
        QVectorND pos = _randomNDimensional(2);

        tree.add(pos, i);
    }

    QBENCHMARK
    {
        QVectorND pos = _randomNDimensional(2);

        tree.add(pos, "final");
    }
}

//private test
void QKDTreeTests::benchmarkTreeNearest1()
{
    QKDTree tree(2);
    QList<QVectorND> results;

    for (int i = 0; i < size1; i++)
    {
        QVectorND pos = _randomNDimensional(2);

        tree.add(pos, i);
    }

    for (int i = 0; i < 1; i++)
    {
        QVectorND pos = _randomNDimensional(2);

        QKDTreeNode nearestResult;
        QBENCHMARK
        {
            tree.nearest(pos, &nearestResult);
        }

        results.append(nearestResult.position());
    }
}

//private test
void QKDTreeTests::benchmarkTreeNearest2()
{
    QKDTree tree(2);
    QList<QVectorND> results;

    for (int i = 0; i < size2; i++)
    {
        QVectorND pos = _randomNDimensional(2);

        tree.add(pos, i);
    }

    for (int i = 0; i < 1; i++)
    {
        QVectorND pos = _randomNDimensional(2);

        QKDTreeNode nearestResult;
        QBENCHMARK
        {
            tree.nearest(pos, &nearestResult);
        }

        results.append(nearestResult.position());
    }
}

//private test
void QKDTreeTests::benchmarkListAdd1()
{
    QList<QVectorND> list;

    for (int i = 0; i < size1; i++)
    {
        QVectorND pos = _randomNDimensional(2);
        list.append(pos);
    }

    QBENCHMARK
    {
        QVectorND pos = _randomNDimensional(2);
        list.append(pos);
    }
}

//private test
void QKDTreeTests::benchmarkListAdd2()
{
    QList<QVectorND> list;

    for (int i = 0; i < size2; i++)
    {
        QVectorND pos = _randomNDimensional(2);
        list.append(pos);
    }

    QBENCHMARK
    {
        QVectorND pos = _randomNDimensional(2);
        list.append(pos);
    }
}

//private test
void QKDTreeTests::benchmarkListNearest1()
{
    QList<QVectorND> list;
    QList<QVectorND> results;

    for (int i = 0; i < size1; i++)
    {
        QVectorND pos = _randomNDimensional(2);
        list.append(pos);
    }

    for (int i = 0; i < 1; i++)
    {
        QVectorND pos = _randomNDimensional(2);

        QVectorND best(1);
        QBENCHMARK
        {
            qreal bestDist = std::numeric_limits<qreal>::max();
            foreach(const QVectorND& vec, list)
            {
                const qreal dist = (vec - pos).lengthSquared();
                if (dist < bestDist)
                {
                    bestDist = dist;
                    best = vec;
                }
            }
        }
        results.append(best);
    }
}

//private test
void QKDTreeTests::benchmarkListNearest2()
{
    QList<QVectorND> list;
    QList<QVectorND> results;

    for (int i = 0; i < size2; i++)
    {
        QVectorND pos = _randomNDimensional(2);
        list.append(pos);
    }

    for (int i = 0; i < 1; i++)
    {
        QVectorND pos = _randomNDimensional(2);

        QVectorND best(1);
        QBENCHMARK
        {
            qreal bestDist = std::numeric_limits<qreal>::max();
            foreach(const QVectorND& vec, list)
            {
                const qreal dist = (vec - pos).lengthSquared();
                if (dist < bestDist)
                {
                    bestDist = dist;
                    best = vec;
                }
            }
        }
        results.append(best);
    }
}

//private static
QVectorND QKDTreeTests::_randomNDimensional(int n)
{
    QVectorND toRet(n);

    for (int i = 0; i < n; i++)
        toRet[i] = qrand();

    return toRet;
}
