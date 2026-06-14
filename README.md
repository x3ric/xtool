<div align="center">

# xtool

X11 automation and window management tool written in [Nytrix](https://github.com/nytrix-lang/nytrix).

#### Requirements

[nytrix](https://github.com/nytrix-lang/nytrix) · X11 · libXtst

### Usage

<pre>
xtool &lt;command&gt; [args...]
</pre>

</div>

---

## Mouse

| Command | Description |
|---|---|
| `mousemove x y` | Move mouse to absolute position |
| `mousemove_smooth x y` | Move mouse to absolute position with smooth animation |
| `mousemove_relative x y` | Move mouse relative to current position |
| `mousemove_relative_smooth x y` | Move mouse relative with smooth animation |
| `click [button]` | Click mouse button (1=left 2=middle 3=right, default 1) |
| `mousedown [button]` | Press mouse button |
| `mouseup [button]` | Release mouse button |
| `getmouselocation [--shell] [--prefix P]` | Print current mouse x/y/screen/window |

## Keyboard

| Command | Description |
|---|---|
| `key <seq>` | Press and release key sequence (e.g. `Return`, `alt+Tab`) |
| `keydown <seq>` | Press key(s) |
| `keyup <seq>` | Release key(s) |
| `combo <seq>` | Send key combo (e.g. `ctrl+alt+F1`) |
| `type <text>` | Type a string character by character |
| `repeat-key <n> <key> [--delay ms]` | Repeat a key N times |

## Windows

| Command | Description |
|---|---|
| `listwindows` | List all client windows |
| `search <text>` | Search windows by title |
| `selectwindow` | Click to select a window, prints its ID |
| `find-window [pattern]` | Find window by title (returns largest match) |
| `wait-window <sec> [pattern]` | Wait up to N seconds for a window to appear |
| `getactivewindow` | Print active window ID |
| `active-window-id` | Print active window ID (decimal) |
| `getwindowfocus` | Print focused window ID |
| `getwindowgeometry <id> [--shell] [--prefix P]` | Window position and size |
| `window-geom <id>` | Window geometry as `x y w h` |
| `getwindowname <id>` | Window title |
| `getwindowclassname <id>` | Window class name |
| `getwindowpid <id>` | Window PID |
| `windowfocus <id>` | Focus a window |
| `windowactivate <id>` | Activate window (switches desktop if needed) |
| `windowclose <id>` | Close window via `_NET_CLOSE_WINDOW` |
| `windowquit <id>` | Send `WM_DELETE_WINDOW` (polite close request) |
| `windowkill <id>` | Kill window's client process |
| `windowsize <id> <w> <h>` | Resize a window |
| `windowmove <id> <x> <y>` | Move a window |
| `windowmap <id>` | Show (map) a window |
| `windowunmap <id>` | Hide (unmap) a window |
| `windowminimize <id>` | Minimize a window |
| `windowraise <id>` | Raise a window to the top |
| `windowlower <id>` | Lower a window to the bottom |
| `move-window <id> <rx> <ry>` | Warp pointer to window-relative position |
| `click-window <id> <rx> <ry> [button]` | Warp+click at window-relative position |
| `wm-state [id]` | List `_NET_WM_STATE` atoms of window (defaults to active) |
| `fullscreen-window <id>` | Set window fullscreen |
| `maximize-window <id>` | Maximize window |

## Desktop

| Command | Description |
|---|---|
| `get_num_desktops` | Print number of virtual desktops |
| `set_num_desktops <n>` | Set number of virtual desktops |
| `get_desktop` | Print current desktop index |
| `set_desktop <n>` | Switch to desktop n |
| `get_desktop_for_window <id>` | Print desktop containing a window |
| `set_desktop_for_window <id> <n>` | Move window to desktop n |
| `get_desktop_viewport [--shell] [--prefix P]` | Print viewport x y |
| `set_desktop_viewport <x> <y>` | Set desktop viewport |
| `getdisplaygeometry [--shell] [--prefix P]` | Print screen width and height |

## Monitoring

| Command | Description |
|---|---|
| `inputscan` | Listen for key press/release events (Escape or Ctrl+C to quit) |
| `fakemouse [--wasd]` | Control mouse with keyboard arrows/numpad (q/Escape to quit) |

`fakemouse` bindings: arrows/numpad=move · space=hold-left · r=hold-right · m=hold-middle · kp5=click · pgup/pgdn=scroll · shift=drag · 1-9=speed · 0=pixel · +/-=adjust speed

## Other

| Command | Description |
|---|---|
| `sleep <sec>` | Sleep for N seconds |
| `exec <cmd...>` | Execute a shell command |
| `version` | Print version |
| `help` | Show help |

---

## Aliases

Most commands have short aliases:

| Alias | Command |
|---|---|
| `ls`, `winlist` | `listwindows` |
| `mv`, `mm` | `mousemove` |
| `mvrel`, `mmrel` | `mousemove_relative` |
| `mvsm`, `mmsm` | `mousemove_smooth` |
| `mvre`, `mmre` | `mousemove_relative_smooth` |
| `loc`, `where` | `getmouselocation` |
| `aw`, `active` | `getactivewindow` |
| `awid`, `activeid` | `active-window-id` |
| `gf`, `focus` | `getwindowfocus` |
| `gw`, `geom` | `getwindowgeometry` |
| `wg` | `window-geom` |
| `wn`, `name` | `getwindowname` |
| `cls` | `getwindowclassname` |
| `sel` | `selectwindow` |
| `wf`, `focusw` | `windowfocus` |
| `wa`, `activate` | `windowactivate` |
| `ws` | `windowsize` |
| `wm`, `movew` | `windowmove` |
| `map` | `windowmap` |
| `unmap` | `windowunmap` |
| `min` | `windowminimize` |
| `raise` | `windowraise` |
| `lower` | `windowlower` |
| `close` | `windowclose` |
| `kill` | `windowkill` |
| `sc` | `search` |
| `fw`, `find` | `find-window` |
| `ww` | `wait-window` |
| `mw`, `movewin` | `move-window` |
| `cw` | `click-window` |
| `wstate` | `wm-state` |
| `fs`, `fullscreen` | `fullscreen-window` |
| `mx`, `maximize` | `maximize-window` |
| `dg`, `disp`, `screen` | `getdisplaygeometry` |
| `nd`, `desks` | `get_num_desktops` |
| `snd`, `numdesktops` | `set_num_desktops` |
| `cd`, `desk` | `get_desktop` |
| `sd` | `set_desktop` |
| `gdw` | `get_desktop_for_window` |
| `sdw` | `set_desktop_for_window` |
| `gdv`, `getviewport` | `get_desktop_viewport` |
| `sdv`, `setviewport` | `set_desktop_viewport` |
| `rk`, `repeat` | `repeat-key` |
| `sl` | `sleep` |
| `wq` | `windowquit` |

---

## Examples

```sh
xtool mousemove 100 100
xtool mousemove_relative -10 20
xtool click 1
xtool key Return
xtool key alt+Tab
xtool combo ctrl+alt+F1
xtool type "Hello, World!"
xtool repeat-key 3 Return
xtool getmouselocation
xtool getmouselocation --shell --prefix MOUSE_
xtool getdisplaygeometry
xtool listwindows
xtool search firefox
xtool selectwindow
xtool getwindowgeometry 0x1e00003
xtool getwindowname 0x1e00003
xtool windowactivate 0x1e00003
xtool windowsize 0x1e00003 1280 720
xtool windowmove 0x1e00003 0 0
xtool maximize-window 0x1e00003
xtool fullscreen-window 0x1e00003
xtool find-window firefox
xtool wait-window 5 firefox
xtool click-window 0x1e00003 400 300
xtool wm-state 0x1e00003
xtool get_desktop
xtool set_desktop 1
xtool inputscan
xtool fakemouse
xtool fakemouse --wasd
xtool sleep 2
xtool exec notify-send "done"
```

---

<p align="center">
<a href="https://archlinux.org">
  <img alt="Arch Linux" src="https://img.shields.io/badge/Arch_Linux-1793D1?style=for-the-badge&logo=arch-linux&logoColor=D9E0EE&color=000000&labelColor=97A4E2"/>
</a>
</p>
