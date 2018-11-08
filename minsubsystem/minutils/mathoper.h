/*

Copyright (c) 2011-2017, Smart Engines Limited. All rights reserved.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of copyright holders.

*/

/**
 * @file   mathoper.h
 * @brief  Definition of mathematical operations.
 */
#pragma once
#ifndef MINUTILS_MATHOPER_H_INCLUDED
#define MINUTILS_MATHOPER_H_INCLUDED

/**
 * @defgroup MinUtils_MathOper Mathematical operations
 * @brief    The module specifies mathematical operations which can be used in
 *           image processing functions. All operations are specified by the
 *           follow constants: @c #OP_MIN (binary minimum), @c #OP_MAX (binary
 *           maximum), @c #OP_ADD (binary addition), @c #OP_DIF (binary difference),
 *           @c #OP_ADF (binary absolute difference), @c #OP_MUL (binary
 *           multiplication), @c #OP_AVE (binary average), and @c #OP_EUC (binary
 *           Euclidean norm). Additionally, the operations are grouped into several
 *           enums in accordance with their properties: all binary operations
 *           (@c #BiOp), associative operations (@c #AsOp), commutative operations
 *           (@c #CoOp), associative-commutative operations (@c #AsCoOp) and
 *           idempotent operations (@c #IdOp).
 */

#include <minbase/macro_helpers.h>

/**
 * @brief   Specifies mathematical operations.
 * @details The enum specifies mathematical operations.
 * @ingroup MinUtils_MathOper
 */
enum MathOp {
  /**
  * @brief   Specifies binary minimum operation.
  * @details The constant specifies the binary minimum operation @f$ f @f$ that
  *          is defined as follows: @f[ f(x, y) = \min(x, y) @f]
  */
  OP_MIN = 1,

  /**
  * @brief   Specifies binary maximum operation.
  * @details The constant specifies the binary maximum operation @f$ f @f$ that
  *          is defined as follows: @f[ f(x, y) = \max(x, y) @f]
  */
  OP_MAX,

  /**
  * @brief   Specifies binary addition operation.
  * @details The constant specifies the binary addition operation @f$ f @f$ that
  *          is defined as follows: @f[ f(x, y) = x + y @f]
  */
  OP_ADD,

  /**
  * @brief   Specifies binary difference operation.
  * @details The constant specifies the binary difference operation @f$ f @f$
  *          that is defined as follows: @f[ f(x, y) = x - y @f]
  */
  OP_DIF,

  /**
  * @brief   Specifies binary absolute difference operation.
  * @details The constant specifies the binary absolute difference operation
  *          @f$ f @f$ that is defined as follows: @f[ f(x, y) = \mid x - y \mid @f]
  */
  OP_ADF,

  /**
  * @brief   Specifies binary multiplication operation.
  * @details The constant specifies the binary multiplication operation @f$ f @f$
  *          that is defined as follows: @f[ f(x, y) = x \cdot y @f]
  */
  OP_MUL,

  /**
  * @brief   Specifies binary average operation.
  * @details The constant specifies the binary average operation @f$ f @f$ that
  *          is defined as follows: @f[ f(x, y) = \frac{x + y}{2} @f]
  */
  OP_AVE,

  /**
  * @brief   Specifies binary Euclidean norm operation.
  * @details The constant specifies the binary Euclidean norm operation @f$ f @f$
  *          that is defined as follows: @f[ f(x, y) = \sqrt{x^2 + y^2} @f]
  */
  OP_EUC,

  /**
  * @brief   Specifies binary division operation.
  * @details The constant specifies the binary multiplication operation @f$ f @f$
  *          that is defined as follows: @f[ f(x, y) = x / y @f]
  */
  OP_DIV,

  /**
  * @brief   Specifies binary sum of squares operation.
  * @details The constant specifies the binary sum of squares operation @f$ f @f$
  *          that is defined as follows: @f[ f(x, y) = x^2 + y^2 @f]
  */
  OP_SSQ,

  /**
  * @brief   Specifies unary negation operation.
  * @details The constant specifies the unary negation operation @f$ f @f$
  *          that is defined as follows: @f[ f(x) = -x @f].
  */
  OP_NEG,

  /**
  * @brief   Specifies unary absolute value operation.
  * @details The constant specifies the unary absolute value operation @f$ f @f$
  *          that is defined as follows: @f[ f(x) = \left\{ \begin{array} {rl} x &
  *          x >= 0 \cr -x & otherwise  \end{array} \right. @f]
  */
  OP_ABS,

  /**
  * @brief   Specifies unary square root operation.
  * @details The constant specifies the unary square root operation @f$ f @f$
  *          that is defined as follows: @f[ f(x) = \sqrt{x} @f]
  */
  OP_SQRT,

  /**
  * @brief   Specifies unary reciprocal operation.
  * @details The constant specifies the unary reciprocal operation @f$ f @f$
  *          that is defined as follows: @f[ f(x) = 1 / x @f]
  */
  OP_RCPR,

  /**
  * @brief   Specifies binary power operation.
  * @details The constant specifies the binary power operation @f$ f @f$
  *          that is defined as follows: @f[ f(x, y) = x^y @f]
  */
  OP_POW,

  /**
  * @brief   Specifies unary inversion operation.
  * @details The constant specifies the unary inversion operation @f$ f @f$
  *          that is defined as follows: @f[ f(x) = \sim x @f],
  *          where ~ is bitwise not.
  * @remarks Deprecated. Use OP_NOT instead.
  */
  OP_INV,

  /**
  * @brief   Specifies unary NOT operation.
  * @details The constant specifies the unary NOT operation @f$ f @f$
  *          that is defined as follows: @f[ f(x) = \sim x @f],
  *          where ~ is bitwise not.
  */
  OP_NOT,

  /**
  * @brief   Specifies binary AND operation.
  * @details The constant specifies the binary AND operation @f$ f @f$
  *          that is defined as follows: @f[ f(x, y) = x \& y @f]
  */
  OP_AND,

  /**
  * @brief   Specifies binary OR operation.
  * @details The constant specifies the binary OR operation @f$ f @f$
  *          that is defined as follows: @f[ f(x, y) = x | y @f]
  */
  OP_OR,

  /**
  * @brief   Specifies binary OR operation.
  * @details The constant specifies the binary OR operation @f$ f @f$
  *          that is defined as follows: @f[ f(x, y) = x \oplus y @f]
  */
  OP_XOR,

  /**
  * @brief   Specifies unary logarithm operation.
  * @details The constant specifies the unary logarithm operation @f$ f @f$
  *          that is defined as follows: @f[ f(x) = \log{x} @f]
  */
  OP_LOG
};

/**
 * @brief   Specifies unary operations.
 * @details The enum specifies unary operations, that is such ones which involve
 *          one operand. Formally, a unary operation @f$ f @f$ on a set @f$ S @f$
 *          maps elements of @f$ S @f$ to @f$ S @f$:
 *          @f[ f: S \to S @f]
 * @ingroup MinUtils_MathOper
 */
enum UnOp {
  UNOP_NEG  = OP_NEG,   ///< Unary operation for value negation.
  UNOP_ABS  = OP_ABS,   ///< Unary operation for computing absolute value.
  UNOP_SQRT = OP_SQRT,  ///< Unary operation for computing square root.
  UNOP_RCPR = OP_RCPR,  ///< Unary operation for computing reciprocal value.
  UNOP_INV  = OP_INV,   ///< Unary operation for value inversion.
  UNOP_NOT  = OP_NOT,   ///< Unary operation for bitwise NOT.
  UNOP_LOG  = OP_LOG    ///< Unary operation for computing logarithm.
};

/**
 * @brief   Specifies binary operations.
 * @details The enum specifies binary operations, that is such ones which involve
 *          two operands. Formally, a binary operation @f$ f @f$ on a set @f$ S @f$
 *          is a binary relation that maps elements of the Cartesian product
 *          @f$ S \times S @f$ to @f$ S @f$:
 *          @f[ f: S \times S \to S @f]
 * @ingroup MinUtils_MathOper
 */
enum BiOp {
  BIOP_MIN = OP_MIN,  ///< Binary minimum operation (see @c #OP_MIN).
  BIOP_MAX = OP_MAX,  ///< Binary maximum operation (see @c #OP_MAX).
  BIOP_ADD = OP_ADD,  ///< Binary addition operation (see @c #OP_ADD).
  BIOP_DIF = OP_DIF,  ///< Binary difference operation (see @c #OP_DIF).
  BIOP_ADF = OP_ADF,  ///< Binary absolute difference operation (see @c #OP_ADF).
  BIOP_MUL = OP_MUL,  ///< Binary multiplication operation (see @c #OP_MUL).
  BIOP_AVE = OP_AVE,  ///< Binary average operation (see @c #OP_AVE).
  BIOP_EUC = OP_EUC,  ///< Binary Euclidean norm operation (see @c #OP_EUC).
  BIOP_DIV = OP_DIV,  ///< Binary division operation (see @c #OP_DIV).
  BIOP_SSQ = OP_SSQ,  ///< Binary sum of squares operation (see @c #OP_SSQ).
  BIOP_POW = OP_POW,  ///< Binary power operation (see @c #OP_POW).
  BIOP_AND = OP_AND,  ///< Binary and operation (see @c #OP_AND).
  BIOP_OR  = OP_OR,   ///< Binary or operation (see @c #OP_OR).
  BIOP_XOR = OP_XOR,  ///< Binary xor operation (see @c #OP_XOR).
};

/**
 * @brief   Specifies associative operations.
 * @details The enum specifies associative operations that is such ones which
 *          can be freely regrouped without altering result. Formally, a binary
 *          operation @f$ f @f$ on a set @f$ S @f$ is called associative if it
 *          satisfies the associative law:
 *          @f[ f(f(x, y), z) = f(x, f(y, z)) \quad \forall x,y,z \in S @f]
 * @ingroup MinUtils_MathOper
 */
enum AsOp {
  ASOP_MIN = OP_MIN,  ///< Binary minimum operation (see @c #OP_MIN).
  ASOP_MAX = OP_MAX,  ///< Binary maximum operation (see @c #OP_MAX).
  ASOP_ADD = OP_ADD,  ///< Binary addition operation (see @c #OP_ADD).
  ASOP_MUL = OP_MUL,  ///< Binary multiplication operation (see @c #OP_MUL).
  ASOP_EUC = OP_EUC,  ///< Binary Euclidean norm operation (see @c #OP_EUC).
  ASOP_AND = OP_AND,  ///< Binary and operation (see @c #OP_AND).
  ASOP_OR  = OP_OR,   ///< Binary or operation (see @c #OP_OR).
  ASOP_XOR = OP_XOR,  ///< Binary xor operation (see @c #OP_XOR).
};

/**
 * @brief   Specifies  commutative operations.
 * @details The enum specifies commutative operations, that is such ones which
 *          do not depend on the order of the input parameters. Formally, a binary
 *          operation @f$ f @f$ on a set @f$ S @f$ is called commutative if it
 *          satisfies the commutative law:
 *          @f[ f(x, y) = f(y, x) \quad \forall x,y \in S @f]
 * @ingroup MinUtils_MathOper
 */
enum CoOp {
  COOP_MIN = OP_MIN,  ///< Binary minimum operation (see @c #OP_MIN).
  COOP_MAX = OP_MAX,  ///< Binary maximum operation (see @c #OP_MAX).
  COOP_ADD = OP_ADD,  ///< Binary addition operation (see @c #OP_ADD).
  COOP_ADF = OP_ADF,  ///< Binary absolute difference operation (see @c #OP_ADF).
  COOP_MUL = OP_MUL,  ///< Binary multiplication operation (see @c #OP_MUL).
  COOP_AVE = OP_AVE,  ///< Binary average operation (see @c #OP_AVE).
  COOP_EUC = OP_EUC,  ///< Binary Euclidean norm operation (see @c #OP_EUC).
  COOP_SSQ = OP_SSQ,  ///< Binary sum of squares operation (see @c #OP_SSQ).
  COOP_AND = OP_AND,  ///< Binary and operation (see @c #OP_AND).
  COOP_OR  = OP_OR,   ///< Binary or operation (see @c #OP_OR).
  COOP_XOR = OP_XOR,  ///< Binary xor operation (see @c #OP_XOR).
};

/**
 * @brief   Specifies associative-commutative operations.
 * @details The enum specifies associative-commutative operations, that is such
 *          ones which have both associative and commutative properties. Formally,
 *          a binary operation @f$ \circ @f$ on a set @f$ S @f$ is called
 *          associative-commutative if it satisfies both the associative and the
 *          commutative laws:
 *          @f[ f(f(x, y), z) = f(x, f(y, z)) \quad \forall x,y,z \in S @f]
 *          @f[ f(x, y) = f(y, x) \quad \forall x,y \in S @f]
 * @ingroup MinUtils_MathOper
 */
enum AsCoOp {
  ASCOOP_MIN = OP_MIN,  ///< Binary minimum operation (see @c #OP_MIN).
  ASCOOP_MAX = OP_MAX,  ///< Binary maximum operation (see @c #OP_MAX).
  ASCOOP_ADD = OP_ADD,  ///< Binary addition operation (see @c #OP_ADD).
  ASCOOP_MUL = OP_MUL,  ///< Binary multiplication operation (see @c #OP_MUL).
  ASCOOP_EUC = OP_EUC,  ///< Binary Euclidean norm operation (see @c #OP_EUC).
  ASCOOP_AND = OP_AND,  ///< Binary and operation (see @c #OP_AND).
  ASCOOP_OR  = OP_OR,   ///< Binary or operation (see @c #OP_OR).
  ASCOOP_XOR = OP_XOR,  ///< Binary xor operation (see @c #OP_XOR).
};

/**
 * @brief   Specifies idempotent operations.
 * @details The enum specifies idempotent operations, that is such ones which
 *          can be applied multiple times without changing the result. Formally,
 *          a binary operation @f$ f @f$ on a set @f$ S @f$ is called idempotent
 *          if @f[ f(x, f(x, y)) = f(x, y) \quad \forall x, y \in S @f]
 * @ingroup MinUtils_MathOper
 */
enum IdOp {
  IDOP_MIN = OP_MIN,  ///< Binary minimum operation (see @c #OP_MIN).
  IDOP_MAX = OP_MAX,  ///< Binary maximum operation (see @c #OP_MAX).
  IDOP_AND = OP_AND,  ///< Binary and operation (see @c #OP_AND).
  IDOP_OR  = OP_OR,   ///< Binary or operation (see @c #OP_OR).
};

DECLARE_COMPOUND_WITH_TYPEDEF(enum, MathOp);
DECLARE_COMPOUND_WITH_TYPEDEF(enum, UnOp);
DECLARE_COMPOUND_WITH_TYPEDEF(enum, BiOp);
DECLARE_COMPOUND_WITH_TYPEDEF(enum, AsOp);
DECLARE_COMPOUND_WITH_TYPEDEF(enum, CoOp);
DECLARE_COMPOUND_WITH_TYPEDEF(enum, AsCoOp);
DECLARE_COMPOUND_WITH_TYPEDEF(enum, IdOp);

#endif // #ifndef MINUTILS_MATHOPER_H_INCLUDED
