/*
 * Copyright (c) 2022, Matthew Olsson <mattco@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/DeprecatedString.h>

namespace PDF {

class Error {
public:
    enum class Type {
        Parse,
        Internal,
        MalformedPDF,
        RenderingUnsupported
    };

    Error(AK::Error error)
        : m_type(Type::Internal)
        , m_message(DeprecatedString::formatted("Internal error while processing PDF file: {}", error.string_literal()))
    {
    }

    Error(Type type, DeprecatedString const& message)
        : m_type(type)
    {
        switch (type) {
        case Type::Parse:
            m_message = DeprecatedString::formatted("Failed to parse PDF file: {}", message);
            break;
        case Type::Internal:
            m_message = DeprecatedString::formatted("Internal error while processing PDF file: {}", message);
            break;
        case Type::MalformedPDF:
            m_message = DeprecatedString::formatted("Malformed PDF file: {}", message);
            break;
        case Type::RenderingUnsupported:
            m_message = DeprecatedString::formatted("Rendering of feature not supported: {}", message);
            break;
        }
    }

    Type type() const { return m_type; }
    DeprecatedString const& message() const { return m_message; }

private:
    Type m_type;
    DeprecatedString m_message;
};

template<typename T>
using PDFErrorOr = ErrorOr<T, Error>;

}
