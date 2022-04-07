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

#include <QObject>

class QLowEnergyController;

/*!
 * Simple class for experimenting with the Pokit device info service.
 *
 * This class will be signifantly refactored, or maybe replaced entirely soonish.
 */
class Informer : public QObject
{
public:
    explicit Informer(QObject * const parent);

public slots:

private:
    QLowEnergyController * controller; ///< BLE controller for accessing the Device Info service.

private slots:
};
