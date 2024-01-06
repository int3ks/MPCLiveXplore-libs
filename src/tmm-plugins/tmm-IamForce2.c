/*
__ __| |           |  /_) |     ___|             |           |
  |   __ \   _ \  ' /  | |  / |      _ \ __ \   |      _` | __ \   __|
  |   | | |  __/  . \  |   <  |   |  __/ |   |  |     (   | |   |\__ \
 _|  _| |_|\___| _|\_\_|_|\_\\____|\___|_|  _| _____|\__,_|_.__/ ____/
-----------------------------------------------------------------------------
TKGL_MIDIMAPPER  custom mapping library .
This is a custom midi mapping library for TKGL_MIDIMAPPER LD_PRELOAD library

IAMFORCE PLUGIN -- Force Emulation on MPC

-----------------------------------------------------------------------------

  Disclaimer.
  This work is licensed under the Creative Commons Attribution-NonCommercial 4.0 International License.
  To view a copy of this license, visit http://creativecommons.org/licenses/by-nc/4.0/
  or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
  NON COMMERCIAL - PERSONAL USE ONLY : You may not use the material for pure
  commercial closed code solution without the licensor permission.
  You are free to copy and redistribute the material in any medium or format,
  adapt, transform, and build upon the material.
  You must give appropriate credit, a link to the github site
  https://github.com/TheKikGen/USBMidiKliK4x4 , provide a link to the license,
  and indicate if changes were made. You may do so in any reasonable manner,
  but not in any way that suggests the licensor endorses you or your use.
  You may not apply legal terms or technological measures that legally restrict
  others from doing anything the license permits.
  You do not have to comply with the license for elements of the material
  in the public domain or where your use is permitted by an applicable exception
  or limitation.
  No warranties are given. The license may not give you all of the permissions
  necessary for your intended use.  This program is distributed in the hope that
  it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#define _GNU_SOURCE
#include <stdint.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <alsa/asoundlib.h>
#include <dlfcn.h>
#include <sys/time.h>

// midimapper defines -----------------------------------------------------------

#include "tkgl_midimapper.h"

// Log utilities ---------------------------------------------------------------

#include "tkgl_logutil.h"

// -----------------------------------------------------------------------------

#define IAMFORCE_VERSION "V2.0 BETA"


#define MPC_BANK_A 1
#define MPC_BANK_B 2
#define MPC_BANK_C 3
#define MPC_BANK_D 4

#define MPC_BANK_E 5
#define MPC_BANK_F 6
#define MPC_BANK_G 7
#define MPC_BANK_H 8


// IAMFORCE MACRO CC# on channel 16
// First CC is 00
// General principle : value = 0 : OFF, 1 : Toggle ON/OFF , 0x7F : ON
// Toggle mode means Force button ON/OFF event (to simulate a press with one value)

#define IAMFORCE_MACRO_MIDICH 0x0F

enum IamForceMacroCCNumbers { 
IAMFORCE_CC_MACRO_PLAY, 
IAMFORCE_CC_MACRO_REC, 
IAMFORCE_CC_MACRO_STOP, 
IAMFORCE_CC_MACRO_TAP, 
IAMFORCE_CC_MACRO_NEXT_SEQ,
IAMFORCE_CC_MACRO_PREV_SEQ,
IAMFORCE_CC_MACRO_FIRST_SEQ,
IAMFORCE_CC_MACRO_LAST_SEQ,
IAMFORCE_CC_MACRO_ARP,
IAMFORCE_CC_MACRO_UNDO,
IAMFORCE_CC_MACRO_LAST_ENTRY
};

// Force columns solo modes
enum ForceSoloModes  {  FORCE_SM_MUTE, FORCE_SM_SOLO, FORCE_SM_REC_ARM, FORCE_SM_CLIP_STOP, FORCE_SM_END };

// Extern globals --------------------------------------------------------------

extern int MPC_Id ;
extern int MPC_Spoofed_Id;

extern const DeviceInfo_t DeviceInfoBloc[];
extern snd_rawmidi_t *raw_outpub;
extern TkRouter_t TkRouter;
extern char ctrl_cli_name[] ;

// extern ld_preload function
extern typeof(&snd_rawmidi_write) orig_snd_rawmidi_write;


// Globals ---------------------------------------------------------------------

// Sysex patterns.
static const uint8_t AkaiSysex[]                  = {AKAI_SYSEX_HEADER};
static const uint8_t IdentityReplySysexHeader[]   = {AKAI_SYSEX_IDENTITY_REPLY_HEADER};

// To navigate in matrix quadran when MPC spoofing a Force
// 1 quadran is 4 MPC pads
//  Q1 Q2
//  Q3 Q4
static int MPCPadQuadran = MPC_BANK_C;

// Force Matrix pads color cache - 10 lines of 8 pads
static RGBcolor_t Force_PadColorsCache[8*10];

// Force Button leds cache
static uint8_t Force_ButtonLedsCache[128];

// SHIFT Holded mode
// Holding shift will activate the shift mode
static bool ShiftMode=false;

static bool EraseMode = false;

//static bool LaunchMode = false;

static bool MixPressed = false;
static bool PlayPressed = false;

static int CopyButtonstate;

// Shift mode within controller
static bool CtrlShiftMode = false;

// Column pads Mode
static bool ControllerColumnsPadMode = false;
static bool ControllerColumnsPadModeLocked = false;

// Current Solo Mode
static int CurrentSoloMode = FORCE_SM_SOLO;

// CurrentPad mode Note or Stepseq
static int currentPadMode = 0;
static int MatrixActive = 0;

// Solo mode buttons map : same order as ForceSoloModes enum
static const uint8_t SoloModeButtonMap [] = { FORCE_BT_MUTE,FORCE_BT_SOLO, FORCE_BT_REC_ARM, FORCE_BT_CLIP_STOP };

// shitfmode for knobs (especially for LIve/One having only 4 knobs)
static int Current_QLink_LED = MPC_BT_QLINK_SELECT_LED_1;

bool KnobsShiftMode() {
    if (DeviceInfoBloc[MPC_Id].qlinkKnobsCount == 4 && (Current_QLink_LED == MPC_BT_QLINK_SELECT_LED_2 || Current_QLink_LED == MPC_BT_QLINK_SELECT_LED_4)) {
        return true;
    }
    return false;
}
// USed by next seq macro
static int CurrentSequence = 0;
static int CurrentMatrixVOffset=0;

// Quadran for external controller (none by default)
static int CtrlPadQuadran = 0;



typedef struct mapping {
    int force;
    int mpc;
    //send force button with shift when shift is NOT pressed
    int shift;
    int altForceButtonIfShiftIsPressed;
    int altForceButton;
}mapping;

// Function prototypes ---------------------------------------------------------

static void MPCSetMapButtonLed(snd_seq_event_t *ev);
static void MPCSetMapButton(snd_seq_event_t *ev) ;

static bool ControllerEventReceived(snd_seq_event_t *ev);
static int  ControllerInitialize();
static void IamForceMacro_NextSeq(int step);
static void MPCRefresCurrentQuadran() ;

static mapping getForceMappingValue(uint8_t mpcValue);

static uint8_t getForcePadIndex(uint8_t mpcPad);
static uint8_t mapForceNote(mapping* values, int mpckey, int size);

// Midi controller specific ----------------------------------------------------
// Include here your own controller implementation

//map mpc pad to chromatic notes in force NOTES mode
static mapping mpcPadForceNoteMapping[] = {
    {106,49}, {107,55}, {108,51}, {109,53},
    {102,48}, {103,47}, {104,45}, {105,43},
    {114,40}, {115,38}, {116,46}, {117,44},
    {110,37}, {111,36}, {112,42}, {113,82}
};

//map steps 2x8 first bar to 4x4 padmatrix in STEP mode
//if quadrant = 2 then force step +=16
static mapping mpcPadForceStepsMapping[] = {
     {54,49}, {55,55}, {56,51}, {57,53}, {58,48}, {59,47}, {60,45}, {61,43},
     {62,40}, {63,38}, {64,46}, {65,44}, {66,37}, {67,36}, {68,42}, {69,82}
};

bool upperrow(int mpckey) {
    if (mpckey == 49 || mpckey == 55 || mpckey == 51 || mpckey == 53 || mpckey == 48 || mpckey == 47 || mpckey == 45 || mpckey == 43) {
        return true;
    }
    return false;
}

static mapping mpcPadToMute[] = {
    {FORCE_BT_MUTE_PAD5,49}, {FORCE_BT_MUTE_PAD6,55}, {FORCE_BT_MUTE_PAD7,51}, {FORCE_BT_MUTE_PAD8,53},
    {FORCE_BT_MUTE_PAD1,48}, {FORCE_BT_MUTE_PAD2,47}, {FORCE_BT_MUTE_PAD3,45}, {FORCE_BT_MUTE_PAD4,43},

    {FORCE_BT_MUTE_PAD5,40}, {FORCE_BT_MUTE_PAD6,38}, {FORCE_BT_MUTE_PAD7,46}, {FORCE_BT_MUTE_PAD8,44},
    {FORCE_BT_MUTE_PAD1,37}, {FORCE_BT_MUTE_PAD2,36}, {FORCE_BT_MUTE_PAD3,42}, {FORCE_BT_MUTE_PAD4,82}
};

static mapping mpcPadToLaunch[] = {
    {FORCE_BT_LAUNCH_5,40}, {FORCE_BT_LAUNCH_6,38}, {FORCE_BT_LAUNCH_7,46}, {FORCE_BT_LAUNCH_8,44},
    {FORCE_BT_LAUNCH_1,37}, {FORCE_BT_LAUNCH_2,36}, {FORCE_BT_LAUNCH_3,42}, {FORCE_BT_LAUNCH_4,82}
};

static mapping mpcPadToTrack[] = {
    {FORCE_BT_COLUMN_PAD5,40}, {FORCE_BT_COLUMN_PAD6,38}, {FORCE_BT_COLUMN_PAD7,46}, {FORCE_BT_COLUMN_PAD8,44},
    {FORCE_BT_COLUMN_PAD1,37}, {FORCE_BT_COLUMN_PAD2,36}, {FORCE_BT_COLUMN_PAD3,42}, {FORCE_BT_COLUMN_PAD4,82}
};

//map bank A (Q3) and bank B (Q4) map to 4x4 matrix in STEP mode
// if quadrant = 4 then force +=4
static mapping mpcPadForceStepmodeNotemapping[] = {
    {86,49},  {87,55},  {88,51},  {89,53},
    {94,48},  {95,47},  {96,45},  {97,43},
    {102,40}, {103,38}, {104,46}, {105,44},
    {110,37}, {111,36}, {112,42}, {113,82}
};

static uint8_t mapForceNote(mapping* values, int mpckey, int size) {
    for (int i = 0; i < size; i++) {
        if (values[i].mpc == mpckey) {
            return values[i].force;
        }
    }
    return 0;
}

static uint8_t getForcePadIndex(uint8_t mpcPad) {
    uint8_t ForcePadNote = 0;

    if (currentPadMode == FORCE_BT_STEP_SEQ) {
        //STEPS
        if (MPCPadQuadran == MPC_BANK_C || MPCPadQuadran == MPC_BANK_D || MPCPadQuadran == MPC_BANK_G || MPCPadQuadran == MPC_BANK_H) {
            ForcePadNote = mapForceNote(mpcPadForceStepsMapping, mpcPad, 16);
            switch (MPCPadQuadran) {
                case MPC_BANK_D: ForcePadNote += 16; break;
                case MPC_BANK_G: ForcePadNote += 32; break;
                case MPC_BANK_H: ForcePadNote += 48; break;
            }
        }
        //NOTES AND VELOCITY
        else {
            ForcePadNote = mapForceNote(mpcPadForceStepmodeNotemapping, mpcPad, 16);
            if (MPCPadQuadran == MPC_BANK_B) {
                ForcePadNote += 4;
            }
        }
    }
    else if (currentPadMode == FORCE_BT_NOTE) {
        ForcePadNote = mapForceNote(mpcPadForceNoteMapping, mpcPad, 16);
        switch (MPCPadQuadran) {
            case MPC_BANK_B: ForcePadNote -= 16; break;
            case MPC_BANK_C: ForcePadNote -= 32; break;
            case MPC_BANK_D: ForcePadNote -= 48; break;
        }
    }
    else if (currentPadMode == FORCE_BT_LAUNCH) {
        ForcePadNote = mapForceNote(mpcPadForceStepmodeNotemapping, mpcPad, 16);
        
        switch (MPCPadQuadran) {
            case MPC_BANK_D: ForcePadNote += 4; break;
            case MPC_BANK_A: ForcePadNote -= 32; break;
            case MPC_BANK_B: ForcePadNote -= 28; break;
        }
    }
    return ForcePadNote;
}


///////////////////////////////////////////////////////////////////////////////
// Show  MPC pads of columns mode (SSM) 
///////////////////////////////////////////////////////////////////////////////

int padscolorcacheindexLAUNC[16] = { 56,57,58,59,48,49,50,51,40,41,42,43,32,33,34,35 };
int padscolorcacheindexNOTES[16] = { 8, 9,10,11,12,13,14,15, 0, 1, 2, 3, 4, 5, 6, 7 };
int padscolorcacheindexSTPSQ[16] = { 12,13,14,15, 8, 9,10,11, 4, 5, 6, 7, 0, 1, 2, 3 };

static void MPCRefresCurrentQuadran() {
    int padcolorindex = 0;
   // tklog_debug("start MPCRefresCurrentQuadran\n");
    for (uint8_t padnumber = 0; padnumber < 16; padnumber++) {

        if (MixPressed) {
            //line 8 and 9
            padcolorindex = padnumber + 64;
          //  tklog_debug("MPCRefresCurrentQuadran MIXER padindex: %d padcolorindex: %d\n",padnumber , padcolorindex);

        }else

        if (EraseMode) {
            //line 8 and 9
            padcolorindex = padnumber + 72;
          //  tklog_debug("MPCRefresCurrentQuadran MIXER padindex: %d padcolorindex: %d\n", padnumber, padcolorindex);
            if (padnumber > 7) { padcolorindex = -1; }
        }
        else

        if (currentPadMode == FORCE_BT_STEP_SEQ){
            if (MPCPadQuadran == MPC_BANK_C || MPCPadQuadran == MPC_BANK_D || MPCPadQuadran == MPC_BANK_G || MPCPadQuadran == MPC_BANK_H) {
                padcolorindex = padscolorcacheindexSTPSQ[padnumber];
                switch (MPCPadQuadran) {
                case MPC_BANK_D: padcolorindex += 16; break;
                case MPC_BANK_G: padcolorindex += 32; break;
                case MPC_BANK_H: padcolorindex += 48; break;
                }
               // tklog_debug("MPCRefresCurrentQuadran STEPS padcolorindex: %d\n", padcolorindex);

            }
            else if(MPCPadQuadran == MPC_BANK_A || MPCPadQuadran == MPC_BANK_B) {
                padcolorindex = padscolorcacheindexLAUNC[padnumber];
                switch (MPCPadQuadran) {
                //case MPC_BANK_A:padcolorindex -= 0; break;
                case MPC_BANK_B:padcolorindex += 4; break;
                }
              //  tklog_debug("MPCRefresCurrentQuadran LAUNCH padcolorindex: %d\n", padcolorindex);
            }
        }
        else if (currentPadMode == FORCE_BT_NOTE) {
            padcolorindex = padscolorcacheindexNOTES[padnumber];
            switch (MPCPadQuadran) {
                case MPC_BANK_C:padcolorindex += 16; break;
                case MPC_BANK_B:padcolorindex += 32; break;
                case MPC_BANK_A:padcolorindex += 48; break;
            }
           // tklog_debug("MPCRefresCurrentQuadran NOTE padcolorindex: %d\n", padcolorindex);
        }
        else if (currentPadMode == FORCE_BT_LAUNCH) {
            padcolorindex = padscolorcacheindexLAUNC[padnumber];
            switch (MPCPadQuadran) {
                case MPC_BANK_A:padcolorindex -= 32; break;
                case MPC_BANK_B:padcolorindex -= 28; break;
                case MPC_BANK_D:padcolorindex += 4; break;
            }
           // tklog_debug("MPCRefresCurrentQuadran LAUNCH padcolorindex: %d\n",padcolorindex);
        }
        if (padcolorindex < 0) {
            DeviceSetPadColorRGB(MPC_Id, padnumber, 0, 0, 0);
        }
        else {
            DeviceSetPadColorRGB(MPC_Id, padnumber, Force_PadColorsCache[padcolorindex].c.r, Force_PadColorsCache[padcolorindex].c.g, Force_PadColorsCache[padcolorindex].c.b);
        }
    }
}



//#define _LIVE2_

static mapping buttonmapping[] = {
    {FORCE_BT_ENCODER , MPC_BT_ENCODER},
    {FORCE_BT_SHIFT, MPC_BT_SHIFT},
    {FORCE_BT_PLAY, MPC_BT_PLAY},
    {FORCE_BT_REC, MPC_BT_REC},
    {FORCE_BT_MATRIX, MPC_BT_MAIN,0,1,FORCE_BT_NAVIGATE},
    {FORCE_BT_LAUNCH, MPC_BT_PLAY_START},
    {FORCE_BT_TAP_TEMPO, MPC_BT_TAP_TEMPO},
    {FORCE_BT_UNDO, MPC_BT_UNDO},
    {FORCE_BT_NOTE, MPC_BT_16_LEVEL},
    {FORCE_BT_MENU, MPC_BT_MENU,0,1,FORCE_BT_LOAD},
    {FORCE_BT_ARP, MPC_BT_NOTE_REPEAT},

#ifdef _LIVE2_
     {FORCE_BT_CLIP, MPC_BT_MUTE},
#endif
#ifdef _ONE_
     { FORCE_BT_CLIP, MPC_BT_TRACK_MUTE },
     { FORCE_BT_NAVIGATE, MPC_BT_PROG_EDIT, FORCE_BT_CLIP },
     { FORCE_BT_MATRIX, MPC_BT_SAMPLER,0,1,FORCE_BT_NAVIGATE},
     { FORCE_BT_CLIP, MPC_BT_SAMPLE_EDIT },
#endif

#if defined _LIVE2_ || _ONE_
#warning MPC Model MPC LIVE II
    {FORCE_BT_DELETE, MPC_BT_ERASE },
    {FORCE_BT_MIXER, MPC_BT_CHANNEL_MIXER},
    {FORCE_BT_MASTER, MPC_BT_FULL_LEVEL},
    {FORCE_BT_NAVIGATE, MPC_BT_NEXT_SEQ, FORCE_BT_CLIP },
    {FORCE_BT_STEP_SEQ, MPC_BT_STEP_SEQ},
    {FORCE_BT_STOP, MPC_BT_STOP},
    {FORCE_BT_STOP_ALL, MPC_BT_OVERDUB , FORCE_BT_REC},
    {FORCE_BT_KNOBS, MPC_BT_QLINK_SELECT},

#else
    {FORCE_BT_EDIT, MPC_BT_ERASE},
    {FORCE_BT_MIXER, MPC_BT_FULL_LEVEL},
    {FORCE_BT_SAVE, MPC_BT_OVERDUB},
    {FORCE_BT_ASSIGN_A, MPC_BT_BANK_A},
    {FORCE_BT_ASSIGN_B,MPC_BT_BANK_B},
    {FORCE_BT_STEP_SEQ,MPC_BT_BANK_C},
    {FORCE_BT_CLIP,MPC_BT_BANK_D},
#endif
};

mapping null = { -1,-1,false };
static mapping getForceMappingValue(uint8_t mpcValue) {
    size_t size = sizeof(buttonmapping) / sizeof(buttonmapping[0]);
    for (int i = 0; i < size; i++) {
        if (buttonmapping[i].mpc == mpcValue) {
            return buttonmapping[i];
        }
    }
    return null;
}

// To compile define one of the following preprocesseur variables :
// NONE is the default.
//#define _LUMI_
#if defined _APCKEY25MK2_
   #warning IamForce driver id : APCKEY25MK2
   #include "Iamforce-APCKEY25MK2.h"
#elif defined _APCMINIMK2_
   #warning IamForce driver id : APCMINIMK2
   #include "Iamforce-APCMINIMK2.h"
#elif defined _LPMK3_
   #warning IamForce driver id : LPMK3
   #include "Iamforce-LPMK3.h"
#elif defined _LPX_
   #warning IamForce driver id : LPX
   #include "Iamforce-LPX.h"
#elif defined _LPPROMK3_
   #warning IamForce driver id : LPPROMK3
   #include "Iamforce-LPPROMK3.h"
#elif defined _LPMK2_
   #warning IamForce driver id : LPMK2
   #include "Iamforce-LPMK2.h"
#elif defined _KIKPADMK3_
   #warning IamForce driver id : KIKPADMK3
   #include "Iamforce-KIKPADMK3.h"
#elif defined _LUMI_
    #warning IamForce driver id : LUMI
    #include "Iamforce-LUMI.h"
#else 
   #warning IamForce driver id : NONE
   #include "Iamforce-NONE.h"
#endif

// Midi controller specific END ------------------------------------------------



///////////////////////////////////////////////////////////////////////////////
// Set a mapped MPC button LED from a Force button Led Event
///////////////////////////////////////////////////////////////////////////////

static void MPCSetMapButtonLed(snd_seq_event_t *ev) { 

  int mapVal = -1 ;

  if (ev->data.control.value == 3 &&
      (ev->data.control.param == FORCE_BT_NOTE || ev->data.control.param == FORCE_BT_STEP_SEQ || ev->data.control.param == FORCE_BT_LAUNCH)) {
      tklog_debug("switch padmode to %d\n", ev->data.control.param);

      currentPadMode = ev->data.control.param;
  }

  if (ev->data.control.param == FORCE_BT_MATRIX) {
      if (ev->data.control.value == 3) {
          MatrixActive = 1;
      }
      else {
          MatrixActive = 0;
      }
      tklog_debug("MatrixActive %d\n", MatrixActive);
  }

  bool sent = false;
  size_t size = sizeof(buttonmapping) / sizeof(buttonmapping[0]);
  for (int i = 0; i < size; i++) {
      if (buttonmapping[i].force == ev->data.control.param) {
          ev->data.control.param = buttonmapping[i].mpc;
          SendMidiEvent(ev);
          sent = true;
      }
  }
  if (sent) {
      return;
  }
  else if (ev->data.control.param == FORCE_BT_ASSIGN_A) {
      MPCPadQuadran = MPC_BANK_A;

      ev->data.control.param = MPC_BT_BANK_A;
      ev->data.control.value = MPC_BUTTON_COLOR_RED_HI;
      SendMidiEvent(ev);

      ev->data.control.param = MPC_BT_BANK_B;
      ev->data.control.value = MPC_BUTTON_COLOR_RED_LO;
      SendMidiEvent(ev);

      ev->data.control.param = MPC_BT_BANK_D;
      SendMidiEvent(ev);
   
      ev->data.control.param = MPC_BT_QLINK_SELECT_LED_1;
      SendMidiEvent(ev);

      ev->data.control.param = MPC_BT_PLUS;
      SendMidiEvent(ev);

      ev->data.control.param = MPC_BT_MINUS;
      SendMidiEvent(ev);

      ev->data.control.param = MPC_BT_TC;
      SendMidiEvent(ev);

      ev->data.control.param = MPC_BT_BANK_C;
      
      SendMidiEvent(ev);

      ev->data.control.param = MPC_BT_COPY;
      SendMidiEvent(ev);

      
      return;
  }

  else if ( ev->data.control.param == FORCE_BT_MUTE )   {
    if ( ev->data.control.value == 3 ) {
      CurrentSoloMode = FORCE_SM_MUTE ; // Resynchronize
    }
  }

  else if ( ev->data.control.param == FORCE_BT_SOLO )   {
    if ( ev->data.control.value == 3 ) {
      CurrentSoloMode = FORCE_SM_SOLO ; // Resynchronize
    }
  }

  else if ( ev->data.control.param == FORCE_BT_REC_ARM ) {
    if ( ev->data.control.value == 3 ) {
      CurrentSoloMode = FORCE_SM_REC_ARM ; // Resynchronize
    }
  }

  else if ( ev->data.control.param == FORCE_BT_CLIP_STOP )   {
    if ( ev->data.control.value == 3 ) {
      CurrentSoloMode = FORCE_SM_CLIP_STOP ; // Resynchronize
    }
  }

  if ( mapVal >=0 ) {
    ev->data.control.param = mapVal;
    SendMidiEvent(ev );
  }

}


static void SetBankButtonLED(int button,bool shift) {
    snd_seq_event_t ev2;
    snd_seq_ev_clear(&ev2);
    ev2.type = SND_SEQ_EVENT_CONTROLLER;
    ev2.data.control.channel = 0;
    SetMidiEventDestination(&ev2, TO_CTRL_MPC_PRIVATE);

    int bankbuttons[] = { MPC_BT_BANK_A, MPC_BT_BANK_B, MPC_BT_BANK_C, MPC_BT_BANK_D };
    for( int i = 0; i < 4; i++) {
        ev2.data.control.param = bankbuttons[i];
        ev2.data.control.value = MPC_BUTTON_COLOR_RED_LO;
        SendMidiEvent(&ev2);
    }
    ev2.data.control.param = button;
    ev2.data.control.value = shift ?  MPC_BUTTON_COLOR_YELLOW_HI : MPC_BUTTON_COLOR_RED_HI;
   // tklog_debug("SetBankButtonLED param: %d -> value: %d \n", ev2.data.control.param, ev2.data.control.value);
    SendMidiEvent(&ev2);

    MPCRefresCurrentQuadran();
}

static void SetButtonLED(int button, int MPC_BUTTON_COLOR) {
    snd_seq_event_t ev2;
    snd_seq_ev_clear(&ev2);
    ev2.type = SND_SEQ_EVENT_CONTROLLER;
    ev2.data.control.channel = 0;
    SetMidiEventDestination(&ev2, TO_CTRL_MPC_PRIVATE);
    ev2.data.control.param = button;
    ev2.data.control.value = MPC_BUTTON_COLOR;
    SendMidiEvent(&ev2);
}

static void SetQLinkButtonLed(int Current_QLink_LED) {
    snd_seq_event_t ev2;
    snd_seq_ev_clear(&ev2);
    ev2.type = SND_SEQ_EVENT_CONTROLLER;
    ev2.data.control.channel = 0;
    SetMidiEventDestination(&ev2, TO_CTRL_MPC_PRIVATE);

    int qleds[] = { MPC_BT_QLINK_SELECT_LED_1, MPC_BT_QLINK_SELECT_LED_2, MPC_BT_QLINK_SELECT_LED_3, MPC_BT_QLINK_SELECT_LED_4 };
    ev2.data.control.value = 0;/*led off*/
    for (int i = 0; i < 4; i++) {
        ev2.data.control.param = qleds[i];
        SendMidiEvent(&ev2);
    }

    ev2.data.control.param = Current_QLink_LED;
    ev2.data.control.value = 3;/*led on*/
    SendMidiEvent(&ev2);
}


static double qlinkButtonPress, qlinkButtonRelease;

static double timeInMs() {
    struct timeval now;
    gettimeofday(&now, NULL);
    double usec = (now.tv_sec * 1000000) + now.tv_usec;
    return usec / 1000.0f;
}


///////////////////////////////////////////////////////////////////////////////
// Set a mapped MPC button  to a Force button
///////////////////////////////////////////////////////////////////////////////
static void MPCSetMapButton(snd_seq_event_t *ev) {

    int mapVal = -1 ;
    bool shiftReleaseBefore = false;
    mapping map;
    
    if (ev->data.note.note == MPC_BT_PLAY) {
        PlayPressed = (ev->data.note.velocity == 0x7F);
      //  tklog_debug("PlayPressed %d\n", PlayPressed);
    }

    if (ev->data.note.note == MPC_BT_CHANNEL_MIXER) {
        MixPressed = (ev->data.note.velocity == 0x7F);
        //tklog_debug("mixPressed %d\n", MixPressed);
        MPCRefresCurrentQuadran();
    }

    if (ev->data.note.note == MPC_BT_ERASE) {
        EraseMode = (ev->data.note.velocity == 0x7F);
        MPCRefresCurrentQuadran();
    }

    if ( ev->data.note.note == MPC_BT_SHIFT ) {
        ShiftMode = ( ev->data.note.velocity == 0x7F ) ;
       // tklog_debug("ShiftMode %d\n", ShiftMode);
        mapVal = FORCE_BT_SHIFT ;
    }

    //shift + long press tap tempo open metronom
    else if (ShiftMode && ev->data.note.note == MPC_BT_TAP_TEMPO) {
        if (ev->data.note.velocity == 0x7F) {
            SendDeviceKeyEvent(FORCE_BT_MUTE_PAD5, 0x7F);
        }
        else {
            SendDeviceKeyEvent(FORCE_BT_MUTE_PAD5, 0);
        }
        return;
    }
   

    else if (ev->data.note.note == MPC_BT_BANK_A && ev->data.note.velocity == 0x7F) { MPCPadQuadran = MPC_BANK_A; SetBankButtonLED(MPC_BT_BANK_A,false);}
    else if (ev->data.note.note == MPC_BT_BANK_B && ev->data.note.velocity == 0x7F) { MPCPadQuadran = MPC_BANK_B; SetBankButtonLED(MPC_BT_BANK_B,false);}
    else if (ev->data.note.note == MPC_BT_BANK_C && ev->data.note.velocity == 0x7F) {
        if (ShiftMode) {
            MPCPadQuadran = MPC_BANK_G; SetBankButtonLED(MPC_BT_BANK_C,true);
        }
        else {
            MPCPadQuadran = MPC_BANK_C; SetBankButtonLED(MPC_BT_BANK_C,false);
        }
        
    }
    else if (ev->data.note.note == MPC_BT_BANK_D && ev->data.note.velocity == 0x7F) {
        if (ShiftMode) {
            MPCPadQuadran = MPC_BANK_H; SetBankButtonLED(MPC_BT_BANK_D, true);
        }
        else {
            MPCPadQuadran = MPC_BANK_D; SetBankButtonLED(MPC_BT_BANK_D, false);
        }
    }

    
    else if ( ev->data.note.note == MPC_BT_PLUS ) {
        if (currentPadMode == FORCE_BT_LAUNCH) {
            //plus goes up in screen
            if (ev->data.note.velocity == 0x7F) { IamForceMacro_NextSeq(-1); }
        }
        else {
            // octave + on PLUS key
            if (ev->data.note.velocity == 0x7F) {
                SendDeviceKeyEvent(FORCE_BT_SHIFT, 0x7F);
                SendDeviceKeyEvent(FORCE_BT_MUTE_PAD8, 0x7F);
            }
            else {
                SendDeviceKeyEvent(FORCE_BT_MUTE_PAD8, 0);
                SendDeviceKeyEvent(FORCE_BT_SHIFT, 0);
            }
        }
        return;
    } 
    else if (ev->data.note.note == MPC_BT_MINUS) {
        if (currentPadMode == FORCE_BT_LAUNCH) {
            //minus goes down in screen
            if (ev->data.note.velocity == 0x7F) { IamForceMacro_NextSeq(1); }
        }
        else {
            //octave - on minus key
            if (ev->data.note.velocity == 0x7F) {
                SendDeviceKeyEvent(FORCE_BT_SHIFT, 0x7F);
                SendDeviceKeyEvent(FORCE_BT_MUTE_PAD7, 0x7F);
            }
            else {
                SendDeviceKeyEvent(FORCE_BT_MUTE_PAD7, 0);
                SendDeviceKeyEvent(FORCE_BT_SHIFT, 0);
            }
        }
        return;
    }


    //long press TC open Timing correction
    else if (ev->data.note.note == MPC_BT_TC) {
        if (ev->data.note.velocity == 0x7F) {
            SendDeviceKeyEvent(FORCE_BT_SHIFT, 0x7F);
            SendDeviceKeyEvent(FORCE_BT_MUTE_PAD6, 0x7F);
        }
        else {
            SendDeviceKeyEvent(FORCE_BT_MUTE_PAD6, 0);
            SendDeviceKeyEvent(FORCE_BT_SHIFT, 0);
        }
        return;
    }
   
    //button mapped
    else if ((map = getForceMappingValue(ev->data.note.note)).force>=0 ) {
            if (!ShiftMode && map.shift > 0 && ev->data.note.velocity == 0x7F) {
                SendDeviceKeyEvent(FORCE_BT_SHIFT, 0x7F);
                SendDeviceKeyPress(map.shift);
                SendDeviceKeyEvent(FORCE_BT_SHIFT, 0);
                return;
            }
            else if (ShiftMode && map.altForceButtonIfShiftIsPressed > 0) {
                SendDeviceKeyEvent(FORCE_BT_SHIFT, 0);
                SendDeviceKeyPress(map.altForceButton);
                return;
            }
            else {
                mapVal = map.force;
            }
    }
    
    //QLink Select
    else if (  ev->data.note.note == MPC_BT_QLINK_SELECT ) {
      
        if (ShiftMode) {
            Current_QLink_LED = MPC_BT_QLINK_SELECT_LED_1;
            SetQLinkButtonLed(Current_QLink_LED);
            //SendDeviceKeyPress(FORCE_BT_NOTE);
            return;
        }

        mapVal = FORCE_BT_KNOBS;
        if (ev->data.note.velocity == 0x7F) {
            qlinkButtonPress = timeInMs();
        }
        else if (ev->data.note.velocity == 0) {
            qlinkButtonRelease = timeInMs();
            double duration = qlinkButtonRelease - qlinkButtonPress;
            if (duration < 800) {
                Current_QLink_LED++;
                if (Current_QLink_LED > MPC_BT_QLINK_SELECT_LED_4) { Current_QLink_LED = MPC_BT_QLINK_SELECT_LED_1; }
                
                if (Current_QLink_LED == MPC_BT_QLINK_SELECT_LED_2 || Current_QLink_LED == MPC_BT_QLINK_SELECT_LED_4) {
                    SendDeviceKeyEvent(FORCE_BT_KNOBS, 0);
                    SendDeviceKeyEvent(FORCE_BT_KNOBS, 0x7F);
                }
            }
            SetQLinkButtonLed(Current_QLink_LED);
        }
        
       
    }// Knobs touch
    else if (  ev->data.note.note >= MPC_BT_QLINK1_TOUCH && ev->data.note.note <= MPC_BT_QLINK16_TOUCH ) {
        mapVal = ev->data.note.note - 1 ;
        if (KnobsShiftMode()) {
            mapVal += 4;
        }
    }

    else if (  ev->data.note.note == MPC_BT_STOP ) {
      shiftReleaseBefore = ShiftMode;
      mapVal = ( ShiftMode ? FORCE_BT_STOP_ALL : FORCE_BT_STOP);
    }
    else if (  ev->data.note.note == MPC_BT_COPY ) {
        if (ev->data.note.velocity == 0x7F) {

//            if (currentPadMode == FORCE_BT_LAUNCH) {
                if (ShiftMode) {
                    shiftReleaseBefore = ShiftMode;
                    mapVal = FORCE_BT_DELETE;
                    //tklog_debug("copy is delete button\n");
                }
                else {
                    mapVal = FORCE_BT_COPY;
                }
  //          }
  //          else {
  //              mapVal = FORCE_BT_MUTE;
  //          }
            CopyButtonstate = mapVal;
        }
        else {
            mapVal = CopyButtonstate;
        }
    }

    //tklog_debug("Button %0x mapped to %0x (Shiftmode = %s)\n",ev->data.note.note,mapVal,ShiftMode ? "SHIFT MODE":"");

    if ( mapVal >=0 )  {

      ev->data.note.note = mapVal;

      // Check if we must send a SHIFT RELEASE event to avoid conflict with Force internal shift status
      if (shiftReleaseBefore) { 
          SendDeviceKeyEvent(FORCE_BT_SHIFT, false); 
          ShiftMode = 0;
          //tklog_debug("ShiftMode %d\n", ShiftMode);
      }

      SendMidiEvent(ev );

    }

}

///////////////////////////////////////////////////////////////////////////////
// The MidiMapperStart() function is called before branching to MPC entry point
///////////////////////////////////////////////////////////////////////////////
void MidiMapperStart() {

  tklog_info("IamForce : Force emulation on MPC devices, by The Kikgen Labs\n");
  tklog_info("IamForce : Version %s\n",IAMFORCE_VERSION);

  // Set the spoofed product code to catch product code read and power supply status access.
   MPC_Spoofed_Id = MPC_FORCE ;

  // Check if a client name was given on the command line
  if (ctrl_cli_name[0] == 0) {
    strcpy(ctrl_cli_name,IAMFORCE_ALSASEQ_DEFAULT_CLIENT_NAME);
    if (TkRouter.Ctrl.port < 0 ) TkRouter.Ctrl.port = IAMFORCE_ALSASEQ_DEFAULT_PORT;
    tklog_info("IamForce : Default client name (%s) and port (%d) will be used.\n",ctrl_cli_name,TkRouter.Ctrl.port);
  }

  memset( Force_ButtonLedsCache, 0, sizeof(Force_ButtonLedsCache) );
  memset( Force_PadColorsCache, 0 ,  sizeof(RGBcolor_t) * 8 * 10 );
  
}

///////////////////////////////////////////////////////////////////////////////
// The MidiMapperSetup() function is called when all ports initialized
///////////////////////////////////////////////////////////////////////////////
void MidiMapperSetup() {

  if ( strcmp(DeviceInfoBloc[MPC_Id].productCode,DeviceInfoBloc[MPC_FORCE].productCode) == 0  ) {
    tklog_fatal("You can't emulate a Force on a Force !!\n");
    exit(1);
  }

  // Draw something on MPC pads
  for ( uint8_t i = 0 ; i< 16 ; i++ ) DeviceSetPadColorValue(MPC_Id, i, COLOR_FULL_RED );
  DeviceSetPadColorValue(MPC_Id, 5, COLOR_WHITE );
  DeviceSetPadColorValue(MPC_Id, 6, COLOR_FULL_BLUE );
  DeviceSetPadColorValue(MPC_Id, 9, COLOR_FULL_GREEN );
  DeviceSetPadColorValue(MPC_Id, 10, COLOR_FULL_YELLOW );
 
  ControllerInitialize();
  
}

///////////////////////////////////////////////////////////////////////////////
// Midi mapper events processing
///////////////////////////////////////////////////////////////////////////////


bool MidiMapper( uint8_t sender, snd_seq_event_t *ev, uint8_t *buffer, size_t size ) {

    switch (sender) {

        // Event from PUBLIC MPC application rawmidi port
        // RAW MIDI. NO EVENT. USE BUFFER and SIZE (SYSEX)
    case FROM_MPC_PUBLIC:
    {
       

        int i = 0;
        //tklog_info("Midi RAW Event received from MPC PUBLIC %s\n", buffer[0]);

       /* tklog_info("Midi RAW Event %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\n",
            buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9],
            buffer[10], buffer[11], buffer[12], buffer[13], buffer[14], buffer[15]);*/


        // Akai Sysex :  F0 47 7F [id] (Fn) (len MSB) (len LSB) (data) F7
        while (buffer[i] == 0xF0 && buffer[i + 1] == 0x47 && i < size) {

            i += sizeof(AkaiSysex);
            // Change Sysex device id
            buffer[i++] = DeviceInfoBloc[MPC_Id].sysexId;

            // PAD COLOR
            // Sysex : set pad color : FN  F0 47 7F [id] -> 65 (00 04) [Pad #] [R] [G] [B] F7
            if (buffer[i] == AKAI_DEVICE_SX_SET_PAD_RGB) {

                i++;

                // Compute MSB + LSB len
                uint16_t msgLen = (buffer[i++] << 7) | buffer[i++];

                //= sizeof(MPCSysexPadColorFn) ;

                uint8_t padF;
                int padCt = -1;

                // Manage mutliple set colors in the same sysex stream
                while (msgLen > 0) {
                    padF = buffer[i];
                    
                    i++;

                    // Update Force pad color cache
                    Force_PadColorsCache[padF].c.r = buffer[i++];
                    Force_PadColorsCache[padF].c.g = buffer[i++];
                    Force_PadColorsCache[padF].c.b = buffer[i++];
                    if (Force_PadColorsCache[padF].c.g >= 64 || Force_PadColorsCache[padF].c.r >= 64 || Force_PadColorsCache[padF].c.b >= 64) {
//                        tklog_debug("PAD COLOR padF=%d greenvalue=%d \n", padF, Force_PadColorsCache[padF].c.g);


                    }
                    //if (Force_PadColorsCache[padF].c.g == 127) {
                     //   tklog_debug("PAD COLOR padF=%d greenvalue=%d \n", padF,Force_PadColorsCache[padF].c.g);
                    //}
                    //stklog_debug("PAD COLOR (len %d) (%d) %02X%02X%02X\n",msgLen,padF,Force_PadColorsCache[padF].c.r, Force_PadColorsCache[padF].c.g,Force_PadColorsCache[padF].c.b);
                    padCt = ControllerGetPadIndex(padF - CtrlPadQuadran);
                    if (padCt >= 0) ControllerSetPadColorRGB(padCt, Force_PadColorsCache[padF].c.r, Force_PadColorsCache[padF].c.g, Force_PadColorsCache[padF].c.b);

                    msgLen -= 4;
                }

            }

            // Reach end of sysex to loop on the next sysex command
            while (buffer[i++] != 0xF7) if (i >= size) break;
        }

        // Send the sysex to the MPC
        orig_snd_rawmidi_write(raw_outpub, buffer, size);

        // Refresh special modes
        if (ControllerColumnsPadMode) ControllerRefreshColumnsPads(true);
        MPCRefresCurrentQuadran();
        return false; // Do not allow default send

        break;
    }

    // Event from PRIVATE MPC application rawmidi port
    case FROM_MPC_PRIVATE:
        //tklog_info("Midi Event received from MPC PRIVATE\n");

        switch (ev->type) {
        case SND_SEQ_EVENT_CONTROLLER:
            // Button Led
            if (ev->data.control.channel == 0) {
                if (ev->data.control.param != 122) {
                    // Save Led status in cache
                    Force_ButtonLedsCache[ev->data.control.param] = ev->data.control.value;
                   // if (ev->data.control.param != 53) tklog_debug("ButtonLedCache param: %d -> value: %d \n", ev->data.control.param, ev->data.control.value);
                    // Map with controller leds. Will send a midi msg to the controller
                    ControllerSetMapButtonLed(ev);

                    // Map with MPC device leds
                    MPCSetMapButtonLed(ev);
                }
                return false;

            }
            break;

        }

        break;

        // Event from MPC hardware internal controller
    case FROM_CTRL_MPC:
        //tklog_info("Midi Event received from CTRL MPC %d\n", ev->data.note.note);

        switch (ev->type) {

        case SND_SEQ_EVENT_SYSEX:

            // Identity request reply
            if (memcmp(ev->data.ext.ptr, IdentityReplySysexHeader, sizeof(IdentityReplySysexHeader)) == 0) {
                memcpy(ev->data.ext.ptr + sizeof(IdentityReplySysexHeader), DeviceInfoBloc[MPC_FORCE].sysexIdReply, sizeof(DeviceInfoBloc[MPC_FORCE].sysexIdReply));
            }

            break;

            // KNOBS TURN FROM MPC
            // B0 [10-31] [7F - n] : Qlinks    B0 64 nn : Main encoder
        case SND_SEQ_EVENT_CONTROLLER:
            if (ev->data.control.channel == 0 && ev->data.control.param >= 0x10 && ev->data.control.param <= 0x31) {
                if (KnobsShiftMode()) {
                    ev->data.control.param += 4;
                }
                if (ev->data.control.param > 0x17) { 
                    tklog_debug("data.control.param > 0x17\n");
                    return false; 
                }
               // tklog_debug("qlinks param %d value %d  shiftmode %d\n", ev->data.control.param, ev->data.control.value, ShiftMode);
            }

            break;

        case SND_SEQ_EVENT_NOTEON:
        case SND_SEQ_EVENT_NOTEOFF:
        case SND_SEQ_EVENT_KEYPRESS:
            // Buttons on channel 0
            if (ev->type != SND_SEQ_EVENT_KEYPRESS && ev->data.note.channel == 0) {
                MPCSetMapButton(ev);
                return false;
            }

            // Mpc Pads on channel 9
            if (ev->data.note.channel == 9) {
                //tklog_debug("PAD track received currentPadMode %d  shiftmode %d\n", currentPadMode, ShiftMode);
                bool  rowlaunchMode = ((MatrixActive || currentPadMode == FORCE_BT_LAUNCH) && PlayPressed);

                if (rowlaunchMode || EraseMode || MixPressed) {
                
                    if (ev->type == SND_SEQ_EVENT_NOTEON) {
                        uint8_t track = 0;
                        if      (EraseMode)     { track = mapForceNote(mpcPadToTrack,  ev->data.note.note, 8); }
                        else if (MixPressed)    { track = mapForceNote(mpcPadToMute,   ev->data.note.note, 16); }
                        else if (rowlaunchMode) { track = mapForceNote(mpcPadToLaunch, ev->data.note.note, 8); }

                        if (track > 0) {
                            if (MixPressed) { 
                                if (upperrow(ev->data.note.note)) {
                                    SendDeviceKeyPress(FORCE_BT_REC_ARM);
                                }
                                else {
                                    SendDeviceKeyPress(FORCE_BT_MUTE);
                                }
                            }
                            SendDeviceKeyPress(track);
                        }
                        MPCRefresCurrentQuadran();
                    }
                    return false;
                
                }

                uint8_t ForcePadNote = ev->data.note.note;
                ForcePadNote = getForcePadIndex(ev->data.note.note);
                if (ev->data.note.velocity > 0) {
                    //tklog_debug("PAD Event received as usual padmode %d -> orgnote= %d forcenote %d\n",currentPadMode, ev->data.note.note, ForcePadNote);
                }

                ev->data.note.note = ForcePadNote;

               
                //padmute mode for Step sequencer
                if (currentPadMode == FORCE_BT_STEP_SEQ &&
                    MPCPadQuadran == MPC_BANK_A &&
                    ShiftMode) {
                    SendDeviceKeyEvent(FORCE_BT_SHIFT, 0);
                    SendDeviceKeyEvent(FORCE_BT_MUTE, 0x7F);
                    SendMidiEvent(ev);
                    SendDeviceKeyEvent(FORCE_BT_MUTE, 0);
                    return false;
                }


                // If Shift Mode, simulate Select key
             /*   if (ShiftMode) {
                    SendDeviceKeyEvent(FORCE_BT_SELECT, 0x7F);
                    SendMidiEvent(ev);
                    SendDeviceKeyEvent(FORCE_BT_SELECT, 0);
                    return false;
                }
                */
            }

            break;

        }
        break;

        // Event from MPC application port mapped with external controller.
        // This port is also used to catch specific CC mapping for global Force command macros
        // By using the standard routing in the Force midi setting, it is possible 
        // to send specific CC/channel 16 to the  port name = "TKGL_(your controller name)" to trig those commands.

    case FROM_MPC_EXTCTRL:
        tklog_debug("Midi Event received from MPC EXCTRL\n");
        if (ev->type == SND_SEQ_EVENT_CONTROLLER) {

            // Is it one of our IAMFORCE macros on midi channel 16 ?
            if (ev->data.control.channel == IAMFORCE_MACRO_MIDICH && ev->data.control.param < IAMFORCE_CC_MACRO_LAST_ENTRY) {
                tklog_debug("Macro received %02X %02X\n", ev->data.control.param, ev->data.control.value);

                // CC 00 is the first CC on channel 16

                int mapVal = -1;
                switch (ev->data.control.param)
                {
                case IAMFORCE_CC_MACRO_PLAY:
                    mapVal = FORCE_BT_PLAY;
                    break;

                case IAMFORCE_CC_MACRO_STOP:
                    mapVal = FORCE_BT_STOP;
                    break;

                case IAMFORCE_CC_MACRO_REC:
                    mapVal = FORCE_BT_REC;
                    break;

                case IAMFORCE_CC_MACRO_TAP:
                    mapVal = FORCE_BT_TAP_TEMPO;
                    break;

                case IAMFORCE_CC_MACRO_PREV_SEQ:
                    if (ev->data.control.value > 0) IamForceMacro_NextSeq(-1);
                    break;

                case IAMFORCE_CC_MACRO_NEXT_SEQ:
                    if (ev->data.control.value > 0) IamForceMacro_NextSeq(1);
                    break;

                case IAMFORCE_CC_MACRO_FIRST_SEQ:
                    if (ev->data.control.value > 0) IamForceMacro_NextSeq(0);
                    break;

                case IAMFORCE_CC_MACRO_LAST_SEQ:
                    if (ev->data.control.value > 0) IamForceMacro_NextSeq(100);
                    break;

                case IAMFORCE_CC_MACRO_ARP:
                    mapVal = FORCE_BT_ARP;
                    break;

                case IAMFORCE_CC_MACRO_UNDO:
                    mapVal = FORCE_BT_UNDO;
                    break;

                }

                if (mapVal >= 0) {

                    if (ev->data.control.value == 1) SendDeviceKeyPress(mapVal);
                    else SendDeviceKeyEvent(mapVal, ev->data.control.value);
                }

                return false; // Never go back to MPC for macros...
            }
        }

        break;

        // Event from external controller HARDWARE

    case FROM_CTRL_EXT:
       // tklog_debug("Midi Event received from CTRL EXT\n");
        return ControllerEventReceived(ev);

    }
 return true;

}

///////////////////////////////////////////////////////////////////////////////
// IAMFORCE macros
///////////////////////////////////////////////////////////////////////////////

static void IamForceMacro_NextSeq(int step) {

  if ( step == 100 ) CurrentSequence = 7;
  else if (step == 0) CurrentSequence = 0;
  else CurrentSequence += step;

  if ( CurrentSequence > 7  ) {
        CurrentSequence = 7;
        CurrentMatrixVOffset++;
        // Send a Matrix down button
        SendDeviceKeyPress(FORCE_BT_DOWN);
  }
  else if ( CurrentSequence < 0 ) {
    CurrentSequence = 0;
    if ( CurrentMatrixVOffset ) {
        CurrentMatrixVOffset--;
        SendDeviceKeyPress(FORCE_BT_UP); 
    }
  }
//tklog_debug("Current suequence = %d\n",CurrentSequence);
  SendDeviceKeyPress(FORCE_BT_LAUNCH_1 + CurrentSequence); 
}
