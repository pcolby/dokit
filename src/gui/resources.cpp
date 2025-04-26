// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "resources.h"
#include "../stringliterals_p.h"

#include <QDirIterator>

DOKIT_USE_STRINGLITERALS

QIcon loadIcon(const QString &path, const QStringList &nameFilters)
{
    QIcon icon;
    QDirIterator iter(path, nameFilters, QDir::Files|QDir::Readable);
    while (iter.hasNext()) {
        icon.addFile(iter.next());
    }
    return icon;
}

QIcon loadPokitLogoIcon()
{
    const QIcon icon = loadIcon(u":/icons/pokit"_s, { u"pokit-logo-*.png"_s });
    Q_ASSERT(icon.availableSizes().size() == 1);
    return icon;
}

QIcon loadPokitMeterIcon(const QString &color)
{
    const QIcon icon = loadIcon(u":/icons/pokit"_s, { u"pokit_meter-%1-*.png"_s.arg(color) });
    Q_ASSERT(icon.availableSizes().size() == 5);
    return icon;
}

QIcon loadPokitProIcon(const QString &color)
{
    const QIcon icon = loadIcon(u":/icons/pokit"_s, { u"pokit_pro-%1-*.png"_s.arg(color) });
    Q_ASSERT(icon.availableSizes().size() == 5);
    return icon;
}
