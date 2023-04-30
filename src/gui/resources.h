// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DOKIT_GUI_RESOURCES_H
#define DOKIT_GUI_RESOURCES_H

#include <QIcon>

QIcon loadIcon(const QString &path, const QStringList &nameFilters);

QIcon loadPokitMeterIcon(const QString &color);
QIcon loadPokitProIcon(const QString &color);

#endif // DOKIT_GUI_RESOURCES_H
