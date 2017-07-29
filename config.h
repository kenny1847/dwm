/* See LICENSE file for copyright and license details. */
#include<X11/XF86keysym.h>
#include"push.c"

/* appearance */
static const char *fonts[] = {
	"Terminus:size=12",
	"FontAwesome:size=12"
};
static const char dmenufont[]       = "Terminus:size=12";
static const char normbordercolor[] = "#404040";
static const char normbgcolor[]     = "#131314";
static const char normfgcolor[]     = "#9B9696";
static const char selbordercolor[]  = "#408075";
static const char selbgcolor[]      = "#131314";
static const char selfgcolor[]      = "#408075";
static const char *colors[SchemeLast][3]      = {
	/*               fg           bg           border   */
	[SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
	[SchemeSel] =  { selfgcolor,  selbgcolor,  selbordercolor  },
};

static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 0;     /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */

/* tagging                   0|0    1|1     2|2    3|3   4|4      5|F1      6|F2     7|F3    8|F4  */
static const char *tags[] = {"dev", "git", "www", "im", "media", "app",     "build", "misc", "other" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class                        instance    title       tags mask     isfloating   monitor */
	{ "Google-chrome",              NULL,       NULL,       1 << 2,       0,           -1},
	{ "google-chrome",              NULL,       NULL,       1 << 2,       0,           -1},
	{ "Telegram",                   NULL,       NULL,       1 << 3,       0,           -1},
};

/* layout(s) */
static const float mfact     = 0.5; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[T]",      tile },    /* first entry is default */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define ALTKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* alsa */
static const char *upvol[]   = { "amixer", "-q", "-D", "pulse", "sset", "Master", "2%+", NULL };
static const char *downvol[] = { "amixer", "-q", "-D", "pulse", "sset", "Master", "2%-", NULL };
static const char *mute[]    = { "amixer", "-q", "-D", "pulse", "sset", "Master", "toggle", NULL };
/* backlight */
static const char *incbacklight[] = { "xbacklight", "-inc", "10", "-steps", "1", NULL };
static const char *decbacklight[] = { "xbacklight", "-dec", "10", "-steps", "1", NULL };
static const char *inc_kbd_backlight[] = { "xbacklight", "-ctrl", "asus::kbd_backlight", "-inc", "30", NULL };
static const char *dec_kbd_backlight[] = { "xbacklight", "-ctrl", "asus::kbd_backlight", "-dec", "30", NULL };
/* launchers */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]    = { "dmenu_run", "-m",dmenumon, "-fn",dmenufont, "-nb",normbgcolor, "-nf",normfgcolor, "-sb",selbgcolor, "-sf",selfgcolor, NULL };
static const char *termcmd[]     = { "urxvt256c-mlc", NULL };
static const char *telegramcmd[] = { "/opt/Telegram/Telegram", NULL };
static const char *scrotcmd[]    = { "scrot", "-q", "100", "-e", "mv $f ~/Screenshots/", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	/*                            Volume Control                              */
	{ 0,              XF86XK_AudioRaiseVolume,       spawn,          {.v = upvol } },
	{ 0,              XF86XK_AudioLowerVolume,       spawn,          {.v = downvol } },
	{ 0,              XF86XK_AudioMute,              spawn,          {.v = mute } },
	/*                            Backlight Control                           */
	{ 0,              XF86XK_MonBrightnessUp,        spawn,          {.v = incbacklight } },
	{ 0,              XF86XK_MonBrightnessDown,      spawn,          {.v = decbacklight } },
	{ 0,              XF86XK_KbdBrightnessUp,        spawn,          {.v = inc_kbd_backlight } },
	{ 0,              XF86XK_KbdBrightnessDown,      spawn,          {.v = dec_kbd_backlight } },
	/*                            Launchers                                  */
	{ MODKEY,                       XK_p,            spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return,       spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_t,            spawn,          {.v = telegramcmd } },
	{ 0,                          XK_Print,          spawn,          {.v = scrotcmd } },
	/*                            Misc Commands                              */
	{ MODKEY,                       XK_b,            togglebar,      {0} },
	{ MODKEY,                       XK_j,            focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,            focusstack,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_j,            pushdown,       {0} },
	{ MODKEY|ShiftMask,             XK_k,            pushup,         {0} },
	{ MODKEY,                       XK_i,            incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,            incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,            setmfact,       {.f = -0.01} },
	{ MODKEY,                       XK_l,            setmfact,       {.f = +0.01} },
	{ MODKEY|ShiftMask,             XK_Return,       zoom,           {0} },
	{ MODKEY,                       XK_Tab,          view,           {0} },
	{ MODKEY,                       XK_q,            killclient,     {0} },
	{ MODKEY,                       XK_e,            setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_w,            setlayout,      {.v = &layouts[1]} },
	{ ALTKEY,                       XK_space,        setlayout,      {0} },
	{ ALTKEY|ShiftMask,             XK_space,        togglefloating, {0} },
	{ MODKEY,                       XK_0,            view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,            tag,            {.ui = ~0 } },
	TAGKEYS(                        XK_grave,                        0)
	TAGKEYS(                        XK_1,                            1)
	TAGKEYS(                        XK_2,                            2)
	TAGKEYS(                        XK_3,                            3)
	TAGKEYS(                        XK_4,                            4)
	TAGKEYS(                        XK_F1,                           5)
	TAGKEYS(                        XK_F2,                           6)
	TAGKEYS(                        XK_F3,                           7)
	TAGKEYS(                        XK_F4,                           8)
	{ MODKEY|ShiftMask,             XK_e,            quit,           {0} },
	{ MODKEY|ShiftMask,             XK_r,            quit,           {.i = 1} },
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[1]} },
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

