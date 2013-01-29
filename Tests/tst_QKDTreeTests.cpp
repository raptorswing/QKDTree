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
void QKDTreeTests::bigNearestTest()
{
    QList<QVectorND> backupList;
    QKDTree tree(2);

    for (int i = 0; i < 6000; i++)
    {
        QVectorND pos(2);
        pos[0] = qrand() % 100;
        pos[1] = qrand() % 100;

        backupList.append(pos);
        tree.add(pos, i);
    }

    QVERIFY(backupList.size() == tree.size());

    bool wasError = false;
    for (int i = 0; i < 5000; i++)
    {
        QVectorND pos(2);
        pos[0] = qrand() % 100;
        pos[1] = qrand() % 100;

        QKDTreeNode nearest;
        tree.nearest(pos, &nearest);
        qreal treeDistance = tree.distanceMetric()->distance(pos,nearest.position());

        QVectorND backupNearest(2);
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
        QVectorND pos(2);
        pos[0] = qrand();
        pos[1] = qrand();

        tree.add(pos, i);
    }

    QBENCHMARK
    {
        QVectorND pos(2);
        pos[0] = qrand();
        pos[1] = qrand();

        tree.add(pos, "final");
    }
}

//private test
void QKDTreeTests::benchmarkTreeAdd2()
{
    QKDTree tree(2);
    for (int i = 0; i < size2; i++)
    {
        QVectorND pos(2);
        pos[0] = qrand();
        pos[1] = qrand();

        tree.add(pos, i);
    }

    QBENCHMARK
    {
        QVectorND pos(2);
        pos[0] = qrand();
        pos[1] = qrand();

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
        QVectorND pos(2);
        pos[0] = qrand();
        pos[1] = qrand();

        tree.add(pos, i);
    }

    for (int i = 0; i < 1; i++)
    {
        QVectorND pos(2);
        pos[0] = qrand();
        pos[1] = qrand();

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
        QVectorND pos(2);
        pos[0] = qrand();
        pos[1] = qrand();

        tree.add(pos, i);
    }

    for (int i = 0; i < 1; i++)
    {
        QVectorND pos(2);
        pos[0] = qrand();
        pos[1] = qrand();

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
        QVectorND pos(2);
        pos[0] = qrand();
        pos[1] = qrand();
        list.append(pos);
    }

    QBENCHMARK
    {
        QVectorND pos(2);
        pos[0] = qrand();
        pos[1] = qrand();
        list.append(pos);
    }
}

//private test
void QKDTreeTests::benchmarkListAdd2()
{
    QList<QVectorND> list;

    for (int i = 0; i < size2; i++)
    {
        QVectorND pos(2);
        pos[0] = qrand();
        pos[1] = qrand();
        list.append(pos);
    }

    QBENCHMARK
    {
        QVectorND pos(2);
        pos[0] = qrand();
        pos[1] = qrand();
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
        QVectorND pos(2);
        pos[0] = qrand();
        pos[1] = qrand();
        list.append(pos);
    }

    for (int i = 0; i < 1; i++)
    {
        QVectorND pos(2);
        pos[0] = qrand();
        pos[1] = qrand();

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
        QVectorND pos(2);
        pos[0] = qrand();
        pos[1] = qrand();
        list.append(pos);
    }

    for (int i = 0; i < 1; i++)
    {
        QVectorND pos(2);
        pos[0] = qrand();
        pos[1] = qrand();

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
