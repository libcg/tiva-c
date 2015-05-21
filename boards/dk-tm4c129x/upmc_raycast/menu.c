#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_types.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/container.h"
#include "grlib/pushbutton.h"
#include "grlib/slider.h"
#include "utils/ustdlib.h"
#include "drivers/kentec320x240x16_ssd2119.h"
#include "intro_snd.h"

void OnMenuPlayPress(tWidget *psWidget);
void OnMenuOptionsPress(tWidget *psWidget);
void OnPrevious(tWidget *psWidget);
void OnNext(tWidget *psWidget);
void OnMenuPaint(tWidget *psWidget, tContext *psContext);
void OnOptionsPaint(tWidget *psWidget, tContext *psContext);
extern tCanvasWidget g_panels[];

Canvas(g_menu, g_panels, 0, 0, &g_sKentec320x240x16_SSD2119,
       0, 0, 320, 160,
       CANVAS_STYLE_APP_DRAWN, 0, 0, 0, 0, 0, 0, OnMenuPaint);
RectangularButton(g_menuOptions, g_panels, &g_menu, 0,
                  &g_sKentec320x240x16_SSD2119, 320/2 - 100/2, 90, 100, 30,
                  PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT,
                  ClrMidnightBlue, ClrBlack, ClrGray, ClrSilver,
                  g_psFontCm22, "Options", 0, 0, 0, 0, OnMenuOptionsPress);
RectangularButton(g_menuPlay, g_panels, &g_menuOptions, 0,
                  &g_sKentec320x240x16_SSD2119, 320/2 - 100/2, 60, 100, 30,
                  PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT,
                  ClrMidnightBlue, ClrBlack, ClrGray, ClrSilver,
                  g_psFontCm22, "Jouer", 0, 0, 0, 0, OnMenuPlayPress);

Canvas(g_sOptions, g_panels + 1, 0, 0, &g_sKentec320x240x16_SSD2119,
       0, 0, 320, 160,
       CANVAS_STYLE_APP_DRAWN, 0, 0, 0, 0, 0, 0, OnOptionsPaint);

//*****************************************************************************
//
// An array of canvas widgets, one per panel.  Each canvas is filled with
// black, overwriting the contents of the previous panel.
//
//*****************************************************************************
tCanvasWidget g_panels[] =
{
    CanvasStruct(0, 0, &g_menuPlay, &g_sKentec320x240x16_SSD2119,
                 0, 0, 320, 160,
                 CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, &g_sOptions, &g_sKentec320x240x16_SSD2119,
                 0, 0, 320, 160,
                 CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
};
#define NUM_PANELS (sizeof(g_panels) / sizeof(g_panels[0]))

//*****************************************************************************
//
// The names for each of the panels, which is displayed at the bottom of the
// screen.
//
//*****************************************************************************
char *g_pcPanelNames[] =
{
    "     Menu     ",
    "     Options     ",
};

//*****************************************************************************
//
// The buttons and text across the bottom of the screen.
//
//*****************************************************************************
RectangularButton(g_sPrevious, 0, 0, 0, &g_sKentec320x240x16_SSD2119,
                  0, 182, 50, 50,
                  PB_STYLE_FILL, ClrBlack, ClrBlack, 0, ClrSilver,
                  g_psFontCm20, "-", 0, 0,
                  0, 0, OnPrevious);
Canvas(g_sTitle, 0, 0, 0, &g_sKentec320x240x16_SSD2119,
       50, 182, 204, 50,
       CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_OPAQUE, 0, 0, ClrSilver,
       g_psFontCm20, 0, 0, 0);
RectangularButton(g_sNext, 0, 0, 0, &g_sKentec320x240x16_SSD2119,
                  (320-50), 182, 50, 50,
                  PB_STYLE_IMG | PB_STYLE_TEXT, ClrBlack, ClrBlack, 0,
                  ClrSilver, g_psFontCm20, "+", 0,
                  0, 0, 0, OnNext);

//*****************************************************************************
//
// The panel that is currently being displayed.
//
//*****************************************************************************
uint32_t g_ui32Panel;
static bool exit;

void
OnMenuPlayPress(tWidget *psWidget)
{
    exit = 1;
}

void
OnMenuOptionsPress(tWidget *psWidget)
{
    OnNext(psWidget);
}

//*****************************************************************************
//
// Handles presses of the previous panel button.
//
//*****************************************************************************
void
OnPrevious(tWidget *psWidget)
{
    //
    // There is nothing to be done if the first panel is already being
    // displayed.
    //
    if(g_ui32Panel == 0)
    {
        return;
    }

    //
    // Remove the current panel.
    //
    WidgetRemove((tWidget *)(g_panels + g_ui32Panel));

    //
    // Decrement the panel index.
    //
    g_ui32Panel--;

    //
    // Add and draw the new panel.
    //
    WidgetAdd(WIDGET_ROOT, (tWidget *)(g_panels + g_ui32Panel));
    WidgetPaint((tWidget *)(g_panels + g_ui32Panel));

    //
    // Set the title of this panel.
    //
    CanvasTextSet(&g_sTitle, g_pcPanelNames[g_ui32Panel]);
    WidgetPaint((tWidget *)&g_sTitle);

    //
    // See if this is the first panel.
    //
    if(g_ui32Panel == 0)
    {
        //
        // Clear the previous button from the display since the first panel is
        // being displayed.
        //
        PushButtonImageOff(&g_sPrevious);
        PushButtonTextOff(&g_sPrevious);
        PushButtonFillOn(&g_sPrevious);
        WidgetPaint((tWidget *)&g_sPrevious);
    }

    //
    // See if the previous panel was the last panel.
    //
    if(g_ui32Panel == (NUM_PANELS - 2))
    {
        //
        // Display the next button.
        //
        PushButtonImageOn(&g_sNext);
        PushButtonTextOn(&g_sNext);
        PushButtonFillOff(&g_sNext);
        WidgetPaint((tWidget *)&g_sNext);
    }
}

//*****************************************************************************
//
// Handles presses of the next panel button.
//
//*****************************************************************************
void
OnNext(tWidget *psWidget)
{
    //
    // There is nothing to be done if the last panel is already being
    // displayed.
    //
    if(g_ui32Panel == (NUM_PANELS - 1))
    {
        return;
    }

    //
    // Remove the current panel.
    //
    WidgetRemove((tWidget *)(g_panels + g_ui32Panel));

    //
    // Increment the panel index.
    //
    g_ui32Panel++;

    //
    // Add and draw the new panel.
    //
    WidgetAdd(WIDGET_ROOT, (tWidget *)(g_panels + g_ui32Panel));
    WidgetPaint((tWidget *)(g_panels + g_ui32Panel));

    //
    // Set the title of this panel.
    //
    CanvasTextSet(&g_sTitle, g_pcPanelNames[g_ui32Panel]);
    WidgetPaint((tWidget *)&g_sTitle);

    //
    // See if the previous panel was the first panel.
    //
    if(g_ui32Panel == 1)
    {
        //
        // Display the previous button.
        //
        PushButtonImageOn(&g_sPrevious);
        PushButtonTextOn(&g_sPrevious);
        PushButtonFillOff(&g_sPrevious);
        WidgetPaint((tWidget *)&g_sPrevious);
    }

    //
    // See if this is the last panel.
    //
    if(g_ui32Panel == (NUM_PANELS - 1))
    {
        //
        // Clear the next button from the display since the last panel is being
        // displayed.
        //
        PushButtonImageOff(&g_sNext);
        PushButtonTextOff(&g_sNext);
        PushButtonFillOn(&g_sNext);
        WidgetPaint((tWidget *)&g_sNext);
    }
}

//*****************************************************************************
//
// Handles paint requests for the introduction canvas widget.
//
//*****************************************************************************
void
OnMenuPaint(tWidget *psWidget, tContext *psContext)
{
    GrContextFontSet(psContext, g_psFontCm20);
    GrContextForegroundSet(psContext, ClrSilver);
    GrStringDraw(psContext, "UPMC Raycast", -1,
                 90, 30, 0);
}

//*****************************************************************************
//
// Handles paint requests for the primitives canvas widget.
//
//*****************************************************************************
void
OnOptionsPaint(tWidget *psWidget, tContext *psContext)
{
}

void
menuInit()
{
    //
    // Add the title block and the previous and next buttons to the widget
    // tree.
    //
    WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sPrevious);
    WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sTitle);
    WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sNext);

    //
    // Add the first panel to the widget tree.
    //
    g_ui32Panel = 0;
    WidgetAdd(WIDGET_ROOT, (tWidget *)g_panels);
    CanvasTextSet(&g_sTitle, g_pcPanelNames[0]);

    //
    // Issue the initial paint request to the widgets.
    //
    WidgetPaint(WIDGET_ROOT);
}

void
menuRun()
{
    exit = 0;
    audio_play(&intro_snd, false);

    while (!exit) {
        WidgetMessageQueueProcess();
        audio_process();
    }
}
