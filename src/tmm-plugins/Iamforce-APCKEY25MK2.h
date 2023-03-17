/*
__ __| |           |  /_) |     ___|             |           |
  |   __ \   _ \  ' /  | |  / |      _ \ __ \   |      _` | __ \   __|
  |   | | |  __/  . \  |   <  |   |  __/ |   |  |     (   | |   |\__ \
 _|  _| |_|\___| _|\_\_|_|\_\\____|\___|_|  _| _____|\__,_|_.__/ ____/
-----------------------------------------------------------------------------
TKGL_MIDIMAPPER  custom mapping library .
This is a custom midi mapping library for TKGL_MIDIMAPPER LD_PRELOAD library

AKAI APC KEY 25 MK2  FOR IAMFORCE
BASED ON THE DEFAULT USER CONFIGURATION

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

/*
-------------------------------------------------------------------------------
MAPPING CONFIGURATION WITH FORCE BUTTONS AND FUNCTIONS - APC KEY 25 MK2

-------------------------------------------------------------------------------
*/

// ----------------------------------------------------------------------------
// APC Key 25 MK2 module for IamForce
// ----------------------------------------------------------------------------

// SYSEX

#define SX_APCK_DEVICE_ID 0x4E
#define SX_APCK_HEADER 0xF0,0x47,0x7F,SX_APCK_DEVICE_ID

// LED RGB COLOR
// Sysex header
//     0x24 MSB (data size) LSB (data size)
//     (start pad) (end pad)
//     (red brigthness MSB) (red brigthness LSB)
//     (green brigthness MSB) (green brigthness LSB)
//     (blue brigthness MSB) (blue brigthness LSB)
// 0xF7
// NB : Due to a bug in th APC Key 25 mk2, the pad part of the sysex
// msg from the pad from-to must be duplicated.

uint8_t SX_APCK_LED_RGB_COLOR[] = {
  SX_APCK_HEADER, 0x24,
  0x00, 0x10,
  0x00, 0x00, 0x00, 0X00, 0x00, 0x00, 0x00, 0X00,
  0x00, 0x00, 0x00, 0X00, 0x00, 0x00, 0x00, 0X00,
  0xF7 };


#define CTRL_BT_TRACK_1 0x40
#define CTRL_BT_TRACK_2 0x41
#define CTRL_BT_TRACK_3 0x42
#define CTRL_BT_TRACK_4 0x43
#define CTRL_BT_TRACK_5 0x44
#define CTRL_BT_TRACK_6 0x45
#define CTRL_BT_TRACK_7 0x46
#define CTRL_BT_TRACK_8 0x47

#define CTRL_BT_LAUNCH_1 0x52
#define CTRL_BT_LAUNCH_2 0x53
#define CTRL_BT_LAUNCH_3 0x54
#define CTRL_BT_LAUNCH_4 0x55
#define CTRL_BT_LAUNCH_5 0x56

#define CTRL_BT_STOP_ALL_CLIPS 0x51
#define CTRL_BT_PLAY 0x5B
#define CTRL_BT_RECORD 0x5D
#define CTRL_BT_SHIFT 0x62

// Knobs are relative
#define CTRL_KNOB_1 0x30
#define CTRL_KNOB_2 0x31
#define CTRL_KNOB_3 0x32
#define CTRL_KNOB_4 0x33
#define CTRL_KNOB_5 0x34
#define CTRL_KNOB_6 0x35
#define CTRL_KNOB_7 0x36
#define CTRL_KNOB_8 0x37

// Pads start at 0 from bottom left to upper right
#define CTRL_PAD_NOTE_OFFSET 0x27
#define CTRL_PAD_MAX_LINE 5
#define CTRL_PAD_MAX_COL  8

// Standard colors
#define CTRL_COLOR_BLACK 0
#define CTRL_COLOR_DARK_GREY 1
#define CTRL_COLOR_GREY 2
#define CTRL_COLOR_WHITE 3
#define CTRL_COLOR_RED 5
#define CTRL_COLOR_RED_LT 0x07
#define CTRL_COLOR_AMBER 9
#define CTRL_COLOR_YELLOW 13
#define CTRL_COLOR_LIME 17
#define CTRL_COLOR_GREEN 21
#define CTRL_COLOR_SPRING 25
#define CTRL_COLOR_TURQUOISE 29
#define CTRL_COLOR_CYAN 33
#define CTRL_COLOR_SKY 37
#define CTRL_COLOR_OCEAN 41
#define CTRL_COLOR_BLUE 45
#define CTRL_COLOR_ORCHID 49
#define CTRL_COLOR_MAGENTA 53
#define CTRL_COLOR_PINK 57

// Static integer color - This is Ableton definitions
// and it is used with a lot of devices, like Launchpads

// const uint32_t ColorAblMap[] = {
//   0x000000, 0x1E1E1E, 0x7F7F7F, 0xFFFFFF, 0xFF4C4C, 0xFF0000, 0x590000, 0x190000, 0xFFBD6C, 0xFF5400,
//   0x591D00, 0x271B00, 0xFFFF4C, 0xFFFF00, 0x595900, 0x191900, 0x88FF4C, 0x54FF00, 0x1D5900, 0x142B00,
//   0x4CFF4C, 0x00FF00, 0x005900, 0x001900, 0x4CFF5E, 0x00FF19, 0x00590D, 0x001902, 0x4CFF88, 0x00FF55,
//   0x00591D, 0x001F12, 0x4CFFB7, 0x00FF99, 0x005935, 0x001912, 0x4CC3FF, 0x00A9FF, 0x004152, 0x001019,
//   0x4C88FF, 0x0055FF, 0x001D59, 0x000819, 0x4C4CFF, 0x0000FF, 0x000059, 0x000019, 0x874CFF, 0x5400FF,
//   0x190064, 0x0F0030, 0xFF4CFF, 0xFF00FF, 0x590059, 0x190019, 0xFF4C87, 0xFF0054, 0x59001D, 0x220013,
//   0xFF1500, 0x993500, 0x795100, 0x436400, 0x033900, 0x005735, 0x00547F, 0x0000FF, 0x00454F, 0x2500CC,
//   0x7F7F7F, 0x202020, 0xFF0000, 0xBDFF2D, 0xAFED06, 0x64FF09, 0x108B00, 0x00FF87, 0x00A9FF, 0x002AFF,
//   0x3F00FF, 0x7A00FF, 0xB21A7D, 0x402100, 0xFF4A00, 0x88E106, 0x72FF15, 0x00FF00, 0x3BFF26, 0x59FF71,
//   0x38FFCC, 0x5B8AFF, 0x3151C6, 0x877FE9, 0xD31DFF, 0xFF005D, 0xFF7F00, 0xB9B000, 0x90FF00, 0x835D07,
//   0x392b00, 0x144C10, 0x0D5038, 0x15152A, 0x16205A, 0x693C1C, 0xA8000A, 0xDE513D, 0xD86A1C, 0xFFE126,
//   0x9EE12F, 0x67B50F, 0x1E1E30, 0xDCFF6B, 0x80FFBD, 0x9A99FF, 0x8E66FF, 0x404040, 0x757575, 0xE0FFFF,
//   0xA00000, 0x350000, 0x1AD000, 0x074200, 0xB9B000, 0x3F3100, 0xB35F00, 0x4B1502
// }

// uint32_t weightedEuclideanColorDistance(uint8_t r1,uint8_t g1,uint8_t b1, uint8_t r2, uint8_t g2,uint8_t b2) {
// }
//
//   uint16_t redMean = ( r1 + r2 ) / 2;
//   uint16_t r = rgb1[0] - rgb2[0] ; r *= r;
//   uint16_t g = rgb1[1] - rgb2[1] ; g *= g;
//   uint16_t b = rgb1[2] - rgb2[2] ; b *= b;
//
//
//   return Math.sqrt(((2+(redMean/256)) *  r) + (4*g) + ((2+((255-redMean)/256))*b)).toFixed(2)
// }




/*
Fire
Clip = 7F0000
No clip = 060101
Track select = 7F1919

Tangerine
Clip = 7F2200
No clip = 060201
Track select = 7F3419

Apricot
Clip = 7F4400
No clip = 060401
Track select = 7F5019

Canary
Clip =  7F6A00
No clip = 060501
Track select = 7F6E19

Lemon
Clip = 737F00
No clip = 050601
Track select = 757F19

Chartreuse
Clip = 517F00
No clip = 040601
Track select = 5A7F19

Neon
Clip =  2A7F00
No clip = 020601
Track select = 3B7F19

Lime
Clip =  087F00
No clip = 010601
Track select = 207F19

Clover
Clip = 007F19
No clip = 010602
Track select = 197F2E

Sea
Clip =007F40
No clip =010603
Track select =197F4C

Mint
Clip = 007F62
No clip = 010605
Track select = 197F68

Neon
Clip =
No clip =
Track select =

Neon
Clip =
No clip =
Track select =
*/

///////////////////////////////////////////////////////////////////////////////
// APC Key 25 Mk2 Specifics
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// APC Key 25 Set several pads RGB Colors
///////////////////////////////////////////////////////////////////////////////
static void ControllerSetMultiPadsColorRGB(uint8_t padCtFrom, uint8_t padCtTo, uint8_t r, uint8_t g, uint8_t b) {

  //  F0 47 7F 4E 24
  //  00 10
  //  00 00 00 7F 00 00 00 00
  //  00 00 00 7F 00 00 00 00
  //  F7

  SX_APCK_LED_RGB_COLOR[7]  = padCtFrom;
  SX_APCK_LED_RGB_COLOR[8]  = padCtTo;

  SX_APCK_LED_RGB_COLOR[9]  = r  >> 7  ;
  SX_APCK_LED_RGB_COLOR[10]  = r & 0x7F;

  SX_APCK_LED_RGB_COLOR[11]  = g  >> 7  ;
  SX_APCK_LED_RGB_COLOR[12]  = g & 0x7F;

  SX_APCK_LED_RGB_COLOR[13]  = b  >> 7  ;
  SX_APCK_LED_RGB_COLOR[14]  = b & 0x7F;

  // Firmware bug workaround
  memcpy( &SX_APCK_LED_RGB_COLOR[15],&SX_APCK_LED_RGB_COLOR[7],8 );

  SeqSendRawMidi( TO_CTRL_EXT,SX_APCK_LED_RGB_COLOR,sizeof(SX_APCK_LED_RGB_COLOR) );
}

///////////////////////////////////////////////////////////////////////////////
// APC Key 25 fill the matrix with a RGB color
///////////////////////////////////////////////////////////////////////////////
static void ControllerFillPadMatrixColorRGB(uint8_t r, uint8_t g, uint8_t b) {
  ControllerSetMultiPadsColorRGB(0, CTRL_PAD_NOTE_OFFSET, r,  g,  b) ;
}

///////////////////////////////////////////////////////////////////////////////
// APC Key 25 Set a pad RGB Colors
///////////////////////////////////////////////////////////////////////////////
static void ControllerSetPadColorRGB(uint8_t padCt, uint8_t r, uint8_t g, uint8_t b) {
  ControllerSetMultiPadsColorRGB(padCt, padCt, r,  g,  b) ;
}

///////////////////////////////////////////////////////////////////////////////
// Show a line from Force pad color cache (source = Force pad#, Dest = ctrl pad #)
///////////////////////////////////////////////////////////////////////////////
static void ControllerDrawPadsLineFromForceCache(uint8_t SrcForce, uint8_t DestCtrl) {

    if ( DestCtrl >= CTRL_PAD_MAX_LINE ) return;

    uint8_t pf = SrcForce * 8 ;
    uint8_t pl = DestCtrl * CTRL_PAD_MAX_COL ;

    for ( uint8_t i = 0  ; i <  8 ; i++) {
      ControllerSetPadColorRGB(pl, Force_PadColorsCache[pf].c.r, Force_PadColorsCache[pf].c.g,Force_PadColorsCache[pf].c.b);
      pf++; pl++;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Refresh the pad surface from Force pad cache
///////////////////////////////////////////////////////////////////////////////
static void ControllerRefreshPadsFromForceCache() {

    for ( int i = 0 ; i< 64 ; i++) {

      // Set pad for external controller eventually
      uint8_t padCt = ( ( 7 - i / 8 ) * 10 + 11 + i % 8 );
      ControllerSetPadColorRGB(padCt, Force_PadColorsCache[i].c.r, Force_PadColorsCache[i].c.g,Force_PadColorsCache[i].c.b);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Controller initialization
///////////////////////////////////////////////////////////////////////////////
static int ControllerInitialize() {

  tklog_info("IamForce : Akai APC Key 25 MK2 implementation.\n");

}

///////////////////////////////////////////////////////////////////////////////
// Get a Force pad index from an APC Key index
///////////////////////////////////////////////////////////////////////////////
static uint8_t ControllerGetForcePadIndex(uint8_t padCt) {
  // Convert pad to Force pad #
  uint8_t padC  =  padCt  %  CTRL_PAD_MAX_COL ;
  uint8_t padL  =  CTRL_PAD_MAX_LINE - 1 - padCt / CTRL_PAD_MAX_COL ;
  return   padL * 8 + padC ;
}

///////////////////////////////////////////////////////////////////////////////
// Get a Force pad note from an APC Key index
///////////////////////////////////////////////////////////////////////////////
static uint8_t ControllerGetForcePadNote(uint8_t padCt) {
  return   ControllerGetForcePadIndex(padCt) + FORCEPADS_NOTE_OFFSET;
}

///////////////////////////////////////////////////////////////////////////////
// Get a APC Key Controller pad index from a Force pad index
///////////////////////////////////////////////////////////////////////////////
static int ControllerGetPadIndex(uint8_t padF) {

  if ( padF > CTRL_PAD_NOTE_OFFSET ) return -1;

  return  ( ( CTRL_PAD_MAX_LINE - 1 - padF / 8 ) * CTRL_PAD_MAX_COL + padF % 8 );
}

///////////////////////////////////////////////////////////////////////////////
// Map LED lightning messages from Force with Launchpad buttons leds colors
///////////////////////////////////////////////////////////////////////////////
static void ControllerSetMapButtonLed(snd_seq_event_t *ev) {

    int mapVal = -1 ;
    int mapVal2 = -1;

    if ( ev->data.control.param != FORCE_BT_TAP_TEMPO)  tklog_debug("LED VALUE  %02X = %02X\n",ev->data.control.param,ev->data.control.value);   


    if      ( ev->data.control.param == FORCE_BT_LAUNCH_1 )   mapVal = CTRL_BT_LAUNCH_1  ;
    else if ( ev->data.control.param == FORCE_BT_LAUNCH_2 )   mapVal = CTRL_BT_LAUNCH_2  ;
    else if ( ev->data.control.param == FORCE_BT_LAUNCH_3 )   mapVal = CTRL_BT_LAUNCH_3  ;
    else if ( ev->data.control.param == FORCE_BT_LAUNCH_4 )   mapVal = CTRL_BT_LAUNCH_4  ;
    else if ( ev->data.control.param == FORCE_BT_LAUNCH_5 )   mapVal = CTRL_BT_LAUNCH_5  ;
    
    else if ( ev->data.control.param == FORCE_BT_COPY )   {
      // LED : 0 = off, 1 low bright, 3 high bright
      mapVal  = CTRL_BT_TRACK_1;
      mapVal2 =  ev->data.control.value == 3 ? 0x7F:00 ;
    }   

    else if ( ev->data.control.param == FORCE_BT_DELETE )  {
      mapVal = CTRL_BT_TRACK_2 ;
      mapVal2 =  ev->data.control.value == 3 ? 0x7F:00;
    }

    else if ( ev->data.control.param == FORCE_BT_EDIT )  {
      mapVal = CTRL_BT_TRACK_3 ;
      mapVal2 =  ev->data.control.value == 3 ? 0x7F:00;
    }
    
    else if ( ev->data.control.param == FORCE_BT_STEP_SEQ )  {
      mapVal = CTRL_BT_TRACK_4 ;
      mapVal2 =  ev->data.control.value == 3 ? 0x7F:00;
    }

    else if ( ev->data.control.param == FORCE_BT_MIXER )  {
      mapVal = CTRL_BT_TRACK_5 ;
      mapVal2 =  ev->data.control.value == 3 ? 0x7F:00;
    }

    else if ( ev->data.control.param == FORCE_BT_LAUNCH )  {
      mapVal = CTRL_BT_TRACK_6 ;
      mapVal2 =  ev->data.control.value == 3 ? 0x7F:00;
    }

    else if ( ev->data.control.param == FORCE_BT_NOTE )  {
      mapVal = CTRL_BT_TRACK_7 ;
      mapVal2 =  ev->data.control.value == 3 ? 0x7F:00;
    }

    else if ( ev->data.control.param == FORCE_BT_MATRIX )  {
      mapVal = CTRL_BT_TRACK_8 ;
      mapVal2 =  ev->data.control.value == 3 ? 0x7F:00;
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
        // Send a LED message to the APC Key
        snd_seq_event_t ev2 = *ev;
        ev2.type = SND_SEQ_EVENT_NOTEON;
        ev2.data.note.channel = 0;
        ev2.data.note.note = mapVal;
        ev2.data.note.velocity = mapVal2 >= 0 ? mapVal2:ev->data.control.value;

        SetMidiEventDestination(&ev2, TO_CTRL_EXT );
        SendMidiEvent(&ev2 );
    }

}

///////////////////////////////////////////////////////////////////////////////
// Refresh columns mode lines 7 & 8 on the LaunchPad
///////////////////////////////////////////////////////////////////////////////
static void ControllerRefreshColumnsPads(bool show) {
  if ( show ) {
    // Line 9 = track selection
    // Line 8 = mute modes
    ControllerDrawPadsLineFromForceCache(9,1);
    ControllerDrawPadsLineFromForceCache(8,0);
  }
  else {
    ControllerDrawPadsLineFromForceCache(6,1);
    ControllerDrawPadsLineFromForceCache(7,0);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Process an event received from the Launchpad
///////////////////////////////////////////////////////////////////////////////
static bool ControllerEventReceived(snd_seq_event_t *ev) {

  switch (ev->type) {
    case SND_SEQ_EVENT_SYSEX:
      break;

    case SND_SEQ_EVENT_CONTROLLER:
    {

      if ( ev->data.control.channel == 0 ) {

        // Knob 1 - 8
        if ( ev->data.control.param >= CTRL_KNOB_1 && ev->data.control.param  <= CTRL_KNOB_8 ) {

          static uint8_t lastKnob = 0xFF;

          SetMidiEventDestination(ev,TO_MPC_PRIVATE );

          uint8_t k = ev->data.control.param - CTRL_KNOB_1 ;
          tklog_debug("Knob no %d\n",k);

          // Remap controller
          ev->data.control.param = FORCE_KN_QLINK_1 + k;
          tklog_debug("Knob remap %02x\n",ev->data.control.param);

          if ( lastKnob != k  ) {
            snd_seq_event_t ev2 = *ev;
            ev2.data.note.channel = 0;
            SetMidiEventDestination(&ev2,TO_MPC_PRIVATE );

            if ( lastKnob != 0xFF) {
              // Simulate an "untouch", but not the first time
              ev2.type = SND_SEQ_EVENT_NOTEOFF;
              ev2.data.note.velocity = 0x00;
              ev2.data.note.note = FORCE_BT_QLINK1_TOUCH + lastKnob ;
              SendMidiEvent(&ev2 );
            }

            // Simulate a "touch" knob
            ev2.type = SND_SEQ_EVENT_NOTEON;
            ev2.data.note.velocity = 0x7F;
            ev2.data.note.note = FORCE_BT_QLINK1_TOUCH + k ;
            SendMidiEvent(&ev2 );
          }

          lastKnob = k;
        }
      }
      break;
    }

    case SND_SEQ_EVENT_NOTEON:
    case SND_SEQ_EVENT_NOTEOFF:
    case SND_SEQ_EVENT_KEYPRESS:

      // Route to MPC app by default
      SetMidiEventDestination(ev,TO_MPC_PRIVATE );

      if ( ev->data.note.channel == 0 ) {

        if ( ev->data.note.note > CTRL_PAD_NOTE_OFFSET && ev->type != SND_SEQ_EVENT_KEYPRESS ) {
          // Buttons
          // NB : Release is a note off (0x80)  + velocity zero. We test nonly velocity here.
          int mapVal = -1;

          if       ( ev->data.note.note == CTRL_BT_SHIFT) {
            CtrlShiftMode = ( ev->data.note.velocity == 0x7F );
            return false;
          }

          // UP / COPY
          else if  ( ev->data.note.note == CTRL_BT_TRACK_1  ) {
            mapVal = CtrlShiftMode ? FORCE_BT_UP : FORCE_BT_COPY  ;
          }
          // DOWN / DELETE
          else if  ( ev->data.note.note == CTRL_BT_TRACK_2  ) {
            mapVal = CtrlShiftMode ? FORCE_BT_DOWN :FORCE_BT_DELETE ;
          }
          // LEFT / EDIT
          else if  ( ev->data.note.note == CTRL_BT_TRACK_3  ) {
            mapVal = CtrlShiftMode ? FORCE_BT_LEFT : FORCE_BT_EDIT  ;
          }

          // Right / Step Seq
          else if  ( ev->data.note.note == CTRL_BT_TRACK_4  ) {
            mapVal = CtrlShiftMode ? FORCE_BT_RIGHT :FORCE_BT_STEP_SEQ ;
          }

          // Volume / Mixer / Master
          else if  ( ev->data.note.note == CTRL_BT_TRACK_5  ) {
            mapVal = CtrlShiftMode ? FORCE_BT_MASTER : FORCE_BT_MIXER ;
          }

          // Pan / Launch
          else if  ( ev->data.note.note == CTRL_BT_TRACK_6  ) {
             mapVal = FORCE_BT_LAUNCH ;
          }

          // Send / Note
          else if  ( ev->data.note.note == CTRL_BT_TRACK_7  ) {
             mapVal = FORCE_BT_NOTE ;
          }

          //  Device / Menu / Matrix
          else if  ( ev->data.note.note == CTRL_BT_TRACK_8  ) {
             mapVal = CtrlShiftMode ? FORCE_BT_MENU:FORCE_BT_MATRIX ;
          }

          // REC
          else if  ( ev->data.note.note == CTRL_BT_RECORD  ) {
            mapVal = FORCE_BT_REC ;
          }

          // PLAY
          else if  ( ev->data.note.note == CTRL_BT_PLAY  ) {
            mapVal = CtrlShiftMode ? FORCE_BT_STOP : FORCE_BT_PLAY ;
          }

          // Mute mode key
          // "Stop all clips" is used to manage solo modes
          // Shift => Knobs button

          else if  ( ev->data.note.note == CTRL_BT_STOP_ALL_CLIPS  ) {
            if ( CtrlShiftMode) mapVal = FORCE_BT_KNOBS ;
            else {
              ColumnsPadMode = ( ev->data.note.velocity == 0x7F ) || ColumnsPadModeLocked ;
              //tklog_debug("Column mode => %s \n", ColumnsPadMode ? "True":"False");
              ControllerRefreshColumnsPads(ColumnsPadMode);
              return false;
            }
          }

          // Launch 5 / STOP ALL In COLUMN MODE
          else if  ( ev->data.note.note == CTRL_BT_LAUNCH_5  ) {
            mapVal = ColumnsPadMode ? FORCE_BT_STOP_ALL : FORCE_BT_LAUNCH_5;
          }

          // Launch 4 / REC ARM
          else if  ( ev->data.note.note == CTRL_BT_LAUNCH_4  ) {
            if ( ColumnsPadMode ) {
              CurrentSoloMode = FORCE_SM_REC_ARM ;
              mapVal = SoloModeButtonMap[CurrentSoloMode];
            }
            else  mapVal = FORCE_BT_LAUNCH_4 ;
          }

          // Launch 3 / Mute
          else if  ( ev->data.note.note == CTRL_BT_LAUNCH_3  ) {
            if ( ColumnsPadMode ) {
              CurrentSoloMode = FORCE_SM_MUTE ;
              mapVal = SoloModeButtonMap[CurrentSoloMode];
            }
            else  mapVal = FORCE_BT_LAUNCH_3 ;
          }

          // Launch 2 / Solo
          else if  ( ev->data.note.note == CTRL_BT_LAUNCH_2  ) {
            if ( ColumnsPadMode ) {
              CurrentSoloMode = FORCE_SM_SOLO ;
              mapVal = SoloModeButtonMap[CurrentSoloMode];
            }
            else  mapVal = FORCE_BT_LAUNCH_2 ;
          }

          // Launch 1 / Clip Stop
          else if  ( ev->data.note.note == CTRL_BT_LAUNCH_1  ) {
            if ( ColumnsPadMode ) {
              CurrentSoloMode = FORCE_SM_CLIP_STOP ;
              mapVal = SoloModeButtonMap[CurrentSoloMode];
            }
            else  mapVal = FORCE_BT_LAUNCH_1 ;
          }

          if ( mapVal >= 0 ) {
                ev->data.note.note = mapVal;
                // Do note on even if note off as Force use only velocity and Note On
                ev->type = SND_SEQ_EVENT_NOTEON;
          }
        }
        else {
          // APC Key 25 pads below 0x28

          // If controller column mode, simulate the columns and mutes pads
          if ( ColumnsPadMode) {

              if ( ev->type == SND_SEQ_EVENT_KEYPRESS ) return false;

              uint8_t padF = ControllerGetForcePadIndex(ev->data.note.note);
              uint8_t padFL = ev->data.note.note  / 8 ;
              uint8_t padFC = ev->data.note.note  % 8 ;

              // Edit current track
              if ( padFL == CTRL_PAD_MAX_LINE - 1 ) {
                snd_seq_event_t ev2 = *ev;

                ev2.data.note.note = FORCE_BT_EDIT ;
                ev2.data.note.velocity  = ( ev->data.note.velocity > 0 ? 0x7F:00);
                ev->data.note.note = FORCE_BT_COLUMN_PAD1 + padFC ;
                ev->data.note.velocity = ev2.data.note.velocity;

                if ( ev2.data.note.velocity == 0x7F ) {
                  SendMidiEvent(&ev2); // Send Edit Press
                  SendMidiEvent(ev); // Send Pad On
                  ev->data.note.velocity = 0 ;
                  SendMidiEvent(ev); // Send Pad off
                }
                else {
                  SendMidiEvent(&ev2); // Send Edit Off
                }
                return false;
              }

              else if ( padFL == 1 ) { // Column pads
                ev->data.note.note = FORCE_BT_COLUMN_PAD1 + padFC;
                ev->data.note.velocity == ( ev->data.note.velocity > 0 ? 0x7F:00);

              }
              else if ( padFL == 0 ) { // Mute mode pads
                ev->data.note.note = FORCE_BT_MUTE_PAD1 + padFC;
                ev->data.note.velocity == ( ev->data.note.velocity > 0 ? 0x7F:00);
              }
          }
          else
            {
              ev->data.note.channel = 9; // Simulate Note Force pad
              ev->data.note.note = ControllerGetForcePadNote(ev->data.note.note);
            }
        }
      }
  }

  return true;
}
