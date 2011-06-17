#include "base.hpp"
#include "Config.hpp"
#include "EventOutputQueue.hpp"
#include "FlagStatus.hpp"
#include "VirtualKey.hpp"
#include "VK_JIS_IM_CHANGE.hpp"

namespace org_pqrs_KeyRemap4MacBook {
  void
  VirtualKey::VK_JIS_IM_CHANGE::initialize(IOWorkLoop& workloop)
  {
    restore_timer_.initialize(&workloop, NULL, timeoutAfterKey2);
  }

  void
  VirtualKey::VK_JIS_IM_CHANGE::terminate(void)
  {
    restore_timer_.terminate();
  }

  void
  VirtualKey::VK_JIS_IM_CHANGE::cancelTimeout(void)
  {
    restore_timer_.cancelTimeout();
  }

  void
  VirtualKey::VK_JIS_IM_CHANGE::setTimeoutMS(int callback00)
  {
    callback2_  = callback00;
    // XXX check value
    restore_timer_.setTimeoutMS(Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_parameter_keyinterval_timeout));
  }

  void
  VirtualKey::VK_JIS_IM_CHANGE::timeoutAfterKey2(OSObject* owner, IOTimerEventSource* sender)
  {
    if (callback2_ == VirtualKey::VK_JIS_IM_CHANGE::CALLBACK_INIT) {
      pass_initialize2_ = VirtualKey::VK_JIS_IM_CHANGE::INIT_DO;

    } else if (callback2_ == VirtualKey::VK_JIS_IM_CHANGE::CALLBACK_RESTORE) {
      EventOutputQueue::FireKey::fire_downup(Flags(0), KeyCode::VK_JIS_TEMPORARY_RESTORE, CommonData::getcurrent_keyboardType());
      pass_initialize2_ = VirtualKey::VK_JIS_IM_CHANGE::INIT_NOT;

    } else if (callback2_ == VirtualKey::VK_JIS_IM_CHANGE::CALLBACK_SEESAW_INIT) {
      VirtualKey::VK_JIS_IM_CHANGE::init_seesaw();
    }
  }

  int
  VirtualKey::VK_JIS_IM_CHANGE::static_get_pass_initialize(void)
  {
    return pass_initialize2_;
  }

  void
  VirtualKey::VK_JIS_IM_CHANGE::static_set_pass_initialize(int pass_initialize00)
  {
    if (pass_initialize00 != VirtualKey::VK_JIS_IM_CHANGE::INIT_NOT && pass_initialize00 != VirtualKey::VK_JIS_IM_CHANGE::INIT_DO) return;
    pass_initialize2_ = pass_initialize00;
  }

  void
  VirtualKey::VK_JIS_IM_CHANGE::static_set_case1_pass_restore(int case1_pass_restore00)
  {
    int ignore_improveIM = Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_remap_jis_ignore_improvement_IM_changing);
    if (ignore_improveIM || case1_pass_restore00 != 0 && case1_pass_restore00 != 1) return;
    case1_pass_restore2_ = case1_pass_restore00;
  }

  bool
  VirtualKey::VK_JIS_IM_CHANGE::handle(const Params_KeyboardEventCallBack& params)
  {
    int ignore_improveIM = Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_remap_jis_ignore_improvement_IM_changing);
    int use_ainu = Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_general_use_ainu);
    KeyCode key00  = params.key;
    int index00;
    int skip00[VirtualKey::VK_JIS_IM_CHANGE::wsdMAX + 1] = { 0 };
    int replace_num00;
    int seesawType00 = -1;
    int skipType00   = -1;

    if (key00 == KeyCode::VK_JIS_IM_CHANGE_CUR_PRE ||
        key00 == KeyCode::VK_JIS_IM_CHANGE_SKIP_NONE_FORWARD  ||
        key00 == KeyCode::VK_JIS_IM_CHANGE_SKIP_NONE_BACK ||
        key00 == KeyCode::VK_JIS_IM_CHANGE_SKIP_PRE_FORWARD ||
        key00 == KeyCode::VK_JIS_IM_CHANGE_SKIP_PRE_BACK ||

        key00 == KeyCode::VK_JIS_IM_CHANGE_EISUU_KANA ||
        key00 == KeyCode::VK_JIS_IM_CHANGE_EISUU_OTHERS ||
        key00 == KeyCode::VK_JIS_IM_CHANGE_KANA_EISUU ||
        key00 == KeyCode::VK_JIS_IM_CHANGE_KANA_OTHERS ||
        key00 == KeyCode::VK_JIS_IM_CHANGE_SKIP_KANA_EISUU ||
        key00 == KeyCode::VK_JIS_IM_CHANGE_SKIP_EISUU ||
        key00 == KeyCode::VK_JIS_IM_CHANGE_SKIP_KANA) {} else {
      return false;
    }

    if (params.ex_iskeydown) {
      if (key00 == KeyCode::VK_JIS_IM_CHANGE_CUR_PRE) {
        seesawType00 = VirtualKey::VK_JIS_IM_CHANGE::CUR_PRE;
      } else if (key00 == KeyCode::VK_JIS_IM_CHANGE_EISUU_KANA) {
        seesawType00 = VirtualKey::VK_JIS_IM_CHANGE::EISUU_KANA;
      } else if (key00 == KeyCode::VK_JIS_IM_CHANGE_KANA_OTHERS) {
        seesawType00 = VirtualKey::VK_JIS_IM_CHANGE::KANA_OTHERS;
      } else if (key00 == KeyCode::VK_JIS_IM_CHANGE_KANA_EISUU) {
        seesawType00 = VirtualKey::VK_JIS_IM_CHANGE::KANA_EISUU;
      } else if (key00 == KeyCode::VK_JIS_IM_CHANGE_EISUU_OTHERS) {
        seesawType00 = VirtualKey::VK_JIS_IM_CHANGE::EISUU_OTHERS;
      } else if (key00 == KeyCode::VK_JIS_IM_CHANGE_SKIP_NONE_FORWARD) {
        skipType00 = VirtualKey::VK_JIS_IM_CHANGE::SKIP_NONE_FORWARD;
      } else if (key00 == KeyCode::VK_JIS_IM_CHANGE_SKIP_PRE_FORWARD) {
        skipType00 = VirtualKey::VK_JIS_IM_CHANGE::SKIP_PRE_FORWARD;
      } else if (key00 == KeyCode::VK_JIS_IM_CHANGE_SKIP_NONE_BACK) {
        skipType00 = VirtualKey::VK_JIS_IM_CHANGE::SKIP_NONE_BACK;
      } else if (key00 == KeyCode::VK_JIS_IM_CHANGE_SKIP_PRE_BACK) {
        skipType00 = VirtualKey::VK_JIS_IM_CHANGE::SKIP_PRE_BACK;
      } else if (key00 == KeyCode::VK_JIS_IM_CHANGE_SKIP_KANA_EISUU) {
        skipType00 = VirtualKey::VK_JIS_IM_CHANGE::SKIP_EISUU_KANA;
      } else if (key00 == KeyCode::VK_JIS_IM_CHANGE_SKIP_KANA) {
        skipType00 = VirtualKey::VK_JIS_IM_CHANGE::SKIP_KANA;
      } else if (key00 == KeyCode::VK_JIS_IM_CHANGE_SKIP_EISUU) {
        skipType00 = VirtualKey::VK_JIS_IM_CHANGE::SKIP_EISUU;
      }

      newflag_ = ModifierFlag::NONE;
      if (seesawType00 != -1) {
        index00 = VirtualKey::VK_JIS_IM_CHANGE::get_index_for_seesaw_AtoB_WSD(seesawType00);

        if (seesawType00 == VirtualKey::VK_JIS_IM_CHANGE::EISUU_KANA   && index00 == VirtualKey::VK_JIS_IM_CHANGE::wsdEISU ||
            seesawType00 == VirtualKey::VK_JIS_IM_CHANGE::KANA_OTHERS  && index00 == VirtualKey::VK_JIS_IM_CHANGE::wsdHIRA ||
            seesawType00 == VirtualKey::VK_JIS_IM_CHANGE::KANA_EISUU   && index00 == VirtualKey::VK_JIS_IM_CHANGE::wsdHIRA ||
            seesawType00 == VirtualKey::VK_JIS_IM_CHANGE::EISUU_OTHERS && index00 == VirtualKey::VK_JIS_IM_CHANGE::wsdEISU) {
          VirtualKey::VK_JIS_IM_CHANGE::setTimeoutMS(VirtualKey::VK_JIS_IM_CHANGE::CALLBACK_SEESAW_INIT);
        }

      } else {
        int sign00;
        if (skipType00 == VirtualKey::VK_JIS_IM_CHANGE::SKIP_EISUU_KANA ||
            skipType00 == VirtualKey::VK_JIS_IM_CHANGE::SKIP_KANA ||
            skipType00 == VirtualKey::VK_JIS_IM_CHANGE::SKIP_EISUU) {
          VirtualKey::VK_JIS_IM_CHANGE::reverse_sign_CHANGE_SKIP(9);
        }

        if (skipType00 == VirtualKey::VK_JIS_IM_CHANGE::SKIP_NONE_BACK ||
            skipType00 == VirtualKey::VK_JIS_IM_CHANGE::SKIP_PRE_BACK) {
          sign00 = -1;
        } else {
          sign00 =  1;
        }


        if (! use_ainu) {
          skip00[VirtualKey::VK_JIS_IM_CHANGE::wsdAINU] = 1;
        }
        if (skipType00 == VirtualKey::VK_JIS_IM_CHANGE::SKIP_NONE_FORWARD ||
            skipType00 == VirtualKey::VK_JIS_IM_CHANGE::SKIP_NONE_BACK) {
          replace_num00 = 1;
        } else {
          if (skipType00 == VirtualKey::VK_JIS_IM_CHANGE::SKIP_EISUU_KANA) {
            skip00[VirtualKey::VK_JIS_IM_CHANGE::wsdEISU] = 1;
            skip00[VirtualKey::VK_JIS_IM_CHANGE::wsdHIRA] = 1;
            replace_num00 = 3;
          } else if (skipType00 == VirtualKey::VK_JIS_IM_CHANGE::SKIP_KANA) {
            skip00[VirtualKey::VK_JIS_IM_CHANGE::wsdHIRA] = 1;
            replace_num00 = 3;
          } else if (skipType00 == VirtualKey::VK_JIS_IM_CHANGE::SKIP_EISUU) {
            skip00[VirtualKey::VK_JIS_IM_CHANGE::wsdEISU] = 1;
            replace_num00 = 3;
          } else {
            replace_num00 = 2;
          }
        }
        index00 = VirtualKey::VK_JIS_IM_CHANGE::get_index_for_replaceWSD(sign00, skip00, replace_num00);
      }

      if (index00 == VirtualKey::VK_JIS_IM_CHANGE::wsdEISU) {
        newkeycode_ = KeyCode::JIS_COLON;
        newflag_    = ModifierFlag::CONTROL_L | ModifierFlag::SHIFT_L;
      } else if (index00 == VirtualKey::VK_JIS_IM_CHANGE::wsdHIRA) {
        newkeycode_ = KeyCode::J;
        newflag_    = ModifierFlag::CONTROL_L | ModifierFlag::SHIFT_L;
      } else if (index00 == VirtualKey::VK_JIS_IM_CHANGE::wsdKATA) {
        newkeycode_ = KeyCode::K;
        newflag_    = ModifierFlag::CONTROL_L | ModifierFlag::SHIFT_L;
      } else if (index00 == VirtualKey::VK_JIS_IM_CHANGE::wsdHKAT) {
        newkeycode_ = KeyCode::SEMICOLON;
        newflag_    = ModifierFlag::CONTROL_L | ModifierFlag::SHIFT_L;
      } else if (index00 == VirtualKey::VK_JIS_IM_CHANGE::wsdFEIS) {
        newkeycode_ = KeyCode::L;
        newflag_    = ModifierFlag::CONTROL_L | ModifierFlag::SHIFT_L;
      } else if (index00 == VirtualKey::VK_JIS_IM_CHANGE::wsdAINU) {
        newkeycode_ = KeyCode::JIS_BRACKET_RIGHT;
        newflag_    = ModifierFlag::CONTROL_L | ModifierFlag::SHIFT_L;
      } else {
        return true;
      }

    }

    if (params.ex_iskeydown) {
      FlagStatus::temporary_decrease(params.flags);
      FlagStatus::temporary_increase(newflag_);
    } else {
      FlagStatus::temporary_decrease(newflag_);
      FlagStatus::temporary_increase(params.flags);
    }

    if (params.ex_iskeydown) {
      bool result00 = false;
      if (! ignore_improveIM) {
        result00 = VirtualKey::VK_JIS_IM_CHANGE::replace_WSD(newkeycode_, newflag_);
      }
      if (result00) {
        VirtualKey::VK_JIS_IM_CHANGE::static_set_pass_initialize(VirtualKey::VK_JIS_IM_CHANGE::INIT_NOT);
      } else {
        VirtualKey::VK_JIS_IM_CHANGE::static_set_pass_initialize(VirtualKey::VK_JIS_IM_CHANGE::INIT_DO);
      }
    }

    Params_KeyboardEventCallBack::auto_ptr ptr(Params_KeyboardEventCallBack::alloc(params.eventType,
                                                                                   FlagStatus::makeFlags(),
                                                                                   newkeycode_,
                                                                                   params.keyboardType,
                                                                                   params.repeat));
    if (ptr) {
      EventOutputQueue::FireKey::fire(*ptr);
    }
    return true;
  }

  void
  VirtualKey::VK_JIS_IM_CHANGE::ControlWorkspaceData(Params_KeyboardEventCallBack& params, int stage00)
  {
    bool isKeyDown = params.ex_iskeydown;
    bool keyisCCOS;
    bool isonCCO;
    bool isonS;
    bool is_eisuu_kana_kanashift;
    bool isonCCOSonly;
    int pass_initialize00;
    int ignore_improveIM = Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_remap_jis_ignore_improvement_IM_changing);
    bool result00;

    if (ignore_improveIM) return;

    keyisCCOS = params.key == KeyCode::COMMAND_L || params.key == KeyCode::COMMAND_R ||
                params.key == KeyCode::CONTROL_L || params.key == KeyCode::CONTROL_R ||
                params.key == KeyCode::OPTION_L  || params.key == KeyCode::OPTION_R  ||
                params.key == KeyCode::SHIFT_L   || params.key == KeyCode::SHIFT_R;

    isonCCO = FlagStatus::makeFlags().isOn(ModifierFlag::COMMAND_L) || params.flags == ModifierFlag::COMMAND_L ||
              FlagStatus::makeFlags().isOn(ModifierFlag::COMMAND_R) || params.flags == ModifierFlag::COMMAND_R ||
              FlagStatus::makeFlags().isOn(ModifierFlag::CONTROL_L) || params.flags == ModifierFlag::CONTROL_L ||
              FlagStatus::makeFlags().isOn(ModifierFlag::CONTROL_R) || params.flags == ModifierFlag::CONTROL_R ||
              FlagStatus::makeFlags().isOn(ModifierFlag::OPTION_L)  || params.flags == ModifierFlag::OPTION_L  ||
              FlagStatus::makeFlags().isOn(ModifierFlag::OPTION_R)  || params.flags == ModifierFlag::OPTION_R;

    isonS = FlagStatus::makeFlags().isOn(ModifierFlag::SHIFT_L) || params.flags == ModifierFlag::SHIFT_L ||
            FlagStatus::makeFlags().isOn(ModifierFlag::SHIFT_R) || params.flags == ModifierFlag::SHIFT_R;

    is_eisuu_kana_kanashift =  params.key == KeyCode::JIS_EISUU && ! isonCCO && ! isonS ||
                              params.key == KeyCode::JIS_EISUU && ! isonS ||
                              params.key == KeyCode::JIS_KANA  && ! isonCCO && ! isonS ||
                              params.key == KeyCode::JIS_KANA  && isonS;

    isonCCOSonly = (isonS || isonCCO) && ! keyisCCOS;

    int isPPP = (is_eisuu_kana_kanashift || isonCCOSonly);
    KeyCode key00  = params.key;
    Flags flag00 = params.flags;

    if (isKeyDown) {
      if (stage00 == POST_REMAP) {
        VirtualKey::VK_JIS_IM_CHANGE::reverse_sign_CHANGE_SKIP(0);
      } else if (stage00 == JUST_AFTER_REMAP) {
        VirtualKey::VK_JIS_IM_CHANGE::reverse_sign_CHANGE_SKIP(1);
        return;
      }
    }

    if (! isKeyDown) {
      if (! isPPP && ! isonCCO && stage00 == POST_REMAP) {
        VirtualKey::VK_JIS_IM_CHANGE::setTimeoutMS(VirtualKey::VK_JIS_IM_CHANGE::CALLBACK_INIT);
      }

      return;
    }

    if (stage00 == POST_REMAP) {
      VirtualKey::VK_JIS_IM_CHANGE::cancelTimeout();
    }

    if (stage00 == POST_REMAP && ! ignore_improveIM) {
      VirtualKey::VK_JIS_IM_CHANGE::learn_WSD();
    }

    pass_initialize00 = VirtualKey::VK_JIS_IM_CHANGE::static_get_pass_initialize();

    bool conC2_1 = isPPP  && stage00 == NON_REMAPPED;
    bool conC2_2 = ! isPPP && stage00 == POST_REMAP && pass_initialize00 == VirtualKey::VK_JIS_IM_CHANGE::INIT_DO;
    bool conC3   = ! isPPP && stage00 == NON_REMAPPED;

    if (ignore_improveIM) {
      if (conC2_1 || conC2_2) {
        VirtualKey::VK_JIS_IM_CHANGE::update_WSD();
        VirtualKey::VK_JIS_IM_CHANGE::static_set_pass_initialize(conC2_1 ? VirtualKey::VK_JIS_IM_CHANGE::INIT_DO : VirtualKey::VK_JIS_IM_CHANGE::INIT_NOT);
        Handle_VK_JIS_TEMPORARY::resetSavedIMD();
      }
    } else {
      if (conC2_1) {
        result00 = VirtualKey::VK_JIS_IM_CHANGE::replace_WSD(key00, flag00);
        if (result00) {
          VirtualKey::VK_JIS_IM_CHANGE::static_set_pass_initialize(VirtualKey::VK_JIS_IM_CHANGE::INIT_NOT);
          Handle_VK_JIS_TEMPORARY::resetSavedIMD();
        } else {
          VirtualKey::VK_JIS_IM_CHANGE::static_set_pass_initialize(VirtualKey::VK_JIS_IM_CHANGE::INIT_DO);
        }
      }else if (conC2_2) {
        VirtualKey::VK_JIS_IM_CHANGE::update_WSD();
        VirtualKey::VK_JIS_IM_CHANGE::static_set_pass_initialize(VirtualKey::VK_JIS_IM_CHANGE::INIT_NOT);
        Handle_VK_JIS_TEMPORARY::resetSavedIMD();
      }
    }
    if (conC3) {
      EventOutputQueue::FireKey::fire_downup(Flags(0), KeyCode::VK_JIS_TEMPORARY_RESTORE, params.keyboardType);
      VirtualKey::VK_JIS_IM_CHANGE::static_set_pass_initialize(VirtualKey::VK_JIS_IM_CHANGE::INIT_NOT);
    }
    return;
  }

  void
  VirtualKey::VK_JIS_IM_CHANGE::reverse_sign_CHANGE_SKIP(int when00)
  {
    if (when00 == 0) {
      pre_counter_plus_minus2_ = counter_plus_minus2_;
    } else if (when00 == 1) {
      if (counter_plus_minus2_ == pre_counter_plus_minus2_ && counter_plus_minus2_ > 0) {
        pre_counter_plus_minus2_ = 0;
        counter_plus_minus2_ = 0;
        if (sign_plus_minus2_ == -99) {
          sign_plus_minus2_ = 1;
        }
        sign_plus_minus2_ = -sign_plus_minus2_;
      } else {}
    } else {
      ++counter_plus_minus2_;
    }
  }

  int
  VirtualKey::VK_JIS_IM_CHANGE::get_WSDindex(InputModeDetail imd00, KeyCode modekey00, Flags flag00) {
    int index00;
    bool CtlSft = (flag00 == (ModifierFlag::CONTROL_L | ModifierFlag::SHIFT_L) ||
                   flag00 == (ModifierFlag::CONTROL_L | ModifierFlag::SHIFT_R) ||
                   flag00 == (ModifierFlag::CONTROL_R | ModifierFlag::SHIFT_L) ||
                   flag00 == (ModifierFlag::CONTROL_R | ModifierFlag::SHIFT_R) );

    if (imd00 == InputModeDetail::UNKNOWN) {
      if (modekey00 == KeyCode::JIS_EISUU && ! (flag00 == ModifierFlag::SHIFT_L || flag00 == ModifierFlag::SHIFT_R) ||
          modekey00 == KeyCode::JIS_COLON && CtlSft) {
        imd00 = InputModeDetail::ROMAN;
      } else if (modekey00 == KeyCode::JIS_KANA  && ! (flag00 == ModifierFlag::SHIFT_L || flag00 == ModifierFlag::SHIFT_R) ||
                 modekey00 == KeyCode::J && CtlSft) {
        imd00 = InputModeDetail::JAPANESE;
      } else if (modekey00 == KeyCode::JIS_KANA  && (flag00 == ModifierFlag::SHIFT_L || flag00 == ModifierFlag::SHIFT_R) ||
                 modekey00 == KeyCode::K && CtlSft) {
        imd00 = InputModeDetail::JAPANESE_KATAKANA;
      } else if (modekey00 == KeyCode::JIS_EISUU && (flag00 == ModifierFlag::SHIFT_L || flag00 == ModifierFlag::SHIFT_R) ||
                 modekey00 == KeyCode::SEMICOLON && CtlSft) {
        imd00 = InputModeDetail::JAPANESE_HALFWIDTH_KANA;
      } else if (modekey00 == KeyCode::JIS_KANA  && (flag00 == ModifierFlag::OPTION_L || flag00 == ModifierFlag::OPTION_R) ||
                 modekey00 == KeyCode::JIS_BRACKET_RIGHT && CtlSft) {
        imd00 = InputModeDetail::AINU;
      } else if (modekey00 == KeyCode::L && CtlSft) {
        imd00 = InputModeDetail::JAPANESE_FULLWIDTH_ROMAN;
      } else {
        return -1;
      }
    }

    if (imd00 == InputModeDetail::ROMAN) {
      index00 = wsdEISU;
    } else if (imd00 == InputModeDetail::JAPANESE_HIRAGANA || imd00 == InputModeDetail::JAPANESE) {
      index00 = wsdHIRA;
    } else if (imd00 == InputModeDetail::JAPANESE_KATAKANA) {
      index00 = wsdKATA;
    } else if (imd00 == InputModeDetail::JAPANESE_HALFWIDTH_KANA) {
      index00 = wsdHKAT;
    } else if (imd00 == InputModeDetail::AINU) {
      index00 = wsdAINU;
    } else if (imd00 == InputModeDetail::JAPANESE_FULLWIDTH_ROMAN) {
      index00 = wsdFEIS;
    } else {
      return -1;
    }
    return index00;
  }

  // XXX: clean up this function.
  bool
  VirtualKey::VK_JIS_IM_CHANGE::control_WSD(ControlWorkspaceDataType type, KeyCode modekey00, Flags flag00, InputModeDetail IMD00)
  {
    int ignore_improveIM = Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_remap_jis_ignore_improvement_IM_changing);
    int index00 = 1;
    BridgeWorkSpaceData curWSD00 = CommonData::getcurrent_workspacedata();

    if (type == CONTROL_WORKSPACEDATA_UPDATE) {
      wsd_public_ = curWSD00;

      index00 = IMD2index(wsd_public_.inputmodedetail);
      set_new_index(index00);
      return true;
    }

    if (! (! ignore_improveIM &&
           (type == CONTROL_WORKSPACEDATA_LEARN ||
            type == CONTROL_WORKSPACEDATA_REPLACE ||
            type == CONTROL_WORKSPACEDATA_RESTORE))) {
      return false;
    }

    if (type == CONTROL_WORKSPACEDATA_LEARN) {
      index00 = IMD2index(curWSD00.inputmodedetail);
    } else if (type == CONTROL_WORKSPACEDATA_REPLACE) {
      index00 = modeKey2index(modekey00, flag00);
    } else {
      index00 = IMD2index(IMD00);
    }
    if (index00 < 0) return false;

    if (type == CONTROL_WORKSPACEDATA_LEARN) {
      if (InputModeDetail::NONE == wsd_save_[wsdSET].inputmodedetail) {
        wsd_save_[wsdSET] = curWSD00;
        VirtualKey::VK_JIS_IM_CHANGE::static_set_pass_initialize(VirtualKey::VK_JIS_IM_CHANGE::INIT_DO);
        set_indexes_directly(-1, index00, -1);
      }
      wsd_save_[index00] = curWSD00;
      return true;

    } else if ((type == CONTROL_WORKSPACEDATA_REPLACE || type == CONTROL_WORKSPACEDATA_RESTORE) && ! ignore_improveIM) {
      if (InputModeDetail::NONE != wsd_save_[index00].inputmodedetail) {
        wsd_public_ = wsd_save_[index00];
        if (type == CONTROL_WORKSPACEDATA_REPLACE) {
          set_new_index(index00);
        }
      } else {
        return false;
      }
    } else {
      return false;
    }
    return true;
  }

  BridgeWorkSpaceData VirtualKey::VK_JIS_IM_CHANGE::wsd_public_;
  BridgeWorkSpaceData VirtualKey::VK_JIS_IM_CHANGE::wsd_save_[7];

  KeyCode VirtualKey::VK_JIS_IM_CHANGE::newkeycode_;
  Flags VirtualKey::VK_JIS_IM_CHANGE::newflag_;
  TimerWrapper VirtualKey::VK_JIS_IM_CHANGE::restore_timer_;
  int VirtualKey::VK_JIS_IM_CHANGE::callback2_ = VirtualKey::VK_JIS_IM_CHANGE::CALLBACK_INIT;
  int VirtualKey::VK_JIS_IM_CHANGE::pass_initialize2_ = VirtualKey::VK_JIS_IM_CHANGE::INIT_NOT;
  // XXX change variable name
  int VirtualKey::VK_JIS_IM_CHANGE::case1_pass_restore2_ = 0;
  int VirtualKey::VK_JIS_IM_CHANGE::pre_index2_ = -1;
  int VirtualKey::VK_JIS_IM_CHANGE::cur_index2_ = -1;
  int VirtualKey::VK_JIS_IM_CHANGE::others_index2_ = -1;
  int VirtualKey::VK_JIS_IM_CHANGE::sign_plus_minus2_ = -99;
  int VirtualKey::VK_JIS_IM_CHANGE::counter_plus_minus2_ = 0;
  int VirtualKey::VK_JIS_IM_CHANGE::pre_counter_plus_minus2_ = 0;
  bool VirtualKey::VK_JIS_IM_CHANGE::seesaw_init2_ = false;
}
