#include "encoder.h"
#include "stm32f103xb.h"

unsigned char NewState, OldState, upState, downState;
// unsigned char Vol = 249;


void EncoderScan(void)
{
    NewState = GPIOA->IDR & 0x03;
    if (NewState != OldState) { 
        switch (OldState) {
            case 2:
                {
                if (NewState == 3)upState++;
                if (NewState == 0)downState++;
                break;
                }
            case 0:
                {
                if (NewState == 2)upState++;
                if (NewState == 1)downState++;
                break;
                }
            case 1:
                {
                if (NewState == 0)upState++;
                if (NewState == 3)downState++;
                break;
                }
            case 3:
                {
                if (NewState == 1)upState++;
                if (NewState == 2)downState++;
                break;
                }
        }; 
        OldState = NewState;
        if (upState >= 4) { 
            Vol += 10;
            upState = 0;
        };
        if (downState >= 4) { 
            Vol -= 10;
            downState = 0;
        };
    
    };

}
