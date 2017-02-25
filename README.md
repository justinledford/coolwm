# coolwm
A very simple stacking window manager for X11, inspired by cwm and tinywm.  

##Current Features:


- Launch xterm  
- Launch xclock  
- Move focused window (up/down/left/right)  
- Resize focused window  
- Cycle through windows  
- Assign windows to groups (workspaces) 
- Focused/unfocused borders    


Keybindings are assigned in a conf file, see default.coolwmrc.  

##To-Do:  
- Options in conf file (move amount)  
- Assign executables to key-bindings in config file (terminals, application launchers, etc.)  
- Remember cursor position on cycle  
- Reverse cycle  


##Development:
Run an X server in a window:  
`Xephyr -ac -screen 800x600 -reset :1 &`  
  
Run coolwm in Xephyr  
`DISPLAY=:1 ./coolwm .coolwmrc`
