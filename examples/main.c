#include "../lib/Config/DEV_Config.h"    // Device configuration header
#include "example.h"                      // Custom example header
#include "../lib/GUI/GUI_BMPfile.h"        // BMP file handling header

#include <math.h>                         // Math functions
#include <stdlib.h>                       // Standard library functions (e.g., exit())
#include <signal.h>                       // Signal handling functions

#define Enhance false                    // Flag for enhanced driving ability
#define USE_Normal_Demo true             // Flag for demo code

UWORD VCOM = 1860;                       // VCOM value (-1.86 * 1000)
IT8951_Dev_Info Dev_Info = {0, 0};        // Device information
UWORD Panel_Width;                        // Panel width
UWORD Panel_Height;                       // Panel height
UDOUBLE Init_Target_Memory_Addr;          // Initial target memory address

// Signal handler for Ctrl+C
void Handler(int signo){
    Debug("\r\nHandler:exit\r\n");        // Debug message
    // Free allocated memory
    if(Refresh_Frame_Buf != NULL){
        free(Refresh_Frame_Buf);
        Debug("free Refresh_Frame_Buf\r\n");
        Refresh_Frame_Buf = NULL;
    }
    if(Panel_Frame_Buf != NULL){
        free(Panel_Frame_Buf);
        Debug("free Panel_Frame_Buf\r\n");
        Panel_Frame_Buf = NULL;
    }
    if(Panel_Area_Frame_Buf != NULL){
        free(Panel_Area_Frame_Buf);
        Debug("free Panel_Area_Frame_Buf\r\n");
        Panel_Area_Frame_Buf = NULL;
    }
    if(bmp_src_buf != NULL){
        free(bmp_src_buf);
        Debug("free bmp_src_buf\r\n");
        bmp_src_buf = NULL;
    }
    if(bmp_dst_buf != NULL){
        free(bmp_dst_buf);
        Debug("free bmp_dst_buf\r\n");
        bmp_dst_buf = NULL;
    }
    if(Dev_Info.Panel_W != 0){
        Debug("Going to sleep\r\n");
        EPD_IT8951_Sleep();               // Enter sleep mode
    }
    DEV_Module_Exit();                    // Clean up and exit
    exit(0);
}

int main(int argc, char *argv[])
{
    // Register signal handler for Ctrl+C
    signal(SIGINT, Handler);

    // Initialize the BCM2835 device
    if(DEV_Module_Init()!=0){
        return -1;
    }

    // Set VCOM and EPD mode
    VCOM = 1860;  // -1.86 * 1000

    Debug("VCOM value:%d\r\n", VCOM);      // Debug message
    Dev_Info = EPD_IT8951_Init(VCOM);      // Initialize EPD

    // Handle enhanced driving ability
#if(Enhance)
    Debug("Attention! Enhanced driving ability, only used when the screen is blurred\r\n");
    Enhance_Driving_Capability();
#endif

    // Get important information from device info
    Panel_Width = Dev_Info.Panel_W;
    Panel_Height = Dev_Info.Panel_H;
    Init_Target_Memory_Addr = Dev_Info.Memory_Addr_L | (Dev_Info.Memory_Addr_H << 16);

    // Determine A2 Mode based on LUT version
    int A2_Mode = 0;  // Default A2 mode
    Debug("A2 Mode:%d\r\n", A2_Mode);      // Debug message

    // Clear the refresh memory
    EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, INIT_Mode);

    // Run demo if enabled
#if(USE_Normal_Demo)
    // Show 16 grayscale
    Display_ColorPalette_Example(Panel_Width, Panel_Height, Init_Target_Memory_Addr);
    EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, GC16_Mode);

    // Show some character and pattern
    Display_CharacterPattern_Example(Panel_Width, Panel_Height, Init_Target_Memory_Addr, BitsPerPixel_4);
    EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, GC16_Mode);

    // Show a bmp file
    Display_BMP_Example(Panel_Width, Panel_Height, Init_Target_Memory_Addr, BitsPerPixel_1);
    Display_BMP_Example(Panel_Width, Panel_Height, Init_Target_Memory_Addr, BitsPerPixel_2);
    Display_BMP_Example(Panel_Width, Panel_Height, Init_Target_Memory_Addr, BitsPerPixel_4);
    EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, GC16_Mode);

    // Show A2 mode refresh effect
    EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, A2_Mode);
    Dynamic_Refresh_Example(Dev_Info,Init_Target_Memory_Addr);
    EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, A2_Mode);
    EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, GC16_Mode);

    // Test frame rate
    Check_FrameRate_Example(800, 600, Init_Target_Memory_Addr, BitsPerPixel_1);
    EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, A2_Mode);
    EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, GC16_Mode);
#endif

    // Final clean-up before exiting
    EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, INIT_Mode);
    EPD_IT8951_Sleep();
    DEV_Delay_ms(5000);
    DEV_Module_Exit();

    return 0;  // Return success
}