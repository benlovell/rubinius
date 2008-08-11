
#include "prelude.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/array.hpp"
#include "builtin/string.hpp"

namespace rubinius {

  /* WARNING. Do not use this version if +num+ has the chance of being
   * greater than FIXNUM_MAX or less than FIXNUM_MIN. */
  FIXNUM Fixnum::from(native_int num) {
    return (FIXNUM)APPLY_TAG(num, TAG_FIXNUM);
  }

  INTEGER Fixnum::add(STATE, FIXNUM other) {
    native_int r = to_nint() + other->to_nint();
    if(r > FIXNUM_MAX || r < FIXNUM_MIN) {
      return Bignum::create(state, r);
    } else {
      return Fixnum::from(r);
    }
  }

  INTEGER Fixnum::add(STATE, Bignum* other) {
    return other->add(state, this);
  }

  Float* Fixnum::add(STATE, Float* other) {
    return other->add(state, this);
  }

  INTEGER Fixnum::sub(STATE, FIXNUM other) {
    native_int r = n2i() - other->n2i();
    if(r > FIXNUM_MAX || r < FIXNUM_MIN) {
      return Bignum::create(state, r);
    } else {
      return Fixnum::from(r);
    }
  }
    
  INTEGER Fixnum::sub(STATE, Bignum* other) {
    return as<Bignum>(other->neg(state))->add(state, this);
  }

  Float* Fixnum::sub(STATE, Float* other) {
    return Float::coerce(state, this)->sub(state, other);
  }

  INTEGER Fixnum::mul(STATE, FIXNUM other) {
    native_int a  = n2i();
    native_int b  = other->n2i();
      
    if(a == 0 || b == 0) return Fixnum::from(0);
      
    if(a > 0) {
      if(b > 0) {
        if(a > (FIXNUM_MAX / b)) {
          return Bignum::create(state, a)->mul(state, other);
        } 
      } else {
        if (b < (FIXNUM_MIN / a)) {
          return Bignum::create(state, a)->mul(state, other);
        }
      }
    } else {
      if(b > 0){
        if(a < (FIXNUM_MIN / b)) {
          return Bignum::create(state, a)->mul(state, other);
        }
      } else {
        if(b < (FIXNUM_MAX / a)) {
          return Bignum::create(state, a)->mul(state, other);
        }
      }
    }
      
    return Fixnum::from(n2i() * other->n2i());
  }

  INTEGER Fixnum::mul(STATE, Bignum* other) {
    return other->mul(state, this);
  }

  Float* Fixnum::mul(STATE, Float* other) {
    return other->mul(state, this);
  }

  INTEGER Fixnum::div(STATE, FIXNUM other) {
    if(other->n2i() == 0) {
      throw ZeroDivisionError(other, "divided by 0");
    }
    native_int numerator = n2i();
    native_int denominator = other->n2i();
    native_int quotient = numerator / denominator;
    if(quotient < 0 && quotient * denominator != numerator) --quotient;
    return Fixnum::from(quotient);
  }

  INTEGER Fixnum::div(STATE, Bignum* other) {
    return Bignum::create(state, n2i())->div(state, other);
  }

  Float* Fixnum::div(STATE, Float* other) {
    return Float::coerce(state, this)->div(state, other);
  }

  INTEGER Fixnum::mod(STATE, FIXNUM other) {
    native_int numerator = n2i();
    native_int denominator = other->n2i();
    native_int quotient = div(state, other)->n2i();
    return Fixnum::from(numerator - denominator * quotient);
  }

  INTEGER Fixnum::mod(STATE, Bignum* other) {
    return Bignum::create(state, n2i())->mod(state, other);
  }

  Float* Fixnum::mod(STATE, Float* other) {
    return Float::create(state, n2i())->mod(state, other);
  }

  Array* Fixnum::divmod(STATE, FIXNUM other) {
    if(other->n2i() == 0) {
      throw ZeroDivisionError(other, "divided by 0");
    }
    native_int numerator = n2i();
    native_int denominator = other->n2i();
    native_int fraction = div(state, other)->n2i();
    Array* ary = Array::create(state, 2);
    ary->set(state, 0, Fixnum::from(fraction));
    ary->set(state, 1, Fixnum::from(numerator - denominator * fraction));
    return ary;
  }

  Array* Fixnum::divmod(STATE, Bignum* other) {
    return Bignum::create(state, n2i())->divmod(state, other);
  }

  Array* Fixnum::divmod(STATE, Float* other) {
    return Float::create(state, n2i())->divmod(state, other);
  }

  INTEGER Fixnum::neg(STATE) {
    return Fixnum::from(-n2i());
  }

  OBJECT Fixnum::equal(STATE, FIXNUM other) {
    return n2i() == other->n2i() ? Qtrue : Qfalse;
  }

  OBJECT Fixnum::equal(STATE, Bignum* other) {
    return other->equal(state, this);
  }

  OBJECT Fixnum::equal(STATE, Float* other) {
    return (double)n2i() == other->val ? Qtrue : Qfalse;
  }

  FIXNUM Fixnum::compare(STATE, FIXNUM other) {
    native_int left  = n2i();
    native_int right = other->n2i();
    if(left == right) {
      return Fixnum::from(0);
    } else if(left < right) {
      return Fixnum::from(-1);
    } else {
      return Fixnum::from(1);
    }
  }

  FIXNUM Fixnum::compare(STATE, Bignum* other) {
    native_int res = other->compare(state, this)->n2i();
    if(res == 0) {
      return Fixnum::from(0);
    } else if(res < 0) {
      return Fixnum::from(1);
    } else {
      return Fixnum::from(-1);
    }
  }

  FIXNUM Fixnum::compare(STATE, Float* other) {
    double left  = (double)n2i();
    double right = other->val;
    if(left == right) {
      return Fixnum::from(0);
    } else if(left < right) {
      return Fixnum::from(-1);
    } else {
      return Fixnum::from(1);
    }
  }

  OBJECT Fixnum::gt(STATE, FIXNUM other) {
    return n2i() > other->n2i() ? Qtrue : Qfalse;
  }

  OBJECT Fixnum::gt(STATE, Bignum* other) {
    return other->lt(state, this);
  }

  OBJECT Fixnum::gt(STATE, Float* other) {
    return (double) n2i() > other->val ? Qtrue : Qfalse;
  }

  OBJECT Fixnum::ge(STATE, FIXNUM other) {
    return n2i() >= other->n2i() ? Qtrue : Qfalse;      
  }

  OBJECT Fixnum::ge(STATE, Bignum* other) {
    return other->le(state, this);
  }

  OBJECT Fixnum::ge(STATE, Float* other) {
    return (double) n2i() >= other->val ? Qtrue : Qfalse;
  }

  OBJECT Fixnum::lt(STATE, FIXNUM other) {
    return n2i() < other->n2i() ? Qtrue : Qfalse;      
  }

  OBJECT Fixnum::lt(STATE, Bignum* other) {
    return other->gt(state, this);
  }

  OBJECT Fixnum::lt(STATE, Float* other) {
    return (double) n2i() < other->val ? Qtrue : Qfalse;
  }

  OBJECT Fixnum::le(STATE, FIXNUM other) {
    return n2i() <= other->n2i() ? Qtrue : Qfalse;      
  }

  OBJECT Fixnum::le(STATE, Bignum* other) {
    return other->ge(state, this);
  }

  OBJECT Fixnum::le(STATE, Float* other) {
    return (double) n2i() <= other->val ? Qtrue : Qfalse;
  }

  INTEGER Fixnum::left_shift(STATE, INTEGER bits) {
    native_int shift = bits->n2i();
    if(shift < 0) {
      return right_shift(state, Fixnum::from(-shift));
    }
      
    native_int self = n2i();
      
    if(shift > (native_int)FIXNUM_WIDTH || self >> ((native_int)FIXNUM_WIDTH - shift) > 0) {
      return Bignum::create(state, self)->left_shift(state, bits);
    }
      
    return Fixnum::from(self << shift);
  }

  INTEGER Fixnum::right_shift(STATE, INTEGER bits) {
    native_int shift = bits->n2i();
    if(shift < 0) {
      return left_shift(state, Fixnum::from(-shift));
    }
      
    native_int self = n2i();
      
    native_int a = self >> shift;
    return Fixnum::from(a); 
  }

  native_int Fixnum::to_nint() {
    return STRIP_TAG(this);
  }

  INTEGER Fixnum::size(STATE) {
    return Fixnum::from(sizeof(native_int));
  }

  INTEGER Fixnum::bit_and(STATE, FIXNUM other) {
    return Fixnum::from(n2i() & other->n2i());
  }

  INTEGER Fixnum::bit_and(STATE, Bignum* other) {
    return other->bit_and(state, this);
  }

  INTEGER Fixnum::bit_and(STATE, Float* other) {
    return Fixnum::from(n2i() & (native_int)other->val);
  }

  INTEGER Fixnum::bit_or(STATE, FIXNUM other) {
    return Fixnum::from(n2i() | other->n2i());
  }

  INTEGER Fixnum::bit_or(STATE, Bignum* other) {
    return other->bit_or(state, this);
  }

  INTEGER Fixnum::bit_or(STATE, Float* other) {
    return Fixnum::from(n2i() | (native_int)other->val);
  }

  INTEGER Fixnum::bit_xor(STATE, FIXNUM other) {
    return Fixnum::from(n2i() ^ other->n2i());
  }

  INTEGER Fixnum::bit_xor(STATE, Bignum* other) {
    return other->bit_xor(state, this);
  }

  INTEGER Fixnum::bit_xor(STATE, Float* other) {
    return Fixnum::from(n2i() ^ (native_int)other->val);
  }

  INTEGER Fixnum::invert(STATE) {
    return Fixnum::from(~n2i());
  }

  Float* Fixnum::to_f(STATE) {
    return Float::create(state, (double)n2i());
  }

  String* Fixnum::to_s(STATE) {
    std::stringstream sout;
    sout << n2i();
    return String::create(state, sout.str().c_str());
  }
}
