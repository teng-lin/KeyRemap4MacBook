#ifndef VIRTUALKEY_VK_JIS_IM_CHANGE_HPP
#define VIRTUALKEY_VK_JIS_IM_CHANGE_HPP

#include "TimerWrapper.hpp"

namespace org_pqrs_KeyRemap4MacBook {
  namespace VirtualKey {
    class VK_JIS_IM_CHANGE {
    public:
      class SeesawType {
      public:
        enum Value {
          NONE,
          CUR_PRE,
          EISUU_KANA,
          KANA_OTHERS,
          KANA_EISUU,
          EISUU_OTHERS,
        };
      };

      class SkipType {
      public:
        enum Value {
          NONE,
          NONE_FORWARD,
          NONE_BACK,
          PRE_FORWARD,
          PRE_BACK,
          EISUU_KANA,
          KANA,
          EISUU,
        };
      };

      // This enum name may not be suitable.
      // I named it temporary.
      enum SavedInputModeIndex {
        // XXX: change wsdNONE to 0.
        wsdNONE  = -1,
        // XXX: change all values to auto-indexed.
        wsdEISU  = 1,
        wsdHIRA  = 2,
        wsdKATA  = 3,
        wsdHKAT  = 4,
        wsdAINU  = 5,
        wsdFEIS  = 6,
        wsdMAX   = 6,
      };

      class CallbackType {
      public:
        enum Value {
          INIT,
          RESTORE,
          SEESAW_INIT,
        };
      };

      static void initialize(IOWorkLoop& workloop);
      static void terminate(void);

      static void scheduleCallback(CallbackType::Value callbacktype);
      static void cancelCallback(void);

      static void set_omit_initialize(bool omit_initialize) { omit_initialize_ = omit_initialize; }

      static void static_set_case1_pass_restore(int case1_pass_restore00);
      static int get_case1_pass_restore(void) { return case1_pass_restore2_; }

      static void ControlWorkspaceData(Params_KeyboardEventCallBack& params, int stage00);

      static void set_indexes_directly(SavedInputModeIndex new_pre, SavedInputModeIndex new_cur, SavedInputModeIndex new_others) {
        if (new_pre != wsdNONE) {
          pre_index2_  = new_pre;
        }
        if (new_cur != wsdNONE) {
          cur_index2_ = new_cur;
        }
        if (new_others != wsdNONE) {
          others_index2_ = new_others;
        }
      }

      static void set_new_index(SavedInputModeIndex index00) {
        if (cur_index2_ != wsdNONE && pre_index2_ != wsdNONE) {
          if (cur_index2_ != index00) {
            set_indexes_directly(cur_index2_, index00, wsdNONE);
          }
        } else if (cur_index2_ == wsdNONE) {
          if (pre_index2_ != index00) {
            set_indexes_directly(wsdNONE, index00, wsdNONE);
          }
        } else {
          set_indexes_directly(index00, wsdNONE, wsdNONE);
        }
      };

      static void init_seesaw(void) {
        seesaw_init2_ = true;
      };

      static SavedInputModeIndex get_index_for_seesaw_AtoB_WSD(SeesawType::Value type);
      // XXX: DO NOT PASS int[] without length!!!
      static SavedInputModeIndex get_index_for_replaceWSD(int sign00, int skip[], int replace_num00);

      enum {
        POST_REMAP       = 0,
        JUST_AFTER_REMAP = 1,
        NON_REMAPPED     = 2,
      };

      static void reverse_sign_CHANGE_SKIP(int when00);
      static const BridgeWorkSpaceData& getwsd_public(void) { return wsd_public_; }

      enum ControlWorkspaceDataType {
        CONTROL_WORKSPACEDATA_UPDATE,
        CONTROL_WORKSPACEDATA_LEARN,
        CONTROL_WORKSPACEDATA_REPLACE,
        CONTROL_WORKSPACEDATA_RESTORE,
      };

      static bool control_WSD(ControlWorkspaceDataType type, KeyCode modekey00, Flags flag00, InputModeDetail IMDsaved00);
      static bool update_WSD(void) {
        return control_WSD(CONTROL_WORKSPACEDATA_UPDATE, KeyCode::VK_NONE, ModifierFlag::NONE, InputModeDetail::UNKNOWN);
      };
      static bool learn_WSD(void) {
        return control_WSD(CONTROL_WORKSPACEDATA_LEARN, KeyCode::VK_NONE, ModifierFlag::NONE, InputModeDetail::UNKNOWN);
      };
      static bool replace_WSD(KeyCode modekey00, Flags flag00) {
        return control_WSD(CONTROL_WORKSPACEDATA_REPLACE, modekey00, flag00, InputModeDetail::UNKNOWN);
      };
      static bool restore_WSD(InputModeDetail IMDsaved00) {
        return control_WSD(CONTROL_WORKSPACEDATA_RESTORE, KeyCode::VK_NONE, ModifierFlag::NONE, IMDsaved00);
      };

      static bool handle(const Params_KeyboardEventCallBack& params);

    private:
      static void restore_timer_callback(OSObject* owner, IOTimerEventSource* sender);

      static KeyCode newkeycode_;
      static Flags newflag_;
      static TimerWrapper restore_timer_;
      static CallbackType::Value callbacktype_;

      static bool omit_initialize_;
      static int case1_pass_restore2_;

      static SavedInputModeIndex pre_index2_;
      static SavedInputModeIndex cur_index2_;
      static SavedInputModeIndex others_index2_;

      static int sign_plus_minus2_;
      static int counter_plus_minus2_;
      static int pre_counter_plus_minus2_;
      static bool seesaw_init2_;

      // XXX remove this value (replace by CommonData::current_workspacedata_)
      static BridgeWorkSpaceData wsd_public_;
      static BridgeWorkSpaceData wsd_save_[wsdMAX + 1];
      // XXX this value name may be wrong. (not wsd_learned_ but "bool initialized"?)
      static BridgeWorkSpaceData wsd_learned_;

      // XXX change function name
      static SavedInputModeIndex IMD2index(InputModeDetail inputmodedetail);
      static SavedInputModeIndex modeKey2index(KeyCode key, Flags flags);
    };
  }
}

#endif
