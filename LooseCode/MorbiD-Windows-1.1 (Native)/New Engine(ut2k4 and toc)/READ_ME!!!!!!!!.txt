:::::::::::::::::::
:::::::::::::::::::
:::::::::::::::::::
MorbiD-Windows Read-Me:
:::::::::::::::::::
:::::::::::::::::::
:::::::::::::::::::


:::::::::::::::::::How to include:::::::::::::::::::

1.) Copy 'MorbiD-Windows.cpp' and 'MorbiD-Windows.h' in your project folder
2.) Open your project (your hook source:P)
3.) Add MorbiD-Windows.cpp to your source files
4.) Add MorbiD-Windows.h   to your header files
5.) Include MorbiD-Windows.h in your main header file
	the command is: #include "MorbiD-Windows.h"
6.) Go to MorbiD-Windows.cpp and Modify the name of the Main header file
	(Standart is: HookMaster.h)
7.) Do the same as in 6 with the MoriD-Windows.h file
8.) Add this line to on top of your postrender: 

	MWindow.MorbidWindowsMain(Canvas);

9.) Add this line to your Tick:

	Mouse.UpdateMouse(DeltaTime);
10.) Use the commands as described below and have fun:P



:::::::::::::::::::Commands:::::::::::::::::::

In order to create a window you need to do this:
================================================

1.) Create a new function like void DrawMyWindows(UCanvas * Canvas) and include it in your postrender
    Or just write everything in your postrender;) 

2.) Add at the buttom of this function folowing command: MWindow.CreateMouse();
    and keep it at the buttom else it will be drown behind the windows;)

2.) Declare the window: MorbidWindow Window1; (or instead of window1 the variable name you want to use)

3.) then use drawwindow: MWindow.DrawWindow()

    The parameters are as folowed:

	- Window (the variable you declared in the last step) :Morbidwindow
	- xPosition of the window			      :float
	- YPosition of the window			      :float
	- Width (the horizontal lengh)                        :float
	- Height (vertical lengh)                             :float
	- Style (if you want to use a title box or not)       :int
	- Traslucent(if you can look trough)		      :bool
	- Title (use: TEXT("Your title")  ) 		      :TCHAR*


To add a checkbox you need to do this:
======================================

1.) Create a window(recomended)
2.) use this command: MWindow.AddCheckBox()
3.) Parameters are: 
	-XPosition					      :float
	-YPosition					      :float
	-The bool that it checks(for example bautoaim)        :bool
	-Rightalinedtext(if the text is right or left)        :bool
	-The name of the box(use: TEXT("Your name")  )	      :TCHAR*

4.) To check if the box was clicked use this function
    ( Mouse.CheckBoxClick())
    Parameters are:
	-XPosition of the box				      :float
	-YPosition of the box				      :float
	-Rightalinedtext(if the text is right or left)	      :bool

    Use it like this:
    if( Mouse.CheckBoxClick(Yourxpos,yourypos,false or true)
    {
	//do something if the box is activated like:
	bAutoAim = !bAutoAim; //autoaim on off
    }


To add a Button you need to do this:
====================================

1.) use this command: MWindow.AddButton
2.) Im not going to explain the parameters once again its similiar to the onse above
3.) To check if it was pressed use this function:
    Mouse.ButtonClick()
    Just like the checkbox click with the diference that you also have to define the widht and height (and no rightalinedtext;) )


Other Commands:
================

From here on i hope you understood the system how this all works ;)
Further commands are:

MWindow.DrawRadarBox()		//Creates a radar box
MWindow.AddStatic()		//Adds a static (black box)
MWindow.DrawText()		//Draws a normal text
MWindow.MakeShadowText()	//Draws a stylish shadow text

All you still need are some parameters like the colors.

To define a specifil color use this function:

Color.Black();			//black color
Color.White();			//white color
....
....				and so on

Fonts are:

SmallFont

//info:
I will add more font on time;)

Feel free to look around in the files and feel free to modify them;)

=======================================================================
=======================================================================
=======================================================================
=======================================================================
I got a tip for you if you didnt guess it allready;)

Create child elements like checkboxes and buttons just after youve drawn the window!

========================================================================
========================================================================
========================================================================

Credits go 2:	
		Helios      : for all his tutorials an everything he ever made;)
		N4ps3r      : for his help whenever i need it
		Cheatat0r   : for support thx m8;)
		Cheaterelite: 1337
		Daru		: For releasing his source(i used it as a base:P)




Ive added an example project in visual studio .net feel free to play around;)

Give me some credits if you use this have fun:P

