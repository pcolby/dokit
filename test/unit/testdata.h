// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QTPOKIT_TESTS_TESTDATA_H
#define QTPOKIT_TESTS_TESTDATA_H

#include <QTest>

inline void loadTestData(const char * const testClassName, QByteArray &output)
{
    QString className = QString::fromUtf8(testClassName);
    if (className.startsWith(QStringLiteral("Test"))) {
        className.remove(0, 4);
    }
    const QString dataBasePath = QStringLiteral("testdata/%1-%2-%3").arg(className,
        QString::fromUtf8(QTest::currentTestFunction()), QString::fromUtf8(QTest::currentDataTag()));
    const QString dataPathName = QFINDTESTDATA(dataBasePath);
    QVERIFY2(!dataPathName.isEmpty(),
             qUtf8Printable(QStringLiteral("failed to locate test data: %1").arg(dataBasePath)));
    QFile dataFile(dataPathName);
    QVERIFY2(dataFile.open(QFile::ReadOnly),
             qUtf8Printable(QStringLiteral("open %1: %2").arg(dataFile.fileName(), dataFile.errorString())));
    output = dataFile.readAll();
}

inline QByteArray loadTestData(const char * const testClassName)
{
    QByteArray data;
    loadTestData(testClassName, data);
    return data;
}

#define LOADTESTDATA(name) const QByteArray name = loadTestData(metaObject()->className());

#endif // QTPOKIT_TESTS_TESTDATA_H
