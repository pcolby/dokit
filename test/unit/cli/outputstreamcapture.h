// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DOKIT_TESTS_OUTPUTSTREAMCAPTURE_H
#define DOKIT_TESTS_OUTPUTSTREAMCAPTURE_H

#include <iostream>
#include <sstream>

/*!
 * Implements a simple RAII capture of output streams.
 */
class OutputStreamCapture
{
public:
    explicit OutputStreamCapture(std::ostream * const stream) : stream(stream)
    {
        originalBuffer = stream->rdbuf(newBuffer.rdbuf());
    }

    std::string data() const
    {
        return newBuffer.str();
    }

    ~OutputStreamCapture()
    {
        stream->rdbuf(originalBuffer);
    }

private:
    std::ostringstream newBuffer;
    std::streambuf * originalBuffer;
    std::ostream * stream;
};

#endif // DOKIT_TESTS_OUTPUTSTREAMCAPTURE_H
