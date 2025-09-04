# OnKey
This Ashita v4 plugin handles key presses from valid VK code keys.

## Example config
Config file is in your Ashita directory under ``config/onkey.ini``.

```
[Keys]
VK_F13=true
VK_F14=true
VK_F15=true
VK_F16=true
```
With this configuration one can create an alias via ``/alias /onkey_VK_F13 /mycommandhere`` so that your command will be executed when F13 is pressed.  Also supported are the following modifier keys with suffixes (such as ``/onkey_VK_F13_apps``

Shift: ``/alias /onkey_VK_F13_shift /mycommandhere``

Control: ``/alias /onkey_VK_F13_ctrl /mycommandhere``

Meta/Windows: ``/alias /onkey_VK_F13_meta /mycommandhere``

Apps: ``/alias /onkey_VK_F13_apps /mycommandhere``

Combinations of modifiers can also be configured, but the order of modifiers is undocumented; take a look at the order in the source if you want.  Also, Alt is technically supported as a modifier but due to issues with it being misread as held when alt tabbing I have personally commented it out as it was annoying.



