#pragma once
#ifndef MINSBASE_MACRO_HELPERS_H_INCLUDED
#define MINSBASE_MACRO_HELPERS_H_INCLUDED

#ifndef IMPLANT_PRAGMA
# if defined(_MSC_VER)
#   define IMPLANT_PRAGMA(x) __pragma(x)
# else
#   define IMPLANT_PRAGMA(x) _Pragma(#x)
# endif
#endif

#ifndef CONCAT_TOKENS
# define CONCAT_TOKENS_INTERNAL(x, y) x ## y
# define CONCAT_TOKENS(x, y) CONCAT_TOKENS_INTERNAL(x, y)
#endif

#ifndef DECLARE_COMPOUND_WITH_TYPEDEF
# define DECLARE_COMPOUND_WITH_TYPEDEF(cmpnd, name) typedef cmpnd name name
#endif

#endif // #ifndef MINSBASE_MACRO_HELPERS_H_INCLUDED
