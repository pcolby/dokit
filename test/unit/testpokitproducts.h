// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestPokitProducts : public QObject
{
    Q_OBJECT

private slots:
    void toString_PokitProduct_data();
    void toString_PokitProduct();

    void isPokitProduct_data();
    void isPokitProduct();

    void pokitProduct_data();
    void pokitProduct();

    /// \todo Unit tests for pokitproduct_p.h functions too.
};
