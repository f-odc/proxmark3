//-----------------------------------------------------------------------------
// Copyright (C) Brad Antoniewicz 2011
// Copyright (C) Proxmark3 contributors. See AUTHORS.md for details.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// See LICENSE.txt for the text of the license.
//-----------------------------------------------------------------------------
// main code for LF aka Proxbrute by Brad antoniewicz
//-----------------------------------------------------------------------------
#include "standalone.h" // standalone definitions
#include "proxmark3_arm.h"
#include "appmain.h"
#include "fpgaloader.h"
#include "util.h"
#include "dbprint.h"
//
#include "ticks.h"
#include "lfops.h"
//
#include "mifaresim.h"
//
#include "stdint.h"

void ModInfo(void)
{
    DbpString("  HF Bruteforce UID (Felix Op den Camp)");
}

// samy's sniff and repeat routine for LF
void RunMod(void)
{
    StandAloneMode();
    Dbprintf(">>  HF Bruteforce UID - (Felix Op den Camp) <<");
    FpgaDownloadAndGo(FPGA_BITSTREAM_HF);

    uint8_t uid[] = {0x00, 0x00, 0x00, 0x00};

#define STATE_READ 0
#define STATE_BRUTE 1

    // main loop
    for (;;)
    {

        WDT_HIT();

        // exit from SamyRun,   send a usbcommand.
        if (data_available())
            break;

        // Was our button held down or pressed?
        int button_pressed = BUTTON_HELD(1000);
        if (button_pressed != BUTTON_NO_CLICK)
            break;

        /*
        Emulate Mifare 1K Card
        @datain = UID
        @flag = indicates size of UID
        */
        RothultSim(FLAG_4B_UID_IN_DATA, 20, uid, 00, 0);

        // increase uid
        if (uid[3] == 0xFF)
        {
            uid[3] = 0x00;
            if (uid[2] == 0xFF)
            {
                uid[2] = 0x00;
                if (uid[1] == 0xFF)
                {
                    uid[1] = 0x00;
                    if (uid[0] == 0xFF)
                    {
                        break;
                    }
                    else
                        uid[0]++;
                }
                else
                    uid[1]++;
            }
            else
                uid[2]++;
        }
        else
            uid[3]++;
    }

    DbpString("[=] You can take the shell back :) ...");
    LEDsoff();
}
