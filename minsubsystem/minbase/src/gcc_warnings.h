#pragma once
#ifndef MINSBASE_SRC_GCC_WARNINGS_H_INCLUDED
#define MINSBASE_SRC_GCC_WARNINGS_H_INCLUDED

#ifdef __GNUC__

#include <minbase/macro_helpers.h>

#define DISABLE_GCC_4_8_C_CXX_WARNINGS \
  IMPLANT_PRAGMA(GCC diagnostic ignored "--all-warnings") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "--extra-warnings") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-W") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wabi") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Waddress") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Waggregate-return") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Waggressive-loop-optimizations") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wall") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Warray-bounds") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wattributes") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wbuiltin-macro-redefined") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wcast-align") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wcast-qual") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wchar-subscripts") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wclobbered") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wcomment") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wcomments") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wconversion") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wcoverage-mismatch") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wcpp") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wdeprecated") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wdeprecated-declarations") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wdisabled-optimization") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wdiv-by-zero") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wdouble-promotion") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wempty-body") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wendif-labels") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wenum-compare") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wextra") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wfloat-equal") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wformat") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wformat=") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wformat-contains-nul") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wformat-extra-args") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wformat-nonliteral") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wformat-security") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wformat-y2k") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wformat-zero-length") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wframe-larger-than=") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wfree-nonheap-object") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wignored-qualifiers") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Winit-self") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Winline") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wint-to-pointer-cast") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Winvalid-memory-model") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Winvalid-pch") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wlarger-than-") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wlarger-than=") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wlogical-op") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wlong-long") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wmain") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wmaybe-uninitialized") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wmissing-braces") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wmissing-declarations") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wmissing-field-initializers") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wmissing-format-attribute") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wmissing-include-dirs") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wmissing-noreturn") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wmultichar") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wnarrowing") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wnonnull") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wnormalized=") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Woverflow") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Woverlength-strings") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wpacked") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wpacked-bitfield-compat") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wpadded") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wparentheses") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wpedantic") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wpointer-arith") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wpragmas") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wproperty-assign-default") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wprotocol") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wrealloc-lhs") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wrealloc-lhs-all") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wreal-q-constant") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wredundant-decls") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wreorder") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wreturn-local-addr") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wreturn-type") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wselector") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wsequence-point") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wshadow") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wsign-compare") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wsign-conversion") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wsign-promo") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wsizeof-pointer-memaccess") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wstack-protector") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wstack-usage=") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wstrict-aliasing") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wstrict-aliasing=") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wstrict-null-sentinel") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wstrict-overflow") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wstrict-overflow=") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wstrict-prototypes") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wstrict-selector-match") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wsuggest-attribute=const") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wsuggest-attribute=format") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wsuggest-attribute=noreturn") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wsuggest-attribute=pure") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wsurprising") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wswitch") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wswitch-default") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wswitch-enum") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wsync-nand") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wsynth") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wsystem-headers") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wtabs") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wtarget-lifetime") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wtraditional") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wtraditional-conversion") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wtrampolines") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wtrigraphs") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wtype-limits") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wundeclared-selector") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wundef") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunderflow") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wuninitialized") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunknown-pragmas") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunreachable-code") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunsafe-loop-optimizations") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunsuffixed-float-constants") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunused") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunused-but-set-parameter") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunused-but-set-variable") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunused-dummy-argument") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunused-function") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunused-label") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunused-local-typedefs") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunused-macros") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunused-parameter") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunused-result") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunused-value") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunused-variable") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wuseless-cast") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wuse-without-only") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wvarargs") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wvariadic-macros") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wvector-operation-performance") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wvirtual-move-assign") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wvla") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wvolatile-register-var") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wwrite-strings") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wzero-as-null-pointer-constant")

#define DISABLE_GCC_4_8_C_WARNINGS \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wbad-function-cast") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wc++-compat") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wdeclaration-after-statement") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Werror-implicit-function-declaration") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wimplicit") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wimplicit-function-declaration") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wimplicit-int") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wjump-misses-init") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wmissing-parameter-type") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wmissing-prototypes") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wnested-externs") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wold-style-declaration") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wold-style-definition") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Woverride-init") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wpointer-sign") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wpointer-to-int-cast")

#define DISABLE_GCC_4_8_CXX_WARNINGS \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wabi-tag") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wc++0x-compat") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wc++11-compat") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wctor-dtor-privacy") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wdelete-non-virtual-dtor") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Weffc++") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Winherited-variadic-ctor") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Winvalid-offsetof") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wliteral-suffix") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wnoexcept") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wnon-template-friend") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wnon-virtual-dtor") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wold-style-cast") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Woverloaded-virtual") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wpmf-conversions")

#define DISABLE_GCC_4_9_C_CXX_WARNINGS DISABLE_GCC_4_8_C_CXX_WARNINGS \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wopenmp-simd") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wdate-time") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wfloat-conversion")

#define DISABLE_GCC_4_9_C DISABLE_GCC_4_8_C_WARNINGS

#define DISABLE_GCC_4_9_CXX_WARNINGS DISABLE_GCC_4_8_CXX_WARNINGS \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wconditionally-supported") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wdelete-incomplete")

#define DISABLE_GCC_5_X_C_CXX_WARNINGS DISABLE_GCC_4_9_C_CXX_WARNINGS \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wabi=") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Warray-bounds=") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wbool-compare") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wchkp") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wformat-signedness") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wlogical-not-parentheses") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wmemset-transposed-args") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wnormalized") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wodr") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wshadow-ivar") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wshift-count-negative") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wshift-count-overflow") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wsized-deallocation") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wsizeof-array-argument") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wsuggest-final-methods") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wsuggest-final-types") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wsuggest-override") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wswitch-bool")

#define DISABLE_GCC_5_X_C_WARNINGS DISABLE_GCC_4_9_C_WARNINGS \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wc90-c99-compat") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wc99-c11-compat") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wdesignated-init") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wdiscarded-array-qualifiers") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wdiscarded-qualifiers") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wincompatible-pointer-types") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wint-conversion")

#define DISABLE_GCC_5_X_CXX_WARNINGS DISABLE_GCC_4_9_CXX_WARNINGS \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wc++14-compat")


#define DISABLE_GCC_6_X_C_CXX_WARNINGS DISABLE_GCC_5_X_C_CXX_WARNINGS \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wduplicated-cond") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wframe-address") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Whsa") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wignored-attributes") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wlto-type-mismatch") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wmisleading-indentation") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wnonnull-compare") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wnull-dereference") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wscalar-storage-order") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wshift-negative-value") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wshift-overflow") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wshift-overflow=") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wsubobject-linkage") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wtautological-compare") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wtemplates") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wterminate") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunused-const-variable") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunused-const-variable=") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wvirtual-inheritance")

#define DISABLE_GCC_6_X_C_WARNINGS DISABLE_GCC_5_X_C_WARNINGS \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Woverride-init-side-effects") \

#define DISABLE_GCC_6_X_CXX_WARNINGS DISABLE_GCC_5_X_CXX_WARNINGS \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wmultiple-inheritance") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wnamespaces") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wplacement-new") \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wplacement-new=") \


// TODO
#define DISABLE_GCC_7_X_C_CXX_WARNINGS DISABLE_GCC_6_X_C_CXX_WARNINGS

// TODO
#define DISABLE_GCC_7_X_C_WARNINGS DISABLE_GCC_6_X_C_WARNINGS

// TODO
#define DISABLE_GCC_7_X_CXX_WARNINGS DISABLE_GCC_6_X_CXX_WARNINGS \
  IMPLANT_PRAGMA(GCC diagnostic ignored "-Wint-in-bool-context")


#if __GNUC__ >= 5
# define LOCAL_GNUC_TOKEN CONCAT_TOKENS(__GNUC__, _X)
#else
# define LOCAL_GNUC_TOKEN CONCAT_TOKENS(__GNUC__, CONCAT_TOKENS(_, __GNUC_MINOR__))
#endif

#define LOCAL_DISABLE_PREFIX CONCAT_TOKENS(DISABLE_GCC_, LOCAL_GNUC_TOKEN)

#ifndef DISABLE_GCC_WARNINGS
# ifdef __cplusplus
#   define DISABLE_GCC_WARNINGS \
        IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunknown-pragmas") \
        CONCAT_TOKENS(LOCAL_DISABLE_PREFIX, _C_CXX_WARNINGS) \
        CONCAT_TOKENS(LOCAL_DISABLE_PREFIX, _CXX_WARNINGS)
# else
#   define DISABLE_GCC_WARNINGS \
        IMPLANT_PRAGMA(GCC diagnostic ignored "-Wunknown-pragmas") \
        CONCAT_TOKENS(LOCAL_DISABLE_PREFIX, _C_CXX_WARNINGS) \
        CONCAT_TOKENS(LOCAL_DISABLE_PREFIX, _C_WARNINGS)
# endif
#endif // #ifndef DISABLE_GCC_WARNINGS

#endif // #ifdef __GNUC__

#endif // #ifndef MINSBASE_SRC_GCC_WARNINGS_H_INCLUDED

