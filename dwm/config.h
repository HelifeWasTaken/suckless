/**
 * File              : config.h
 * Author            : Mattis DALLEAU <mattisdalleau@gmail.com>
 * Date              : 29.05.2021
 * Last Modified Date: 29.05.2021
 * Last Modified By  : Mattis DALLEAU <mattisdalleau@gmail.com>
 */
/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

#ifndef NULL
    #define NULL ((void *)0)
#endif

/* appearance */
static const unsigned int gappx        = 0;    /* gaps size */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#1e1e1e";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#F4800d";
static const char col_gray4[]       = "#1e1e1e";
static const char col_cyan[]        = "#f4800d";
static const char *colors[][3]      = {
    /*               fg         bg         border   */
    [SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
    [SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* Volume */

static const char *upvolume[] =
    { "/usr/bin/pactl", "set-sink-volume", "0", "+5%", NULL };
static const char *downvolume[] =
    { "/usr/bin/pactl", "set-sink-volume", "0", "-5%", NULL };
static const char *mutevolume[] =
    { "/usr/bin/pactl", "set-sink-mute",   "0", "toogle", NULL };

/* tagging */

#define webapp "firefox"
static const char *youtube[] = { webapp, "youtube.com", NULL };

static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Launcher launchers[] = {
    /* command     Name */
    { youtube,    "youtube" },
};

static const Rule rules[] = {
    /* xprop(1):
     *    WM_CLASS(STRING) = instance, class
     *    WM_NAME(STRING) = title
     */
    /* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
    { "Gimp",    NULL,     NULL,           0,         1,          0,           0,        -1 },
    { "Firefox", NULL,     NULL,           0,      0,          0,          -1,        -1 },
    { "St",      NULL,     NULL,           0,         0,          1,           0,        -1 },
    { NULL,      NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
    /* symbol     arrange function */
    { "[]=",      tile },    /* first entry is default */
    { "><>",      NULL },    /* no layout function means floating behavior */
    { "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
    { MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
    { MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
    { MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
    { MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *locker[] = { "slock -m 'Ingenious software is simple. Simplicity is the heart of the Unix philosophy. The more code lines you have removed, the more progress you have made. As the number of lines of code in your software shrinks, the more skilled you have become and the less your software sucks.'", NULL };
static const char *screnshotter[] = { "gnome-screenshot", NULL };

static Key keys[] = {
    /* modifier                     key        function        argument */
    { MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
    { MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
    { MODKEY|ControlMask,            XK_l,       spawn,       {.v = locker } },
    { MODKEY|ShiftMask,            XK_space,       spawn,       {.v = screnshotter } },
    { MODKEY,                       XK_b,      togglebar,      {0} },
    { MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
    { MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
    { MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
    { MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
    { MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
    { MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
    { MODKEY,                       XK_Return, zoom,           {0} },
    { MODKEY,                       XK_Tab,    view,           {0} },
    { MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
    { MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
    { MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
    { MODKEY,                       XK_0,      view,           {.ui = ~0 } },
    { MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
    { MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
    { MODKEY,                       XK_period, focusmon,       {.i = +1 } },
    { MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
    { MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
    { MODKEY|ControlMask,            XK_q,       quit,       {0} },
    { 0,                          XF86XK_AudioLowerVolume,   spawn, {.v = downvolume } },
    { 0,                           XF86XK_AudioRaiseVolume,   spawn, {.v = upvolume   } },
    { 0,                           XF86XK_AudioMute,          spawn, {.v = mutevolume } },

    TAGKEYS(                        19,              0)
    TAGKEYS(                        10,              1)
    TAGKEYS(                        11,              2)
    TAGKEYS(                        12,              3)
    TAGKEYS(                        13,              4)
    TAGKEYS(                        14,              5)
    TAGKEYS(                        15,              6)
    TAGKEYS(                        16,              7)
    TAGKEYS(                        17,              8)
    TAGKEYS(                        18,              9)
    TAGKEYS(                        XK_ampersand,              0)
    TAGKEYS(                        XK_eacute,                 1)
    TAGKEYS(                        XK_quotedbl,               2)
    TAGKEYS(                        XK_apostrophe,             3)
    TAGKEYS(                        XK_parenleft,              4)
    TAGKEYS(                        XK_minus,                  5)
    TAGKEYS(                        XK_egrave,                 6)
    TAGKEYS(                        XK_underscore,             7)
    TAGKEYS(                        XK_ccedilla,               8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
    /* click                event mask      button          function        argument */
    { ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
    { ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
    { ClkWinTitle,          0,              Button2,        zoom,           {0} },
    { ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
    { ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
    { ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
    { ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
    { ClkTagBar,            0,              Button1,        view,           {0} },
    { ClkTagBar,            0,              Button3,        toggleview,     {0} },
    { ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
    { ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

