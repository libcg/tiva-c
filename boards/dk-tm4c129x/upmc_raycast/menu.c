#include "menu.h"
#include "config.h"
#include "audio.h"
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

#ifdef USE_SOUND
#include "res/intro_snd.h"
#endif

static void OnMenuPlayPress(tWidget *psWidget);
static void OnMenuOptionsPress(tWidget *psWidget);
static void OnOptionsRetourPress(tWidget *psWidget);
static void OnNextLevelBtnPress(tWidget *psWidget);
static void OnEndBtnPress(tWidget *psWidget);
static void OnTimesupBtnPress(tWidget *psWidget);
static void OnMenuPaint(tWidget *psWidget, tContext *psContext);
static void OnOptionsPaint(tWidget *psWidget, tContext *psContext);
static void OnNextLevelPaint(tWidget *psWidget, tContext *psContext);
static void OnEndPaint(tWidget *psWidget, tContext *psContext);
static void OnTimesupPaint(tWidget *psWidget, tContext *psContext);
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

Canvas(g_nextLevel, g_panels + 2, 0, 0, &g_sKentec320x240x16_SSD2119,
       0, 0, 320, 160,
       CANVAS_STYLE_APP_DRAWN, 0, 0, 0, 0, 0, 0, OnNextLevelPaint);
RectangularButton(g_nextLevelBtn, g_panels, &g_nextLevel, 0,
                  &g_sKentec320x240x16_SSD2119, 320/2 - 100/2, 120, 100, 30,
                  PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT,
                  ClrMidnightBlue, ClrMidnightBlue, ClrGray, ClrSilver,
                  g_psFontCm22, "Suivant", 0, 0, 0, 0, OnNextLevelBtnPress);

Canvas(g_end, g_panels + 3, 0, 0, &g_sKentec320x240x16_SSD2119,
       0, 0, 320, 160,
       CANVAS_STYLE_APP_DRAWN, 0, 0, 0, 0, 0, 0, OnEndPaint);
RectangularButton(g_endBtn, g_panels, &g_end, 0,
                  &g_sKentec320x240x16_SSD2119, 320/2 - 100/2, 120, 100, 30,
                  PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT,
                  ClrMidnightBlue, ClrMidnightBlue, ClrGray, ClrSilver,
                  g_psFontCm22, "Retour", 0, 0, 0, 0, OnEndBtnPress);

Canvas(g_timesup, g_panels + 4, 0, 0, &g_sKentec320x240x16_SSD2119,
       0, 0, 320, 160,
       CANVAS_STYLE_APP_DRAWN, 0, 0, 0, 0, 0, 0, OnTimesupPaint);
RectangularButton(g_timesupBtn, g_panels, &g_timesup, 0,
                  &g_sKentec320x240x16_SSD2119, 320/2 - 100/2, 120, 100, 30,
                  PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT,
                  ClrMidnightBlue, ClrMidnightBlue, ClrGray, ClrSilver,
                  g_psFontCm22, "Retour", 0, 0, 0, 0, OnTimesupBtnPress);

//*****************************************************************************
//
// An array of canvas widgets, one per panel.  Each canvas is filled with
// black, overwriting the contents of the previous panel.
//
//*****************************************************************************
tCanvasWidget g_panels[] =
{
    CanvasStruct(0, 0, &g_menuPlay, &g_sKentec320x240x16_SSD2119,
                 0, 0, 320, 240,
                 CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, &g_sOptionsBack, &g_sKentec320x240x16_SSD2119,
                 0, 0, 320, 240,
                 CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, &g_nextLevelBtn, &g_sKentec320x240x16_SSD2119,
                 0, 0, 320, 240,
                 CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, &g_endBtn, &g_sKentec320x240x16_SSD2119,
                 0, 0, 320, 240,
                 CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, &g_timesupBtn, &g_sKentec320x240x16_SSD2119,
                 0, 0, 320, 240,
                 CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
};
#define NUM_PANELS (sizeof(g_panels) / sizeof(g_panels[0]))

//*****************************************************************************
//
// The panel that is currently being displayed.
//
//*****************************************************************************
static uint32_t g_ui32Panel = 0;
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
    g_ui32Panel = MENU_OPTIONS;
    WidgetAdd(WIDGET_ROOT, (tWidget *)(g_panels + g_ui32Panel));
    WidgetPaint((tWidget *)(g_panels + g_ui32Panel));
}

static void
OnOptionsRetourPress(tWidget *psWidget)
{
    WidgetRemove((tWidget *)(g_panels + g_ui32Panel));
    g_ui32Panel = MENU_HOME;
    WidgetAdd(WIDGET_ROOT, (tWidget *)(g_panels + g_ui32Panel));
    WidgetPaint((tWidget *)(g_panels + g_ui32Panel));
}

static void
OnNextLevelBtnPress(tWidget *psWidget)
{
    exit = 1;
}

static void
OnEndBtnPress(tWidget *psWidget)
{
    OnOptionsRetourPress(psWidget);
}

static void
OnTimesupBtnPress(tWidget *psWidget)
{
    OnOptionsRetourPress(psWidget);
}

static void
OnMenuPaint(tWidget *psWidget, tContext *psContext)
{
    GrContextFontSet(psContext, g_psFontCm20);
    GrContextForegroundSet(psContext, ClrSilver);
    GrStringDraw(psContext, "UPMC Raycast", -1,
                 90, 30, 0);
}

static void
OnOptionsPaint(tWidget *psWidget, tContext *psContext)
{
}

static void
OnNextLevelPaint(tWidget *psWidget, tContext *psContext)
{
    GrContextFontSet(psContext, g_psFontCm20);
    GrContextForegroundSet(psContext, ClrSilver);
    GrStringDraw(psContext, "Gagne !", -1,
                 320/2 - 35, 30, 0);
}

static void
OnEndPaint(tWidget *psWidget, tContext *psContext)
{
    GrContextFontSet(psContext, g_psFontCm20);
    GrContextForegroundSet(psContext, ClrSilver);
    GrStringDraw(psContext, "Jeu termine !", -1,
                 320/2 - 50, 30, 0);
}

static void
OnTimesupPaint(tWidget *psWidget, tContext *psContext)
{
    GrContextFontSet(psContext, g_psFontCm20);
    GrContextForegroundSet(psContext, ClrSilver);
    GrStringDraw(psContext, "Temps ecoule !", -1,
                 320/2 - 55, 30, 0);
}

void menuInit()
{
#ifdef USE_SOUND
    audio_play(&intro_snd, false);
#endif
}

void menuSetState(int state)
{
    g_ui32Panel = state;
}

void menuRun()
{
    WidgetAdd(WIDGET_ROOT, (tWidget *)(g_panels + g_ui32Panel));
    WidgetPaint((tWidget *)(g_panels + g_ui32Panel));

    exit = false;

    while (!exit) {
        WidgetMessageQueueProcess();
        audio_process();
    }

    WidgetRemove((tWidget *)(g_panels + g_ui32Panel));
}
