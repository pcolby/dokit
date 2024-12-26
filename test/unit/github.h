// SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DOKIT_TESTS_GITHUB_H
#define DOKIT_TESTS_GITHUB_H

#include <QOperatingSystemVersion>

/*!
 * Returns \c true if the application is executing on a GitHub Actions runner, \c false otherwise.
 */
inline bool isGitHubActionsRunner()
{
    return qgetenv("GITHUB_ACTIONS") == "true";
}

/*!
 * Returns the host operating system's version information if executing on a GitHub Actions runner, otherwise
 * a QOperatingSystemVersion::Unknown version.
 */
inline QOperatingSystemVersion gitHubActionsRunnerOsVersion()
{
    return isGitHubActionsRunner() ? QOperatingSystemVersion::current()
                                   : QOperatingSystemVersion(QOperatingSystemVersion::Unknown, -1);
}

#endif // DOKIT_TESTS_GITHUB_H
