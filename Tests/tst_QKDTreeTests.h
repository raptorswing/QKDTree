#ifndef TST_QKDTREETESTS_H
#define TST_QKDTREETESTS_H

#include <QString>
#include <QtTest>

class QKDTreeTests : public QObject
{
    Q_OBJECT

public:
    QKDTreeTests();

private Q_SLOTS:
    void insertionTest();
    void bigNearestTest();

    void benchmarkTreeAdd1();
    void benchmarkTreeAdd2();

    void benchmarkTreeNearest1();
    void benchmarkTreeNearest2();

    void benchmarkListAdd1();
    void benchmarkListAdd2();

    void benchmarkListNearest1();
    void benchmarkListNearest2();
};

#endif // TST_QKDTREETESTS_H
