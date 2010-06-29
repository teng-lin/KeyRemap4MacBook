#ifndef IGNOREMULTIPLESAMEKEYPRESS_HPP
#define IGNOREMULTIPLESAMEKEYPRESS_HPP

#include "RemapFuncBase.hpp"

namespace org_pqrs_KeyRemap4MacBook {
  namespace RemapFunc {
    // ex. Ignore JIS_KANA x 2. (validate only the first once)
    class IgnoreMultipleSameKeyPress {
    public:
      IgnoreMultipleSameKeyPress(void) {};
      bool remap(RemapParams& remapParams);
      void initialize(void);
      void terminate(void);

      // ----------------------------------------
      // [0] => fromKey_
      void add(KeyCode newval);
      void add(Flags newval);

    private:
      PairKeyFlags fromKey_;
      KeyCode lastkeycode_;
    };
  }
}

#endif