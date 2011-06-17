#ifndef VIRTUALKEY_VK_JIS_IM_CHANGE_HPP
#define VIRTUALKEY_VK_JIS_IM_CHANGE_HPP

namespace org_pqrs_KeyRemap4MacBook {
  namespace VirtualKey {
    class VK_JIS_IM_CHANGE {
    public:
      enum {
        CUR_PRE      = 0,
        EISUU_KANA   = 1,
        KANA_OTHERS  = 2,
        KANA_EISUU   = 3,
        EISUU_OTHERS = 4,
      };

      enum {
        wsdSET   = 0,
        wsdEISU  = 1,
        wsdHIRA  = 2,
        wsdKATA  = 3,
        wsdHKAT  = 4,
        wsdAINU  = 5,
        wsdFEIS  = 6,
        wsdMAX   = 6,
      };

      enum {
        SKIP_NONE_FORWARD = 0,
        SKIP_NONE_BACK    = 1,
        SKIP_PRE_FORWARD  = 2,
        SKIP_PRE_BACK     = 3,
        SKIP_EISUU_KANA   = 4,
        SKIP_KANA         = 5,
        SKIP_EISUU        = 6,
      };

      static void initialize(IOWorkLoop& workloop);
      static void terminate(void);
      // XXX change the function name
      static void cancelTimeout(void);
      // XXX change the function name
      // XXX change name of callback00
      static void setTimeoutMS(int callback00);
      static int  static_get_pass_initialize(void);
      static void static_set_pass_initialize(int pass_initialize00);
      static void set_pass_initialize(int value00) { pass_initialize2_ = value00; }
      static void static_set_case1_pass_restore(int case1_pass_restore00);
      static int get_case1_pass_restore(void) { return case1_pass_restore2_; }

      static void ControlWorkspaceData(Params_KeyboardEventCallBack& params, int stage00);

      static void set_indexes_directly(int new_pre, int new_cur, int new_others) {
        if (new_pre > 0) {
          pre_index2_  = new_pre;
        }
        if (new_cur > 0) {
          cur_index2_ = new_cur;
        }
        if (new_others > 0) {
          others_index2_ = new_others;
        }
      }

      static void set_new_index(int index00) {
        if (cur_index2_ != -1 && pre_index2_ != -1) {
          if (cur_index2_ != index00) {
            set_indexes_directly(cur_index2_, index00, -1);
          }
        } else if (cur_index2_ == -1) {
          if (pre_index2_ != index00) {
            set_indexes_directly(-1, index00, -1);
          }
        } else {
          set_indexes_directly(index00, -1, -1);
        }
      };

      static void init_seesaw(void) {
        seesaw_init2_ = true;
      };

      static int get_index_for_seesaw_AtoB_WSD(int type00) {
        int tmp_index;
        int Aindex00, Bindex00;

        if (type00 == CUR_PRE) {
          Aindex00 = cur_index2_;
          Bindex00 = pre_index2_;
        } else if (type00 == EISUU_KANA) {
          Aindex00 = wsdEISU;
          Bindex00 = wsdHIRA;
        } else if (type00 == KANA_EISUU) {
          Aindex00 = wsdHIRA;
          Bindex00 = wsdEISU;
        } else if (type00 == KANA_OTHERS) {
          if (others_index2_ == -1) {
            set_indexes_directly(-1, -1, wsdKATA);
          }
          Aindex00 = wsdHIRA;
          Bindex00 = others_index2_;
        } else {
          if (others_index2_ == -1) {
            set_indexes_directly(-1, -1, wsdKATA);
          }
          Aindex00 = wsdEISU;
          Bindex00 = others_index2_;
        }

        if (cur_index2_ != -1 && pre_index2_ != -1) {
          if (type00 == CUR_PRE) {
            set_indexes_directly(Aindex00, Bindex00, -1);
            return cur_index2_;
          } else {
            tmp_index = cur_index2_;
          }

        } else {
          if (type00 == CUR_PRE) {
            if (pre_index2_ + 1 > wsdMAX) {
              set_indexes_directly(-1, 1, -1);
            } else {
              set_indexes_directly(-1, pre_index2_ + 1, -1);
            }
            return cur_index2_;
          } else {
            tmp_index = Bindex00;
          }
        }
        if (cur_index2_ != Aindex00) {
          set_indexes_directly(tmp_index, Aindex00, -1);
        } else {
          set_indexes_directly(Aindex00, Bindex00, -1);
        }
        if (seesaw_init2_) {
          if (cur_index2_ != Aindex00) {
            set_indexes_directly(cur_index2_, Aindex00, -1);
          }
          seesaw_init2_ = false;
        }
        return cur_index2_;
      };

      static int get_index_for_replaceWSD(int sign00, int skip[], int replace_num00) {
        int ii;
        int ret;
        int cur_index_tmp, pre_index_tmp, others_index_tmp;

        cur_index_tmp    = cur_index2_;
        pre_index_tmp    = pre_index2_;
        others_index_tmp = others_index2_;

        bool cond00 = (cur_index2_ == wsdEISU);
        bool cond01 = (pre_index2_ == wsdHKAT && cur_index2_ == wsdKATA);
        bool cond02 = (pre_index2_ != wsdKATA && cur_index2_ == wsdHKAT);
        bool cond10 = (pre_index2_ == wsdEISU && cur_index2_ == wsdHIRA);
        bool cond11 = (pre_index2_ == wsdKATA && cur_index2_ == wsdHIRA && others_index2_ == wsdKATA);
        bool cond12 = (pre_index2_ == wsdHKAT && cur_index2_ == wsdHIRA && others_index2_ == wsdHKAT);
        bool cond13 = (pre_index2_ != wsdHKAT && cur_index2_ == wsdKATA);
        bool cond14 = (pre_index2_ == wsdKATA && cur_index2_ == wsdHKAT);
        if (replace_num00 == 2) {
          skip[pre_index2_] = 1;

        } else if (replace_num00 == 3) {
          if (sign_plus_minus2_ == -99) {
            sign_plus_minus2_ = 1;
          }
          if (sign_plus_minus2_ == 1 &&
              (cond00 || cond01 || cond02)) {
            sign_plus_minus2_ = -1;
            if (cond00) {
              others_index_tmp = wsdEISU;
            }
          } else if (sign_plus_minus2_ == -1     && (
                       cond10 || cond11 || cond12 || cond13 || cond14)) {
            sign_plus_minus2_ = 1;
          } else {}
          sign00 = sign_plus_minus2_;
        }

        int continue_end00 = 0;
        for (ii = (cur_index_tmp == -1 ? 1 : cur_index_tmp);;) {
          if (ii > wsdMAX && sign00 == 1 || ii < 1 && sign00 == -1) {
            if (continue_end00 == 1) {
              ret = -1;
              break;
            }
            if (sign00 == 1) {
              ii = 1;
            } else {
              ii = wsdMAX;
            }
            continue_end00 = 1;
            continue;
          }

          if (cur_index_tmp != -1 && cur_index_tmp != ii &&
              others_index_tmp != ii) {
            if (skip[ii] != 1) {
              ret = ii;
              break;
            }
          }
          ii = ii + sign00;
        }

        if (ret > 0) {
          if (replace_num00 == 2) {
            set_indexes_directly(-1, ret, -1);
          } else {
            set_indexes_directly(cur_index_tmp, ret, -1);
          }
          if (replace_num00 == 2 || replace_num00 == 3) {
            set_indexes_directly(-1, -1, ret);
          }
        } else {
          ret = cur_index_tmp;
        }

        return ret;
      };

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

      enum {
        INIT_DO              = 0,
        INIT_NOT             = 1,
        CALLBACK_INIT        = 0,
        CALLBACK_RESTORE     = 1,
        CALLBACK_SEESAW_INIT = 2,
      };
      static bool handle(const Params_KeyboardEventCallBack& params);

    private:
      // XXX change the function name
      static void timeoutAfterKey2(OSObject* owner, IOTimerEventSource* sender);

      static KeyCode newkeycode_;
      static Flags newflag_;
      static TimerWrapper restore_timer_;
      // XXX change the variable name
      static int callback2_;
      static int pass_initialize2_;
      static int case1_pass_restore2_;
      static int pre_index2_;
      static int cur_index2_;
      static int others_index2_;
      static int sign_plus_minus2_;
      static int counter_plus_minus2_;
      static int pre_counter_plus_minus2_;
      static bool seesaw_init2_;

      // XXX remove this value (replace by CommonData::current_workspacedata_)
      static BridgeWorkSpaceData wsd_public_;
      static BridgeWorkSpaceData wsd_save_[wsdMAX + 1];
      static int get_WSDindex(InputModeDetail imd00, KeyCode modekey00, Flags flag00);
      static int IMD2index(InputModeDetail imd00) {
        return get_WSDindex(imd00, KeyCode::VK_NONE, ModifierFlag::NONE);
      }
      static int modeKey2index(KeyCode modekey00, Flags flag00) {
        return get_WSDindex(InputModeDetail::UNKNOWN, modekey00, flag00);
      }
    };
  }
}

#endif
