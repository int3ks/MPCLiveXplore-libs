/*
__ __| |           |  /_) |     ___|             |           |
  |   __ \   _ \  ' /  | |  / |      _ \ __ \   |      _` | __ \   __|
  |   | | |  __/  . \  |   <  |   |  __/ |   |  |     (   | |   |\__ \
 _|  _| |_|\___| _|\_\_|_|\_\\____|\___|_|  _| _____|\__,_|_.__/ ____/
-----------------------------------------------------------------------------
TKGL_MIDIMAPPER  custom mapping library .
This is a custom midi mapping library for TKGL_MIDIMAPPER LD_PRELOAD library

----------------------------------------------------------------------------
LUMI Keys BLOCK MIDI 1
----------------------------------------------------------------------------

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
*/

#define IAMFORCE_DRIVER_VERSION "1.0"
#define IAMFORCE_DRIVER_ID "LUMI"
#define IAMFORCE_DRIVER_NAME "LUMI Keys"

#define IAMFORCE_ALSASEQ_DEFAULT_CLIENT_NAME "LUMI Keys BLOCK MIDI 1"

#define IAMFORCE_ALSASEQ_DEFAULT_PORT 0


///////////////////////////////////////////////////////////////////////////////
// Controller initialization
///////////////////////////////////////////////////////////////////////////////
static int ControllerInitialize() {

    tklog_info("IamForce : %s implementation, version %s.\n", IAMFORCE_DRIVER_NAME, IAMFORCE_DRIVER_VERSION);

}
//{force,mpc(lumi)}
static mapping mpcPadForceStepmodeLUMImapping[] = {
    {110,36},  {111,37},  {112,38},  {113,39},
    {102,40},  {103,41},  {104,42},  {105,43},
    {94,44},   {95,45},   {96,46},   {97,47},
    {86,48},   {87,49},   {88,50},   {89,51},
};

static mapping lumiToMute[] = {
    {FORCE_BT_MUTE_PAD5,43}, {FORCE_BT_MUTE_PAD6,45}, {FORCE_BT_MUTE_PAD7,47}, {FORCE_BT_MUTE_PAD8,48},
    {FORCE_BT_MUTE_PAD1,36}, {FORCE_BT_MUTE_PAD2,38}, {FORCE_BT_MUTE_PAD3,40}, {FORCE_BT_MUTE_PAD4,41}
};

///////////////////////////////////////////////////////////////////////////////
// Process an event received from the Launchpad
///////////////////////////////////////////////////////////////////////////////
static bool ControllerEventReceived(snd_seq_event_t *ev) {

  switch (ev->type) {
    case SND_SEQ_EVENT_SYSEX:
        break;

    case SND_SEQ_EVENT_CONTROLLER:
        break;

    case SND_SEQ_EVENT_NOTEON:
    case SND_SEQ_EVENT_NOTEOFF:
    case SND_SEQ_EVENT_KEYPRESS:
      if ( ev->data.note.channel == 0 ) {
        // LumiKeys
          if (currentPadMode == FORCE_BT_STEP_SEQ){// || currentPadMode == FORCE_BT_NOTE) {

              uint8_t ForcePadNote = 0;
              if (currentPadMode == FORCE_BT_STEP_SEQ) {
                  while (ev->data.note.note > 51)ev->data.note.note -= 24;
                  while (ev->data.note.note < 36)ev->data.note.note += 24;
                  ForcePadNote = mapForceNote(mpcPadForceStepmodeLUMImapping, ev->data.note.note, 16);
              }
              if (ForcePadNote == 0) {
                 // tklog_debug("ForcePadNote is null for %d, return;\n", ev->data.note.note);
                  return false;
              }
              //tklog_debug("Controller Event received %d note %d forcenote %d\n", currentPadMode, ev->data.note.note, ForcePadNote);
              ev->data.note.note = ForcePadNote;
              SetMidiEventDestination(ev, TO_MPC_PRIVATE);
          }
          else {
              // tklog_debug("send to TO_MPC_EXTCTRL;\n");
              SetMidiEventDestination(ev, TO_MPC_EXTCTRL);
              SendMidiEvent(ev);
              return false;
          }
          ev->data.note.channel = 9; // Note Force pad

      }
  }

  return true;
}
static void ControllerSetPadColorRGB(uint8_t padCt, uint8_t r, uint8_t g, uint8_t b) { }

static void ControllerRefreshColumnsPads(bool show) { }

static int ControllerGetPadIndex(uint8_t padF) { return  -1; }

static void ControllerSetMapButtonLed(snd_seq_event_t* ev) { }