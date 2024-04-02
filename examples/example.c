#include "example.h"                        // Include custom header file

#include <time.h>                           // Time-related functions
#include <sys/types.h>                      // System types
#include <sys/stat.h>                       // System statistics
#include <unistd.h>                         // Symbolic constants
#include <fcntl.h>                          // File control options
#include <stdlib.h>                         // Standard library functions

#include "../lib/e-Paper/EPD_IT8951.h"      // e-Paper library header
#include "../lib/GUI/GUI_Paint.h"            // GUI painting library header
#include "../lib/GUI/GUI_BMPfile.h"          // BMP file handling library header
#include "../lib/Config/Debug.h"             // Debug configuration header

// Global variables for frame buffers
UBYTE *Refresh_Frame_Buf = NULL;            // Refresh frame buffer
UBYTE *Panel_Frame_Buf = NULL;              // Panel frame buffer
UBYTE *Panel_Area_Frame_Buf = NULL;         // Panel area frame buffer

bool Four_Byte_Align = false;               // Flag for four-byte alignment

/******************************************************************************
Function to demonstrate 4bpp palette example
function: Display_ColorPalette_Example
parameter:
    Panel_Width: Width of the panel
    Panel_Height: Height of the panel
    Init_Target_Memory_Addr: Memory address of IT8951 target memory address
******************************************************************************/
UBYTE Display_ColorPalette_Example(UWORD Panel_Width, UWORD Panel_Height, UDOUBLE Init_Target_Memory_Addr){
    UWORD In_4bp_Refresh_Area_Width;
    if(Four_Byte_Align == true){
        In_4bp_Refresh_Area_Width = Panel_Width - (Panel_Width % 32);
    }else{
        In_4bp_Refresh_Area_Width = Panel_Width;
    }
    UWORD In_4bp_Refresh_Area_Height = Panel_Height/16;

    UDOUBLE Imagesize;

    clock_t In_4bp_Refresh_Start, In_4bp_Refresh_Finish;
    double In_4bp_Refresh_Duration;

    Imagesize  = ((In_4bp_Refresh_Area_Width*4 % 8 == 0)? (In_4bp_Refresh_Area_Width*4 / 8 ): (In_4bp_Refresh_Area_Width*4 / 8 + 1)) * In_4bp_Refresh_Area_Height;

    if((Refresh_Frame_Buf = (UBYTE *)malloc(Imagesize)) == NULL) {
        Debug("Failed to apply for black memory...\r\n");
        return -1;
    }

    Debug("Start to demostrate 4bp palette example\r\n");
    In_4bp_Refresh_Start = clock();

    UBYTE SixteenColorPattern[16] = {0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00};

    for(int i=0; i < 16; i++){
        memset(Refresh_Frame_Buf, SixteenColorPattern[i], Imagesize);
        EPD_IT8951_4bp_Refresh(Refresh_Frame_Buf, 0, i * In_4bp_Refresh_Area_Height, In_4bp_Refresh_Area_Width, In_4bp_Refresh_Area_Height, false, Init_Target_Memory_Addr, false);
    }

    In_4bp_Refresh_Finish = clock();
    In_4bp_Refresh_Duration = (double)(In_4bp_Refresh_Finish - In_4bp_Refresh_Start) / CLOCKS_PER_SEC;
    Debug( "Write and Show 4bp occupy %f second\n", In_4bp_Refresh_Duration );

    if(Refresh_Frame_Buf != NULL){
        free(Refresh_Frame_Buf);
        Refresh_Frame_Buf = NULL;
    }
    return 0;
}


/******************************************************************************
function: Display_CharacterPattern_Example
parameter:
    Panel_Width: Width of the panel
    Panel_Height: Height of the panel
    Init_Target_Memory_Addr: Memory address of IT8951 target memory address
    BitsPerPixel: Bits Per Pixel, 2^BitsPerPixel = grayscale
******************************************************************************/
UBYTE Display_CharacterPattern_Example(UWORD Panel_Width, UWORD Panel_Height, UDOUBLE Init_Target_Memory_Addr, UBYTE BitsPerPixel){
    UWORD Display_Area_Width;
    if(Four_Byte_Align == true){
        Display_Area_Width = Panel_Width - (Panel_Width % 32);
    }else{
        Display_Area_Width = Panel_Width;
    }
    UWORD Display_Area_Height = Panel_Height;

    UWORD Display_Area_Sub_Width = Display_Area_Width / 5;
    UWORD Display_Area_Sub_Height = Display_Area_Height / 5;

    UDOUBLE Imagesize;

    Imagesize = ((Display_Area_Width * BitsPerPixel % 8 == 0)? (Display_Area_Width * BitsPerPixel / 8 ): (Display_Area_Width * BitsPerPixel / 8 + 1)) * Display_Area_Height;
    if((Refresh_Frame_Buf = (UBYTE *)malloc(Imagesize)) == NULL) {
        Debug("Failed to apply for image memory...\r\n");
        return -1;
    }

    Paint_NewImage(Refresh_Frame_Buf, Display_Area_Width, Display_Area_Height, 0, BLACK);
    Paint_SelectImage(Refresh_Frame_Buf);
    Paint_SetBitsPerPixel(BitsPerPixel);
    Paint_Clear(WHITE);
    

    for(int y=20; y<Display_Area_Height - Display_Area_Sub_Height; y += Display_Area_Sub_Height )//To prevent arrays from going out of bounds
    {
        for(int  x=20; x<Display_Area_Width - Display_Area_Sub_Width; x += Display_Area_Sub_Width )//To prevent arrays from going out of bounds
        {
            //For color definition of all BitsPerPixel, you can refer to GUI_Paint.h
            Paint_DrawPoint(x+Display_Area_Sub_Width*3/8, y+Display_Area_Sub_Height*3/8, 0x10, DOT_PIXEL_7X7, DOT_STYLE_DFT);
            Paint_DrawPoint(x+Display_Area_Sub_Width*5/8, y+Display_Area_Sub_Height*3/8, 0x30, DOT_PIXEL_7X7, DOT_STYLE_DFT);
            Paint_DrawLine(x+Display_Area_Sub_Width*3/8, y+Display_Area_Sub_Height*5/8, x+Display_Area_Sub_Width*5/8, y+Display_Area_Sub_Height*5/8, 0x50, DOT_PIXEL_3X3, LINE_STYLE_SOLID);
            Paint_DrawRectangle(x, y, x+Display_Area_Sub_Width, y+Display_Area_Sub_Height, 0x00, DOT_PIXEL_3X3, DRAW_FILL_EMPTY);
            Paint_DrawCircle(x + Display_Area_Sub_Width/2, y + Display_Area_Sub_Height/2, Display_Area_Sub_Height/2, 0x50, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
            Paint_DrawNum(x+Display_Area_Sub_Width*3/10, y+Display_Area_Sub_Height*1/4, 1234567890, &Font16, 0x20, 0xE0);
            Paint_DrawString_EN(x+Display_Area_Sub_Width*3/10, y+Display_Area_Sub_Height*3/4, "hello world", &Font16, 0x30, 0xD0);
        }
    }


    switch(BitsPerPixel){
        case BitsPerPixel_8:{
            EPD_IT8951_8bp_Refresh(Refresh_Frame_Buf, 0, 0, Display_Area_Width,  Display_Area_Height, false, Init_Target_Memory_Addr);
            break;
        }
        case BitsPerPixel_4:{
            EPD_IT8951_4bp_Refresh(Refresh_Frame_Buf, 0, 0, Display_Area_Width,  Display_Area_Height, false, Init_Target_Memory_Addr,false);
            break;
        }
        case BitsPerPixel_2:{
            EPD_IT8951_2bp_Refresh(Refresh_Frame_Buf, 0, 0, Display_Area_Width,  Display_Area_Height, false, Init_Target_Memory_Addr,false);
            break;
        }
        case BitsPerPixel_1:{
            EPD_IT8951_1bp_Refresh(Refresh_Frame_Buf, 0, 0, Display_Area_Width,  Display_Area_Height, A2_Mode, Init_Target_Memory_Addr,false);
            break;
        }
    }

    if(Refresh_Frame_Buf != NULL){
        free(Refresh_Frame_Buf);
        Refresh_Frame_Buf = NULL;
    }

    return 0;
}



/******************************************************************************
function: Display_BMP_Example
parameter:
    Panel_Width: Width of the panel
    Panel_Height: Height of the panel
    Init_Target_Memory_Addr: Memory address of IT8951 target memory address
    BitsPerPixel: Bits Per Pixel, 2^BitsPerPixel = grayscale
******************************************************************************/
UBYTE Display_BMP_Example(UWORD Panel_Width, UWORD Panel_Height, UDOUBLE Init_Target_Memory_Addr, UBYTE BitsPerPixel){
    UWORD WIDTH;
    if(Four_Byte_Align == true){
        WIDTH  = Panel_Width - (Panel_Width % 32);             // Adjust width for four-byte alignment
    }else{
        WIDTH = Panel_Width;
    }
    UWORD HEIGHT = Panel_Height;

    UDOUBLE Imagesize;

    Imagesize = ((WIDTH * BitsPerPixel % 8 == 0)? (WIDTH * BitsPerPixel / 8 ): (WIDTH * BitsPerPixel / 8 + 1)) * HEIGHT;   // Calculate image size
    if((Refresh_Frame_Buf = (UBYTE *)malloc(Imagesize)) == NULL) {
        Debug("Failed to apply for black memory...\r\n");      // Allocate memory for frame buffer
        return -1;
    }

    Paint_NewImage(Refresh_Frame_Buf, WIDTH, HEIGHT, 0, BLACK); // Create a new image with specified properties
    Paint_SelectImage(Refresh_Frame_Buf);                      // Select the image for painting
    Paint_SetBitsPerPixel(BitsPerPixel);                       // Set bits per pixel for painting
    Paint_Clear(WHITE);                                        // Clear the image with white color

    char Path[30];
    sprintf(Path,"./pic/%dx%d_0.bmp", WIDTH, HEIGHT);          // Set the path to the BMP file
    Debug("Selected image file: %s\r\n", Path);         // Print the selected image file path to debug

    GUI_ReadBmp(Path, 0, 0);                                   // Read the BMP file into the image

    //you can draw your character and pattern on the image, for color definition of all BitsPerPixel, you can refer to GUI_Paint.h, 
    //Paint_DrawRectangle(50, 50, WIDTH/2, HEIGHT/2, 0x30, DOT_PIXEL_3X3, DRAW_FILL_EMPTY);
    //Paint_DrawCircle(WIDTH*3/4, HEIGHT/4, 100, 0xF0, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    //Paint_DrawNum(WIDTH/4, HEIGHT/5, 709, &Font20, 0x30, 0xB0);

    switch(BitsPerPixel){
        case BitsPerPixel_8:{                                  // Handle different bits per pixel cases
            Paint_DrawString_EN(10, 10, "8 bits per pixel 16 grayscale", &Font24, 0xF0, 0x00);  // Draw text on the image
            EPD_IT8951_8bp_Refresh(Refresh_Frame_Buf, 0, 0, WIDTH,  HEIGHT, false, Init_Target_Memory_Addr);   // Refresh the display with the image
            break;
        }
        case BitsPerPixel_4:{
            Paint_DrawString_EN(10, 10, "4 bits per pixel 16 grayscale", &Font24, 0xF0, 0x00);
            EPD_IT8951_4bp_Refresh(Refresh_Frame_Buf, 0, 0, WIDTH,  HEIGHT, false, Init_Target_Memory_Addr,false);
            break;
        }
        case BitsPerPixel_2:{
            Paint_DrawString_EN(10, 10, "2 bits per pixel 4 grayscale", &Font24, 0xC0, 0x00);
            EPD_IT8951_2bp_Refresh(Refresh_Frame_Buf, 0, 0, WIDTH,  HEIGHT, false, Init_Target_Memory_Addr,false);
            break;
        }
        case BitsPerPixel_1:{
            Paint_DrawString_EN(10, 10, "1 bit per pixel 2 grayscale", &Font24, 0x80, 0x00);
            EPD_IT8951_1bp_Refresh(Refresh_Frame_Buf, 0, 0, WIDTH,  HEIGHT, A2_Mode, Init_Target_Memory_Addr,false);
            break;
        }
    }

    if(Refresh_Frame_Buf != NULL){
        free(Refresh_Frame_Buf);                               // Free the frame buffer memory
        Refresh_Frame_Buf = NULL;
    }

    DEV_Delay_ms(5000);                                        // Delay for 5 seconds

    return 0;
}


/******************************************************************************
function: Dynamic_Refresh_Example
parameter:
    Dev_Info: Information structure read from IT8951
    Init_Target_Memory_Addr: Memory address of IT8951 target memory address
******************************************************************************/
UBYTE Dynamic_Refresh_Example(IT8951_Dev_Info Dev_Info, UDOUBLE Init_Target_Memory_Addr){
    UWORD Panel_Width = Dev_Info.Panel_W;                   // Get panel width from device info
    UWORD Panel_Height = Dev_Info.Panel_H;                  // Get panel height from device info

    UWORD Dynamic_Area_Width = 96;                           // Initialize dynamic area width
    UWORD Dynamic_Area_Height = 48;                          // Initialize dynamic area height

    UDOUBLE Imagesize;

    UWORD Start_X = 0, Start_Y = 0;                          // Initialize starting X and Y coordinates

    UWORD Dynamic_Area_Count = 0;

    UWORD Repeat_Area_Times = 0;

    // Allocate memory for 1bp picture
    Imagesize = ((Panel_Width * 1 % 8 == 0)? (Panel_Width * 1 / 8 ): (Panel_Width * 1 / 8 + 1)) * Panel_Height;
    if((Refresh_Frame_Buf = (UBYTE *)malloc(Imagesize)) == NULL){
        Debug("Failed to apply for picture memory...\r\n");
        return -1;
    }

    clock_t Dynamic_Area_Start, Dynamic_Area_Finish;
    double Dynamic_Area_Duration;  

    while(1)
    {
        Dynamic_Area_Width = 128;
        Dynamic_Area_Height = 96;

        Start_X = 0;
        Start_Y = 0;

        Dynamic_Area_Count = 0;

        Dynamic_Area_Start = clock();
        Debug("Start to dynamic display...\r\n");

        // Iterate over dynamic area widths and heights
        for(Dynamic_Area_Width = 96, Dynamic_Area_Height = 64; (Dynamic_Area_Width < Panel_Width - 32) && (Dynamic_Area_Height < Panel_Height - 24); Dynamic_Area_Width += 32, Dynamic_Area_Height += 24)
        {
            // Calculate image size and allocate memory
            Imagesize = ((Dynamic_Area_Width % 8 == 0)? (Dynamic_Area_Width / 8 ): (Dynamic_Area_Width / 8 + 1)) * Dynamic_Area_Height;
            Paint_NewImage(Refresh_Frame_Buf, Dynamic_Area_Width, Dynamic_Area_Height, 0, BLACK);
            Paint_SelectImage(Refresh_Frame_Buf);
            Paint_SetBitsPerPixel(1);

            // Iterate over start Y positions
            for(int y=Start_Y; y< Panel_Height - Dynamic_Area_Height; y += Dynamic_Area_Height)
            {
                // Iterate over start X positions
                for(int x=Start_X; x< Panel_Width - Dynamic_Area_Width; x += Dynamic_Area_Width)
                {
                    Paint_Clear(WHITE);

                    // Draw rectangle, circle, and number on the image
                    Paint_DrawRectangle(0, 0, Dynamic_Area_Width-1, Dynamic_Area_Height, 0x00, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
                    Paint_DrawCircle(Dynamic_Area_Width*3/4, Dynamic_Area_Height*3/4, 5, 0x00, DOT_PIXEL_1X1, DRAW_FILL_FULL);
                    Paint_DrawNum(Dynamic_Area_Width/4, Dynamic_Area_Height/4, ++Dynamic_Area_Count, &Font20, 0x00, 0xF0);

                    // Refresh the display
                    EPD_IT8951_1bp_Refresh(Refresh_Frame_Buf, x, y, Dynamic_Area_Width,  Dynamic_Area_Height, A2_Mode, Init_Target_Memory_Addr, true);
                }
            }
            Start_X += 32;
            Start_Y += 24;
        }

        Dynamic_Area_Finish = clock();
        Dynamic_Area_Duration = (double)(Dynamic_Area_Finish - Dynamic_Area_Start) / CLOCKS_PER_SEC;
        Debug( "Write and Show occupy %f second\n", Dynamic_Area_Duration );

        Repeat_Area_Times ++;
        if(Repeat_Area_Times > 0){
            break;
        }
    }
    if(Refresh_Frame_Buf != NULL){
        free(Refresh_Frame_Buf);
        Refresh_Frame_Buf = NULL;
    }

    return 0;
}


/******************************************************************************
function: Dynamic_GIF_Example
parameter:
    Panel_Width: Width of the panel
    Panel_Height: Height of the panel
    Init_Target_Memory_Addr: Memory address of IT8951 target memory address
    BitsPerPixel: Bits Per Pixel, 2^BitsPerPixel = grayscale
******************************************************************************/
UBYTE Dynamic_GIF_Example(UWORD Panel_Width, UWORD Panel_Height, UDOUBLE Init_Target_Memory_Addr){

    // Set initial parameters for animation area
    UWORD Animation_Start_X = 0;
    UWORD Animation_Start_Y = 0;
    UWORD Animation_Area_Width = 800;
    UWORD Animation_Area_Height = 600;

    // Check if animation area exceeds panel dimensions
    if(Animation_Area_Width > Panel_Width){
        return -1;
    }
    if(Animation_Area_Height > Panel_Height){
        return -1;
    }

    UDOUBLE Imagesize;

    UBYTE Pic_Count = 0;
    UBYTE Pic_Num = 7;
    char Path[30];

    UDOUBLE Basical_Memory_Addr = Init_Target_Memory_Addr;

    UDOUBLE Target_Memory_Addr = Basical_Memory_Addr;
    UWORD Repeat_Animation_Times = 0;

    clock_t Animation_Test_Start, Animation_Test_Finish;
    double Animation_Test_Duration;

    // Calculate image size for animation area
    Imagesize = ((Animation_Area_Width * 1 % 8 == 0)? (Animation_Area_Width * 1 / 8 ): (Animation_Area_Width * 1 / 8 + 1)) * Animation_Area_Height;

    // Allocate memory for image
    if((Refresh_Frame_Buf = (UBYTE *)malloc(Imagesize)) == NULL){
        Debug("Failed to apply for image memory...\r\n");
        return -1;
    }

    // Initialize image for animation
    Paint_NewImage(Refresh_Frame_Buf, Animation_Area_Width, Animation_Area_Height, 0, BLACK);
    Paint_SelectImage(Refresh_Frame_Buf);
    Paint_SetBitsPerPixel(1);

    Debug("Start to write an animation\r\n");
    Animation_Test_Start = clock();

    // Iterate through images in the GIF
    for(int i=0; i < Pic_Num; i += 1){
        Paint_Clear(WHITE);
        sprintf(Path,"./pic/800x600_gif_%d.bmp",Pic_Count++);
        GUI_ReadBmp(Path, 0, 0);
        Paint_DrawNum(10, 10, i+1, &Font16, 0x00, 0xF0);

		// Refresh the display with the current image
		EPD_IT8951_1bp_Multi_Frame_Write(Refresh_Frame_Buf, Animation_Start_X, Animation_Start_Y, Animation_Area_Width,  Animation_Area_Height, Target_Memory_Addr,false);
        Target_Memory_Addr += Imagesize;
    }

    // Calculate and print the duration of writing the animation
    Animation_Test_Finish = clock();
    Animation_Test_Duration = (double)(Animation_Test_Finish - Animation_Test_Start) / CLOCKS_PER_SEC;
	Debug( "Write all frames occupy %f second\r\n", Animation_Test_Duration);

    // Reset target memory address
    Target_Memory_Addr = Basical_Memory_Addr;

    // Loop to display the animation
    while(1){
        Debug("Start to show an animation\r\n");
        Animation_Test_Start = clock();

        // Display all frames of the animation
        for(int i=0; i< Pic_Num; i += 1){
			EPD_IT8951_1bp_Multi_Frame_Refresh(Animation_Start_X, Animation_Start_Y, Animation_Area_Width,  Animation_Area_Height, Target_Memory_Addr);
            Target_Memory_Addr += Imagesize;
        }
        Target_Memory_Addr = Basical_Memory_Addr;

        // Calculate and print the duration of displaying the animation
        Animation_Test_Finish = clock();
        Animation_Test_Duration = (double)(Animation_Test_Finish - Animation_Test_Start) / CLOCKS_PER_SEC;
        Debug( "Show all frames occupy %f second\r\n", Animation_Test_Duration );
   		Debug( "The frame rate is: %lf fps\r\n", Pic_Num/Animation_Test_Duration);

        Repeat_Animation_Times ++;
        if(Repeat_Animation_Times >15){
            break;
        }
    }

    // Free allocated memory
    if(Refresh_Frame_Buf != NULL){
        free(Refresh_Frame_Buf);
        Refresh_Frame_Buf = NULL;
    }

    return 0;
}


/******************************************************************************
function: Check_FrameRate_Example
parameter:
    Panel_Width: Width of the panel
    Panel_Height: Height of the panel
    Init_Target_Memory_Addr: Memory address of IT8951 target memory address
    BitsPerPixel: Bits Per Pixel, 2^BitsPerPixel = grayscale
******************************************************************************/
UBYTE Check_FrameRate_Example(UWORD Panel_Width, UWORD Panel_Height, UDOUBLE Target_Memory_Addr, UBYTE BitsPerPixel){
    UWORD Frame_Rate_Test_Width;
    // Adjust width for four-byte alignment
    if(Four_Byte_Align == true){
        Frame_Rate_Test_Width = Panel_Width - (Panel_Width % 32);
    }else{
        Frame_Rate_Test_Width = Panel_Width;
    }
    UWORD Frame_Rate_Test_Height = Panel_Height;
    UDOUBLE Imagesize;

    UBYTE *Refresh_FrameRate_Buf = NULL;

    UBYTE Count = 0;

    clock_t Frame_Rate_Test_Start, Frame_Rate_Test_Finish;
    double Frame_Rate_Test_Duration;

    // Calculate image size based on width and bits per pixel
    Imagesize = ((Frame_Rate_Test_Width * BitsPerPixel % 8 == 0)? (Frame_Rate_Test_Width * BitsPerPixel / 8 ): (Frame_Rate_Test_Width * BitsPerPixel / 8 + 1)) * Frame_Rate_Test_Height;

    // Allocate memory for image
    if((Refresh_FrameRate_Buf = (UBYTE *)malloc(Imagesize)) == NULL) {
        Debug("Failed to apply for image memory...\r\n");
        return -1;
    }

    // Initialize image
    Paint_NewImage(Refresh_FrameRate_Buf, Frame_Rate_Test_Width, Frame_Rate_Test_Height, 0, BLACK);
    Paint_SelectImage(Refresh_FrameRate_Buf);
    Paint_SetBitsPerPixel(BitsPerPixel);

    Debug("Start to test Frame Rate\r\n");
    Frame_Rate_Test_Start = clock();

    // Display 10 frames
    for(int i=0; i<10; i++){
        Paint_Clear(WHITE);
        Paint_DrawRectangle(20, 20, Frame_Rate_Test_Width-20, Frame_Rate_Test_Height-20, 0x00, DOT_PIXEL_4X4, DRAW_FILL_EMPTY);
        Paint_DrawNum(Frame_Rate_Test_Width/2, Frame_Rate_Test_Height/2, ++Count, &Font24, 0x00, 0xF0);
        Paint_DrawString_EN(Frame_Rate_Test_Width/2, Frame_Rate_Test_Height/4, "frame rate test", &Font20, 0xF0, 0x00);
        Paint_DrawString_EN(Frame_Rate_Test_Width/2, Frame_Rate_Test_Height*3/4, "frame rate test", &Font20, 0xF0, 0x00);

        // Refresh the display with the current frame
        switch(BitsPerPixel){
            case 8:{
				EPD_IT8951_8bp_Refresh(Refresh_FrameRate_Buf, 0, 0, Frame_Rate_Test_Width,  Frame_Rate_Test_Height, false, Target_Memory_Addr);
                break;
            }
            case 4:{
				EPD_IT8951_4bp_Refresh(Refresh_FrameRate_Buf, 0, 0, Frame_Rate_Test_Width,  Frame_Rate_Test_Height, false, Target_Memory_Addr,false);
                break;
            }
            case 2:{
				EPD_IT8951_2bp_Refresh(Refresh_FrameRate_Buf, 0, 0, Frame_Rate_Test_Width,  Frame_Rate_Test_Height, false, Target_Memory_Addr,false);
                break;
            }
            case 1:{
				EPD_IT8951_1bp_Refresh(Refresh_FrameRate_Buf, 0, 0, Frame_Rate_Test_Width,  Frame_Rate_Test_Height, A2_Mode, Target_Memory_Addr,false);
                break;
            }
        }
    }

    // Calculate and print the duration and frame rate of displaying the frames
    Frame_Rate_Test_Finish = clock();
    Frame_Rate_Test_Duration = (double)(Frame_Rate_Test_Finish - Frame_Rate_Test_Start) / CLOCKS_PER_SEC;
	Debug( "Write and Show 10 Frames occupy %f second\r\n", Frame_Rate_Test_Duration);
    Debug( "The frame rate is: %lf fps\r\n", 10/Frame_Rate_Test_Duration);
    Debug( "All Tests have completed successfully\r\n");

    // Free allocated memory
    if(Refresh_FrameRate_Buf != NULL){
        free(Refresh_FrameRate_Buf);
        Refresh_FrameRate_Buf = NULL;
    }

    return 0;
}