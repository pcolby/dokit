// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "resources.h"

#include <QDirIterator>

QIcon loadIcon(const QString &path, const QStringList &nameFilters)
{
    QIcon icon;
    QDirIterator iter(path, nameFilters, QDir::Files|QDir::Readable);
    while (iter.hasNext()) {
        icon.addFile(iter.next());
    }
    return icon;
}

QIcon loadPokitMeterIcon(const QString &color)
{
    const QIcon icon = loadIcon(QStringLiteral(":/icons/pokit"),
        { QStringLiteral("pokit_meter-%1-*.png").arg(color) });
    Q_ASSERT(icon.availableSizes().size() == 5);
    return icon;
}

QIcon loadPokitProIcon(const QString &color)
{
    const QIcon icon = loadIcon(QStringLiteral(":/icons/pokit"),
        { QStringLiteral("pokit_pro-%1-*.png").arg(color) });
    Q_ASSERT(icon.availableSizes().size() == 5);
    return icon;
}
