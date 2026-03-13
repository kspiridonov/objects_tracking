#pragma once
#include <string>
#include <magic_enum/magic_enum.hpp>
#include "core/exception/wrong_args_exception.h"

class EnumUtil {
public:
    template<typename E>
    static std::string toString(E value) {
        return std::string(magic_enum::enum_name(value));
    }

    template<typename E>
    static E fromString(std::string name) {
        transform(name.begin(), name.end(), name.begin(),
              ::toupper);
        auto value = magic_enum::enum_cast<E>(name);
        if (value.has_value()) {
            return value.value();
        }
        throw WrongArgsException("Try to get Enum by unknown string value: " + name);
    }

    template<typename E>
    static int toInt(E value) {
        return magic_enum::enum_integer(value);
    }
};
