# SMOT

Compile and Run on an X11 system, and select and area to screenshot.

## Dependencies

`X11`, `scrot`, and a composer (if you dont have one, use the option -n for no visual feedback)

## Usage

`smot [options] dest_folder`

### IMPORTANT

You probably want to set the dest folder, as the default is `~/Images/screenshots/` which is rather unusual.
NOTE: make sure your destination string ends with `/`. I did not spend the time to make it nice, as this
is really just intended for personal use.

## Options

`-v` to get more information as the program runs<br>
`-n` to disable visual feedback (it can be laggy, ill fix it as some point. probably.)<br>
