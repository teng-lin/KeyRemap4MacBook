#include "PointingButtonToKey.hpp"

namespace org_pqrs_KeyRemap4MacBook {
  namespace RemapFunc {
    void
    PointingButtonToKey::initialize(void)
    {
      keytokey_.initialize();
    }

    void
    PointingButtonToKey::terminate(void)
    {
      keytokey_.terminate();
    }

    void
    PointingButtonToKey::add(PointingButton newval)
    {
      switch (index_) {
        case 0:
          fromButton_.button = newval;
          break;
        default:
          IOLOG_ERROR("Invalid PointingButtonToKey::add\n");
          break;
      }
      ++index_;
    }

    void
    PointingButtonToKey::add(KeyCode newval)
    {
      switch (index_) {
        case 0:
          IOLOG_ERROR("Invalid PointingButtonToKey::add\n");
          break;

        case 1:
          // pass-through (== no break)
          keytokey_.add(KeyCode::VK_PSEUDO_KEY);
          keytokey_.add(fromButton_.flags);
        default:
          keytokey_.add(newval);
          break;
      }
      ++index_;
    }

    void
    PointingButtonToKey::add(Flags newval)
    {
      switch (index_) {
        case 0:
          IOLOG_ERROR("Invalid PointingButtonToKey::add\n");
          break;
        case 1:
          fromButton_.flags = newval;
          break;
        default:
          keytokey_.add(newval);
          break;
      }
    }

    bool
    PointingButtonToKey::remap(RemapPointingParams_relative& remapParams)
    {
      if (remapParams.isremapped) return false;
      if (! fromkeychecker_.isFromPointingButton(remapParams.params, fromButton_.button, fromButton_.flags)) return false;
      remapParams.isremapped = true;

      bool isButtonDown = remapParams.params.ex_justPressed.isOn(fromButton_.button);
      if (isButtonDown) {
        ButtonStatus::decrease(fromButton_.button);
      } else {
        ButtonStatus::increase(fromButton_.button);
      }

      return keytokey_.call_remap_with_VK_PSEUDO_KEY(isButtonDown ? EventType::DOWN : EventType::UP);
    }
  }
}