#pragma once

#include "types.hpp"
#include <cassert>

namespace fizzy
{
inline const FuncType& function_type(const Module& module, FuncIdx idx)
{
    assert(idx < module.imported_function_types.size() + module.funcsec.size());

    if (idx < module.imported_function_types.size())
        return module.imported_function_types[idx];

    const auto type_idx = module.funcsec[idx - module.imported_function_types.size()];
    assert(type_idx < module.typesec.size());

    return module.typesec[type_idx];
}
}  // namespace fizzy
