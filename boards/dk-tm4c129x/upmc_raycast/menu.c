#include "res/intro_snd.h"
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

static void OnMenuPlayPress(tWidget *psWidget);
static void OnMenuOptionsPress(tWidget *psWidget);
static void OnOptionsRetourPress(tWidget *psWidget);
static void OnMenuPaint(tWidget *psWidget, tContext *psContext);
static void OnOptionsPaint(tWidget *psWidget, tContext *psContext);
extern tCanvasWidget g_panels[];

Canvas(g_menu, g_panels, 0, 0, &g_sKentec320x240x16_SSD2119,
       0, 0, 320, 160,
       CANVAS_STYLE_APP_DRAWN, 0, 0, 0, 0, 0, 0, OnMenuPaint);
RectangularButton(g_menuOptions, g_panels, &g_menu, 0,
                  &g_sKentec320x240x16_SSD2119, 320/2 - 100/2, 90, 100, 30,
                  PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT,
                  ClrMidnightBlue, ClrMidnightBlue, ClrGray, ClrSilver,
                  g_psFontCm22, "Options", 0, 0, 0, 0, OnMenuOptionsPress);
RectangularButton(g_menuPlay, g_panels, &g_menuOptions, 0,
                  &g_sKentec320x240x16_SSD2119, 320/2 - 100/2, 60, 100, 30,
                  PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT,
                  ClrMidnightBlue, ClrMidnightBlue, ClrGray, ClrSilver,
                  g_psFontCm22, "Jouer", 0, 0, 0, 0, OnMenuPlayPress);

Canvas(g_sOptions, g_panels + 1, 0, 0, &g_sKentec320x240x16_SSD2119,
       0, 0, 320, 160,
       CANVAS_STYLE_APP_DRAWN, 0, 0, 0, 0, 0, 0, OnOptionsPaint);
RectangularButton(g_sOptionsBack, g_panels, &g_sOptions, 0,
                  &g_sKentec320x240x16_SSD2119, 20, 20, 100, 30,
                  PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT,
                  ClrMidnightBlue, ClrMidnightBlue, ClrGray, ClrSilver,
                  g_psFontCm22, "Retour", 0, 0, 0, 0, OnOptionsRetourPress);

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
    CanvasStruct(0, 0, &g_sOptionsBack, &g_sKentec320x240x16_SSD2119,
                 0, 0, 320, 160,
                 CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
};
#define NUM_PANELS (sizeof(g_panels) / sizeof(g_panels[0]))

//*****************************************************************************
//
// The panel that is currently being displayed.
//
//*****************************************************************************
static uint32_t g_ui32Panel;
static bool exit;

static void
OnMenuPlayPress(tWidget *psWidget)
{
    exit = 1;
}

static void
OnMenuOptionsPress(tWidget *psWidget)
{
    WidgetRemove((tWidget *)(g_panels + g_ui32Panel));
    g_ui32Panel = 1;
    WidgetAdd(WIDGET_ROOT, (tWidget *)(g_panels + g_ui32Panel));
    WidgetPaint((tWidget *)(g_panels + g_ui32Panel));
}

static void
OnOptionsRetourPress(tWidget *psWidget)
{
    WidgetRemove((tWidget *)(g_panels + g_ui32Panel));
    g_ui32Panel = 0;
    WidgetAdd(WIDGET_ROOT, (tWidget *)(g_panels + g_ui32Panel));
    WidgetPaint((tWidget *)(g_panels + g_ui32Panel));
}

//*****************************************************************************
//
// Handles paint requests for the introduction canvas widget.
//
//*****************************************************************************
static void
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
static void
OnOptionsPaint(tWidget *psWidget, tContext *psContext)
{
}

void
menuInit()
{
    //
    // Add the first panel to the widget tree.
    //
    g_ui32Panel = 0;
    WidgetAdd(WIDGET_ROOT, (tWidget *)g_panels);

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
