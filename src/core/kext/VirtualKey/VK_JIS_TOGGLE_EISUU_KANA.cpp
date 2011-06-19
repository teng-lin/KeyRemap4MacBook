#include "base.hpp"
#include "Config.hpp"
#include "EventOutputQueue.hpp"
#include "FlagStatus.hpp"
#include "VK_JIS_IM_CHANGE.hpp"
#include "VK_JIS_TOGGLE_EISUU_KANA.hpp"

namespace org_pqrs_KeyRemap4MacBook {
  KeyCode VirtualKey::VK_JIS_TOGGLE_EISUU_KANA::newkeycode_;

  bool
  VirtualKey::VK_JIS_TOGGLE_EISUU_KANA::handle(const Params_KeyboardEventCallBack& params)
  {
    if (params.key != KeyCode::VK_JIS_TOGGLE_EISUU_KANA) return false;

    int ignore_improveIM = Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_remap_jis_ignore_improvement_IM_changing);

    if (params.ex_iskeydown) {
      if (ignore_improveIM && InputMode::JAPANESE == CommonData::getcurrent_workspacedata().inputmode ||
          ! ignore_improveIM && InputMode::JAPANESE == VirtualKey::VK_JIS_IM_CHANGE::getwsd_public().inputmode) {
        newkeycode_ = KeyCode::JIS_EISUU;
      } else {
        newkeycode_ = KeyCode::JIS_KANA;
      }

      bool result00 = false;
      if (! ignore_improveIM) {
        result00 = VirtualKey::VK_JIS_IM_CHANGE::replace_WSD(newkeycode_, ModifierFlag::NONE);
      }
      if (result00) {
        VirtualKey::VK_JIS_IM_CHANGE::set_omit_initialize(true);
      } else {
        VirtualKey::VK_JIS_IM_CHANGE::set_omit_initialize(false);
      }
    }

    Params_KeyboardEventCallBack::auto_ptr ptr(Params_KeyboardEventCallBack::alloc(params.eventType, params.flags, newkeycode_,
                                                                                   params.keyboardType, params.repeat));
    if (ptr) {
      EventOutputQueue::FireKey::fire(*ptr);
    }
    return true;
  }
}
