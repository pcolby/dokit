/*
    Copyright 2022 Paul Colby

    This file is part of QtPokit.

    QtPokit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QtPokit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QtPokit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "abstractcommand.h"

//Q_LOGGING_CATEGORY(pokitInfoCommand, "pokit.ui.informer", QtInfoMsg);

/*!
 * \class AbstractCommand
 * The AbstractCommand class provides a consistent base for the classes that implement CLI commands.
 */

/*!
 * Constructs a new worker with \a parent.
 */
AbstractCommand::AbstractCommand(QObject * const parent) : QObject(parent)
{

}

/*!
 * Returns a list of CLI option names required by this command. The main console appication may
 * use this list to output an eror (and exit) if any of the returned names are not found in the
 * parsed CLI options.
 *
 * This base implementation simply returns an empty list. Derived classes should override this
 * function to include any required options.
 */
QStringList AbstractCommand::requiredOptions() const
{
    return QStringList();
}

/*!
 * Returns a list of CLI option names supported by this command. The main console appication may
 * use this list to output a warning for any parsed CLI options not included in the returned list.
 *
 * This base implementation simply returns requiredOptions(). Derived classes should override this
 * function to include optional options, such as:
 *
 * ```
 * QStringList Derived::supportedOptions() const
 * {
 *     const QStringList list = AbstractCommand::supportedOptions() + QStringList{ ... };
 *     list.sort();
 *     list.removeDuplicates(); // Optional, recommended.
 *     return list;
 * }
 * ```
 */

QStringList AbstractCommand::supportedOptions() const
{
    return requiredOptions();
}

/*!
 * Processes the relevant options from the command line \a parser.
 *
 * Returns `true` if all required and supported options were processes succesfully, `false`
 * otherwise.
 *
 * Note, this base implementation does no processing, so will fail (return `false`) if any options
 * are required. Derived classes which oveeride requiredOptions() to return any options, must also
 * override this function to process them.
 */
bool AbstractCommand::processOptions(const QCommandLineParser &parser)
{
    Q_UNUSED(parser)
    return (requiredOptions().isEmpty());
}
