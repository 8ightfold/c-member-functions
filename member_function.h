#ifndef PREPROCESSOR_MEMBERFUNCTION_H
#define PREPROCESSOR_MEMBERFUNCTION_H

#include <stddef.h>

#define $XCAT(x, y) x##y
#define $CAT(x, y) $XCAT(x, y)

#define $XTRICAT(x, y, z) x##y##z
#define $TRICAT(x, y, z) $XTRICAT(x, y, z)

#define $EXPAND(...) __VA_ARGS__
#define $EAT(...)


#include <boost/preprocessor.hpp>

#define $TO_LIST(...) BOOST_PP_TUPLE_TO_LIST((__VA_ARGS__))
#define $TO_SEQ(...) BOOST_PP_TUPLE_TO_SEQ((__VA_ARGS__))

// TRAMPOLINE: (ret)(name)(args...) -> name, (ret)(args...)
// "type" manipulation
#define $GET_RET_I(ret) ret $EAT
#define $GET_RET(type) $GET_RET_I type

#define $GET_ARGS_EXPAND(_, ...) __VA_ARGS__
#define $GET_ARGS_I(ret) $GET_ARGS_EXPAND
#define $GET_ARGS(type) $GET_ARGS_I type

#define $ARG_COUNT_COUNT(...) BOOST_PP_DEC(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__))
#define $ARG_COUNT_I(ret) $ARG_COUNT_COUNT
#define $ARG_COUNT(type) $ARG_COUNT_I type

#define $GET_BASE_EXPAND(base, ...) base
#define $GET_BASE_I(ret) $GET_BASE_EXPAND
#define $GET_BASE(type) $GET_BASE_I type

#define $EXTRACT_NAME(fn) BOOST_PP_SEQ_ELEM(1, fn)
#define $EXTRACT_TYPE(fn) (BOOST_PP_SEQ_HEAD(fn))BOOST_PP_SEQ_REST_N(2, fn)

#define $VA_COMMA_I() ,
#define $VA_COMMA(count) BOOST_PP_IF(count, $VA_COMMA_I, $EAT)
#define $TVA_COMMA(type) $VA_COMMA($ARG_COUNT(type))()

// function argument passing / trampoline registry
#define $RT_ENUMERATE_F(z, n, t) $CAT(_, n),
#define $RT_ENUMERATE_TYPE_F(r, data, i, type) type $CAT(data, i),

#define $RT_ENUMERATE_II(count)     BOOST_PP_TUPLE_POP_BACK(( BOOST_PP_REPEAT(count, $RT_ENUMERATE_F,) BOOST_PP_NIL ))
#define $RT_ENUMERATE_TYPE_II(list) BOOST_PP_TUPLE_POP_BACK(( BOOST_PP_LIST_FOR_EACH_I($RT_ENUMERATE_TYPE_F, _, list) BOOST_PP_NIL ))

#define $RT_ENUMERATE_I(count, ...)      BOOST_PP_IF(count, $RT_ENUMERATE_II(count), ())
#define $RT_ENUMERATE_TYPE_I(count, ...) BOOST_PP_IF(count, $RT_ENUMERATE_TYPE_II($TO_LIST(__VA_ARGS__)), ())

#define $RT_ENUMERATE(count)            BOOST_PP_REMOVE_PARENS( $RT_ENUMERATE_I(count) )
#define $RT_ENUMERATE_TYPE(count, ...)  BOOST_PP_REMOVE_PARENS( $RT_ENUMERATE_TYPE_I(count, __VA_ARGS__) )


#define $REGISTER_TRAMPOLINE(name, type)                                                                                                    \
$GET_RET(type) $TRICAT($, __LINE__, $TRICAT(_, name, _trampoline))                                                                          \
($RT_ENUMERATE_TYPE( $ARG_COUNT(type), $GET_ARGS(type) ))                                                                                   \
{ void* tmp_thunk_this = thunk_this; thunk_this = NULL; return name( ($GET_BASE(type))tmp_thunk_this $TVA_COMMA(type) $RT_ENUMERATE($ARG_COUNT(type)) ); }

// tuple trampoline expansion
#define $RAT_F(r, d, type) $REGISTER_TRAMPOLINE($EXTRACT_NAME(type), $EXTRACT_TYPE(type))

#define $REGISTER_ALL_TRAMPOLINES_I(seq) BOOST_PP_SEQ_FOR_EACH($RAT_F,, seq)
#define $REGISTER_ALL_TRAMPOLINES(...) $REGISTER_ALL_TRAMPOLINES_I( $TO_SEQ(__VA_ARGS__) )


// WRAPPER:
// struct params
#define $GEN_DEF(r, d, fn) BOOST_PP_SEQ_HEAD(fn)(*BOOST_PP_SEQ_ELEM(1, fn))( $GET_ARGS($EXTRACT_TYPE(fn)) );

#define $CREATE_ALL_DEFS_I(seq) BOOST_PP_SEQ_FOR_EACH($GEN_DEF,, seq)
#define $CREATE_ALL_DEFS(...) $CREATE_ALL_DEFS_I( $TO_SEQ(__VA_ARGS__) )

// struct assignments
#define $GEN_NAME_I(name) $TRICAT($, __LINE__, $TRICAT(_, name, _trampoline))
#define $GEN_NAME(r, d, fn) $GEN_NAME_I( $EXTRACT_NAME(fn) ),

#define $GEN_ALL_NAMES_I(seq) BOOST_PP_SEQ_FOR_EACH($GEN_NAME,, seq)
#define $GEN_ALL_NAMES(...) $GEN_ALL_NAMES_I( $TO_SEQ(__VA_ARGS__) )

// struct combo
#define $GEN_WRAPPER(type, ...)                                             \
struct $TRICAT($, type, _registry) { $CREATE_ALL_DEFS(__VA_ARGS__) }        \
$TRICAT($, type, _registry_instance) = { $GEN_ALL_NAMES(__VA_ARGS__) };


// INTERFACE
#define $register(type, ...) $REGISTER_ALL_TRAMPOLINES(__VA_ARGS__) $GEN_WRAPPER(type, __VA_ARGS__)
#define $def(type, name) struct $TRICAT($, type, _registry)* $CAT(name, _local_) = &$TRICAT($, type, _registry_instance); type name
#define $(var) thunk_this = &var; $CAT(var, _local_)
void* thunk_this = { 0 };


#endif
