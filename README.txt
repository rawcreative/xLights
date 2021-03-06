xLights is a program that allows you to play the sequence files from Vixen and
LOR. It has the ability to drive USB DMX controllers, E1.31 Ethernet controllers. xLights has a extensive scheduler.
Nutcracker is a program that generates animated effects on massive RGB devices such as megatrees, matrices, and arches.

Latest releases are found at http://nutcracker123.com/nutcracker/releases/

XLIGHTS/NUTCRACKER RELEASE NOTES:
3.6.15 Dec 20, 2014
    -enh (djulien) Allow option to continue Preview playback if channel data is missing from .xseq file (helps avoid the need to re-render everything if you've added props at the end)
    -enh (djulien) Default Brightness to 100% if setting not present in XML file (only affects externally generated XML files)
    -enh (djulien) show elapsed time on file loads
    -bug (djulien) Fixed Wave effect when rendered in a .xseq (FillColor was not selected, which resulted in no output)
3.6.14 Dec 17, 2014
    -enh (djulien) Add collapse/expand button (located next to "Create Random Effects")
		to allow more grid space on laptops
    -enh (djulien) Add "My Display" and "Brightness" columns to Preview model CSV - makes it easier to see why a prop is not getting any data. 8/
    -bug (djulien) Avoid "error 0" when exporting CSV of Preview models.  Show correct value for "start channel#".
    -enh (gjones) Update On effect to have 3 color sliders, associated text fields, and bitmap showing selected color.
                  The bitmap button can be used to select the color which will update the sliders and text fields.
    -bug (dkulp) Should not be able to select fseq for model export, only eseq
    -bug (dkulp) On OSX only, could not open notes and map files that used txt extensions.
    -enh (gjones) Add On effect.  This effect simply allows you to turn on all pixels in a model to the selected color.
    -bug (gjones) Remove extraneous whitespace from the LMS filetype in the drop-down on the file select dialog when
                  selecting a file to convert.
    -bug (dkulp) Use cos/sin/radians/etc... to render circles effect.  Removes some extra noise.
    -enh (dkulp) When loading a sequence on Nutcracker tab, reset flags so you don't also need to re-load it on Preview pane
3.6.13 Dec 9, 2014
	-enh (dkulp) Allow a skip size of zero. This allow the skips effect to stand still
    -enh (djulien) Allow continue if port open fails (useful for diagnostics/debug)
    -enh (djulien) Add up-once and down-once options to Picture.
    -enh (djulien) Add Bubble option to Circles effect.
    -enh (djulien) Add Decaying Sine and Ivy/Fractal Wave options.
    -bug (djulien) Remove extraneous leading "-" in FV attr in xlights_papagayo.xml
    -bug (djulien) Sort list of channel#s in drop-down list on Papagayo tab
3.6.12 Dec 5, 2014
    -bug (dkulp) If Column 1 renders really fast, it could hang on save due to Thread 2 waiting for
		information from thread 1 which is already gone. This could cause hangs on save.
    -bug (dkulp) Fix some thread safety issues in RenderCircle, RenderText, RenderPicture. This would have muged the effects.
    -test (dkulp) Try flipping to a busy wait for TextEffect rendering to see if that prevents some crashes/hangs
    -enh (dkulp) RenderPictures now has a 20FPS check box so rendering can just advance per frame if the movie is already in 20fps.
		If unchecked, the "Speed" slider adjusts the speed (with 10 being "normal", <10 slower, >10 faster). A speed of 10 = 20fps.
3.6.11 Dec 4, 2014
    -enh (dkulp) Add a "Settings" menu.  First setting: ability to disable the multi-threaded saves.
			When disable, all is rendered on the main thread.  Slower, but maybe safer.
			With this change, there are now four ways to save
			1) Normal - threaded
			2) Normal + Fast Save - threaded but only rendering the stuff that has changed
			3) Threading disabled (settings menu) - all rendering is on the main thread, one column at a time.
			4) Threading disabled + fast save - all rendering is on the main thread,
				one column at a time, but starting with the top left most changed cell.
    -bug (dkulp) When "Play" hits the bottom of the grid, the "Stop" button wasn't changed back to "Play"
	-bug (sean) Ripple effect was not calculating corners correctly
	-enh (sean) Added thickness slider and 3D button to Ripple.
3.6.10	Dec 3, 2014
	-bug (sean) Added missing xLights.ico file. You should see icon in tray now
	-bug (sean) "Create Random Effects"Would create None,None about 20% of time. This is now fixed.
	-bug (sean) Ripple was not rendering to PREVIEW screen or to actual lights. fixed
	-bug (dkulp) If "Save" is clicked multiple times real quick, it could hang/crash as it tries to save/render things at the same time.
			Fixed by disabling the Save buttons while saving.
	-bug (dkulp) Insert row inserting two rows, not one
	-bug (dkulp) On the MAC, the clipboard sometimes used \r instead of \n.
		(From Numbers for example) Update the paste to the grid to accommodate that.
	-enh (dkulp) Change the "Play (F4)" button in the NUTCRACKER tab to be a Play/Stop toggle.
3.6.9	Nov 29, 2014
    -bug (dkulp) Move Text effect rendering back to main thread due GDI calls it makes (to get Font info and to
		render the text) which is required to be done on the main thread.
    -bug (dkulp) Custom models that don't have an element in the grid for all channels in the range would cause
		buffer overruns on ModelExport
    -bug (dkulp) ModelExport was not using the SettingsMap to determine the state of the "Persistent" checkbox
		and instead looked at the actual checkbox on the page.
    -enh (dkulp) MAC - turn off anti-aliased text rendering to make it match Windows (and look better)
    -bug (dkulp) Model Export was crashing, now fixed.
3.6.8	Nov 28, 2014
	-enh (sean) Modified install program so that it has an auto launch button for Windows installs
    -enh (mbrown) Removed check that number of Renard channels is a multiple of 8.
    -enh (dkulp) Add ability to ignore LOR channels that do not have a deviceType and network defined. (common
                  for beat tracks and unused channels.
	-bug (sean) Fixed label creation of SETUP and convert so export of lms file now creates the RGB data
	-enh (dpitts) Added Preview Groups into PREVIEW tab, Create a group, when you select that group the
			"Part of My Display" will be set automatically for you. This allows you to set groups of models and be
			able to turn on the "Part of My Display"
    -bug (dkulp) Fix a bug where the LastChannelCount always assumed RGB nodes
    -bug (dkulp) On Mac, could not paste stuff into custom model due to using \r instead of \n for line delimiter
    -enh (dkulp) SPEED: Multi-thread the Nutcracker tab saving to speed up the rendering/saving
    -enh (dkulp) SPEED: Change the internal representation of a color to avoid some unnecessary OS calls.  Results in
                  significantly faster rendering of effects.
    -enh (dkulp) SPEED: Add "Fast Save" option on Nutcracker tab to allow the save command to only render the cells below
                  and to the right of the cells that have changed.  Placing the models you are working on to the
                  right on the grid can significantly speed up turn around time if you flip between Preview and
                  Nutcracker often.
    -bug (dkulp) Fix bug in SingelStrand chase effect that could cause a crash/memory corruption if used on
                  models with more than 1000 nodes.  (like whole house)
    -bug (dkulp) Fix a crash that would occur if the image effect was used, but no image was available.
    -bug (dkulp) Column  Shift left/right now copies cell protection.
3.6.7	Nov 23, 2014
	-bug (djulien) scrunch UI so it will fit on a laptop (needs to be < 760 px high)
    -bug (dkulp) Fix Rendering of Ripple effect during save, now shows up in PREVIEW and lights
    -bug (dkulp) Fix Rendering of Strobe effect during save, now shows up in PREVIEW and lights
3.6.6	Nov 22, 2014
	-bug (dkulp) Size of picture and setting size of display window in PREVIEW could cause a condition where you
			could not select items. fixed.
	-enh (cpinkham) Add 'Pixelnet-Open' Network Type for Pixelnet output using Generic FTDI chipset USB to RS485 dongles.
	-enh (dkulp) During conversions, buffer non-critical updates to the messages list and append
		to the list box in blocks of about 10K.   Significantly speeds up conversions for HLS and LOR
		with very large files that display a lot of messages.
3.6.5	Nov 20, 2014
	-enh (sean) New effect class , Strobe.
	-enh (sean) Icons on top of each effect improved. This icons will be what you drag and drop effects in xLights 4.0. This early introduction
		is to help familiarize users with the icons to an effect.
	-bug (frankr) Non music sequences would freeze preview play. fixed
	-enh (djulien) Add Papagayo option for scaled vs. fixed picture mode
	-enh (djulien) Allow Papagayo grid row labels to be aliased (click on row label to change it); cosmetic only - does not change xml file
	-bug (djulien) Avoid "error 0" after writing new Papagayo file
    -bug (dkulp) SingleLine models with multiple string bug one node per string were displayed as a single light
    -bug (dkulp) Rotating single lines would cause the number of lights to display to drop by 2. (lose the ends)
    -enh (dkulp) Added pixel level "Skips" effect to SingleStrand effects.
    -enh (dkulp) Add ability to shift columns in the grid left or right to reorder them.
    -enh (dkulp) If the effect is NONE and the buffer is already clear, skip clearing it.  For large Whole House models, clearing the
                buffer can take time so skip it if we can.
    -bug (dkulp)  If the preview is too large for the viewport such that part of the preview was cut off
                at the top, clicking wasn't adjusting the Y coordinate to compensate when determining what
                was being clicked on.
3.6.4	Nov 12, 2014
	-bug (frankr) Fixed playback issue. If you play a sequence in the PREVIEW tab, stop, go and edit some effects in the NUYTCRACKER tab,
			press save and then go back to the PREVIEW tab, xlights would freeze up. This should be fixed now
	-enh (frankr) improvements to audio scrubbing and better handling of stop now button on
	-bug (frankr) When first entering teh PREVIEW tab , if you right click and select model xlights crashed. Fixed.
	-enh (sean) New effect "Off". This effect is the equivalent of "Colorwash, 1color=BLACK"
	-enh (sean) When saving a sequence the elapsed time for the save is now shown in the bottom status line.
	-enh (sean) New favicons have been added to the left most corner of Effects. The purpose of these is to have a graphical
			representation of effects. This is pre work for the new xlights 4.0 coming early next year. In xlights 4, effects will
			be dropped onto horizontal timelines, for for each model.
    -bug (dkulp) OSX - selecting cells on the Nutcracker grid was not updating the colours on the palette buttons
    -bug (dkulp) On OSX Yosemite, the Font dialogue for the text effect would cause a crash.
	-bug (dkulp) sometimes xlights would crash when you selected multiple objects and then right clicked to the alignment menu. Fixed

3.6.3	Nov 11, 2014
    -bug (dkulp) Fix problem of stars using a single RGB node per concentric layer not displaying correctly.
	-enh (sean) Moved "Create Whole House Model" button to be higher on the screen. This helps people with lower
		resolution computers.
	-enh (sean) reformatted Pinwheel screen so that it does not take as much screen to draw form.
3.6.2	Nov 7, 2014
    -enh (dkulp) Tree360 and Tree270 can now specify which strand to use for the first column when exporting and individual model.
                    This allows the exported models to work around issues with the 12:00 first strand for Nutcracker but the first
                    strand is expected to be the 9:00 position in HLS.
	-enh (frankr) New play,pause,stop buttons in PREVIEW screen. Moving the slider plays sequence data and plays sound,
		Move it to place where you want to start. Press PLAY, sequence starts from there. STOP button resets sequence to 0:0.0
	-enh (dpitts) New feature to select multiple objects in the PREVIEW window. 1st) Click and select first object that will be used
		as baseline. 2nd) Hold the shift key down and draw a rectangle. Any objects that fall within the rectangle will be selected.
		3rd) Right click and select how you want the selected objects to be aligned (Top, Bottom, Left, Right, Horizontal Center,
			Vertical Center). If items are stretched out horizontally, use Top/Bottom. If item are stretched out vertically, use
			Left/Right.
			A group that is highlighted in yellow can be moved anywhere on the screen, hold the ctrl key down before moving.
			Note: A highlighted group cannot be sized or rotated. That will be added in a future release.
		NOTE: If you screw something up, exit without saving and come back in. Later we will add an undo key.
		NOTE2: If you have been editing your preview and it looks good, do a Save on the preview screen. Also do a "File,Backup".
		This will save your current preview work AND make a copy of it in the Backup subdirectory.
3.6.1	Nov 6,2014
	-bug (dkulp) Pinwheel effect was not producing data in Preview screen or on lights, it is fixed now
	-enh (sean) Move Effect 1 and Effect 2 into tabs  . This gives more space for the preview screen
	-enh (dpitts) Selected objects in the preview are yellow , not magenta
3.6.0	Nov 6,2014
	-enh (dpitts) NUTCRACKER and PREVIEW windows now use OpenGL to draw objects. The result of this is xLights can now draw very
		large objects with no delay.
	-enh (dpitts) New ability to load an image into the background of the PREVIEW tab. There is a brightness slider to dim the image you
		just loaded to give it a dusk time feel.
	-enh (dpitts) New ability to set the resolution of the PREVIEW screen. Set the size and it will be remembered. This is needed to
				help when making Whole House Models. This way you can always come back to the layout you had been working on.
	-enh (dpitts) You can now click and drag objects in the PREVIEW tab. If there are multiple objects under your mouse click multiple
		times and it will cycle through all of them
	-enh (dpitts) Now objects are sized and rotated about their graphical center, This means objects do not move around
		the screen if you resize or rotate.
	-enh (dpitts) New rotate handles. Grab the handle and move mouse to rotate. Highlighted model is now magenta insteads of yellow
	-enh (sean) Added Pinwheel effect
	-bug (dpitts) Model Brightness Now works
	-enh (dkulp) Allow star layers to be ordered low to high (to inside layer first)
	-bug (captmurdoch,materdaddy,dkulp) Modify make files and libraries so 3.6.0 can be compiled on MAC and on Linux
3.5.2	Oct 30,2014
	-bug (frankr) Fix for preview moving some elements into the wrong place. Before the ability to rotate any element
		rotations were in 3 degree increments, now they are 1 degree. This change caused existing previews to draw elements in
		a new position. New code detects old rotation or new rotation values and draws both correctly.
	-bug (frankr) Fixed issue related to  views when switching to new show folder. Under some circumstances, xlights would crash.
3.5.1	Oct 30, 2014
	-bug (dpitts) Found bug that causes crash. Single color models that have multiple lights (think 1 string of single color
		with 50 lights uses 1 channel, but draws 50 places) was not allocating memory correctly for each pseudo light.
	-enh (frankr) Ability added to preview screen, you now can rotate any object not just single string.
3.5.0	Oct 29, 2014
	-enh (dpitts) New button added to the preview screen. This will create a Whole House Model for you automatically.
		There is now a new type of model "Whole House". You no longer need to use spreadsheets and the custom model.
		Procedure: Layout your preview. When everything is correct, press "Create Whole House Model". Give it a name.
		This new whole house model should NOT have "Part of my Display" checked.
		Add the new whole house model as a column in your Nutcracker grid.
		enjoy the new feature, many thanks to Dave Pitts!!
	-bug (dkulp) fixed issue with Star model with users sequences that were older than Oct 20th. This was crashing
		xlights at launch time.
3.4.24	Oct 28, 2014
	-enh (dpitts) New feature to allow creating views. A view is a collection of models. When you select a view it acts
		like a filter and only shows you the models contained in your view. T
    -enh (dkulp) Star model can now have multiple star size “layers” to model exploding star type things
                     Example: a 100 node star may have 50 for the outer layer, 30 for the middle, 20 for the inner.
	-bug (djulien) Fixed multi-cell morph (caused Papagayo flicker), avoid error if try to save file with no pgo data
	-enh (sean) Increased custom model from 300x300 to 1000x1000
3.4.23	Oct 25, 2014
	-enh (sean) Added Ripple Effect Class
	-bug (sean) Recompiled entire rpoject , we believe a few modeuls were compiled with debug and that was
				causing the error message in the model screen. Since that recompile, we have not see the error
	-bug (djulien) Fixed up auto-fade arithmetic (morph flicker still occurs); fix pgo grid column label with presets > 4 voices
	-enh (djulien) Papgayo: Rework auto-rest options, insert rests between phonemes
3.4.22	Oct 23, 2014
	-enh (sean) Added new effect class "Shimmer". This effect is basically a square wave, it turns all lights on and then all
		lights off.
	-enh (sean) Removed FACES, COROFACES and PINWHEEL from the "Create Random Effects" button
	-enh (djulien) Change Pgo "auto fade element" to "auto rest" (not functional yet), leave initial Pgo Preset cleared for stitch
3.4.21	Oct 20, 2014
	-enh (djulien) Added model CSV export to the Models screen. This csv file can be opened by Excel or
		Open Office spreadsheets
3.4.20	Oct 19, 2014
	-enh (djulien) Added Papagayo Preset stitch (allows Presets to be combined dynamically at run time)
	NOTES:
	 - Papagayo Presets match by model or voice name now rather than column#.  This means they can move around between columns and still work.
	 - The format of the xlights_papagayo.xml file (where pgo Presets are stored) changed but should be backward-compatible.
	 - Custom model info is cached with the Presets; if you change your Custom Models then re-Save the associated Presets
	 - workflow for horizontal (side-by-side) stitch of multiple pgo files + presets is as follows:
	   1. clear grid
	   2. load first pgo file
	   3. select corresponding Preset; this will "bind" the Preset info to the voice(s) in the pgo file
	   4. load next pgo file using "+" button, specify "0" for #frames to shift
	   5. select corresponding Preset
	   6. select output file if not already set
	   7. click button at the bottom to generate the sequence
	   8. load additional pgo files as desired; for vertical stitch (concatenate) enter a non-0 value for #frames to shift
3.4.19	Oct 18, 2014
	-bug (djulien) Fixed Coro Faces render problem on models with > 26 columns, adjust text size on model zoom in/out, Papagayo Preset delete button
	-enh (djulien) Added Papagayo horizontal stitch (supports any number of voices within reason), accept 39 extended phonemes (auto-map to basic 10), removed duplicate phoneme in frame message (CAUTION: phonemes will be dropped with no warnings), added Preset consistency check, start making Model dialog resizable, show reason for "reseting flags" message
3.4.18	Oct 13,2014
	-enh (djulien) Merged Papagayo sub-tabs; implemented Image option (Movie option TBD); enabled fade on Auto faces; added Copy column button; flattened xlights_papagayo.xml file (old Presets still used for backwards compatibility, but new Presets are written a level higher and merged)
	-bug (djulien) Fixed Papagayo auto-fade; mostly works now, but there seems to still be some flicker (caused by something else within xLights)
	-enh (sean)    New effect class PinWheel. This is NOT working yet, it is a place holder.
3.4.17	Oct 8, 2014
	PRE-RELEASE. There are still bugs we are working in the corofaces code. One we know of , outline does not show in PREVIEW
	-enh (djulien) This release sees the Papagayo Tab for Corofaces working.
					To use this, choose a pgo file as input, choose a file for output.
					Choose your custom models from a pull down list. Be sure they have "Part of my Display" checked.
					Assign channels to Phonemes. You can assign multiple channels by doing ctl-click.
					Create an xml file of the pgo.
3.4.16	Oct 1, 2014
	-enh (djulien) Add zoom in/out buttons on Custom Model grid display (makes it easier to see larger grids)
	-enh (djulien) On Papagayo Coro faces tab, moved model name into grid, show drop-down list of choices in grid cells, allow Pagagayo file stitch ("+" button), add UI controls for face element auto-fade, save/restore last tab, show list of checkboxes in Coro Faces effect panel
3.4.15  Sep 28, 2014
	- ENH (Sean) merge_xml.awk will combine multiple xlights xml sequences into one.
	- ENH (Sean) corofaces.awk awk now will stitch together multiple files for the same voice.
3.4.14  Sep 26, 2014
	- ENH (Sean) scripts added that can take Papagayo voices and make xlights sequences from them
	- ENH (Sean) Scripts are found in your installation directory. corofaces.awk, coroface.bat
	- ENH (Sean) Installation of xlights now installs gawk.
3.4.13	Sep 19, 2014
	-ENH (sean) The scale slider in the PREVIEW tab now goes to 300. before range was 1-100, now 1-300
	-bug (djulien) Avoid wx assert error during save/load pgo settings (bad grid row#), default to first group after loading settings (however, this can lead to mismatches), don't show "suspicious" message for missing voice data, list all models (even if not in Preview)
	-bug (djulien) Allow pgo parser to tolerate blank voices
3.4.12	Aug 19, 2014
        -ENH (dkulp) HLS import - now uses wxYield to display progress during load
        -ENH (dkulp) HLS import - if an HLS universe has less channels than configured in xLights, use the xLights value.
                     If the HLS universe has more, display a warning.
	-ENH (sean) 4th Open option of Nuctacrker, "New Animation Sequence", now allows fractional times. Before
		only values like "10,11,15" were allowed. Now "10.123,11.32123,15.2"
3.4.11	Aug 15, 2014
	-ENH (sean) Single Strand Effect now can use "Fit to Time".
		If this is checked, one cycle of an arch will be done.
		It will exactly fit the time on the current row of the grid.
	-Bug (sean) Two places in the code would cause a crash. Both have been fixed.
3.4.10	Aug 14, 2014
	-ENH (sean) complete rewrite of Single Strand effect. New sliders: Number of Chases,
		color (Rainbow,palette)
3.4.9	Aug 7, 2014
	-Bug (djulien) wxDateTime is broken again; force RFC822 format to try to get it to work;
	for example: "Thu, 25 Dec 2014 00:00:00 MST" or "Thu, 25 Dec 2014 00:00:00 -0700"
	can be used on line 1 of the text effect, select different display options from the countdown
	-Bug (sean) "Create Random Effects" would create a preponderance of BARS and BUTTERFLY effects. Effects are now random.
	-Bug (sean) xLights icon (Yellow Bulb), was not associated with the exe file. Shortcuts showed default icon. This now fixed.
3.4.8   Aug 3,2014
	-Enh (sean) Changed Playback counter on NUTCRACKER tab. It now updated 20 times per second instead of once per second.
	also changed the format to include fractional seconds. Finally added two displays, one has minutes, the second is
	just total seconds.
3.4.7   Jul 29,2014
	-Enh (sean) Added Model brightness adjustment into Model screen. Not functional yet, this is a pre-release
	-Bug (sean) Fixed export for single channel strings for LSP
3.4.6   Jul 23,2014
	-Enh (sean) Increased maximum time for a empty sequence from 300 seconds to 3600 seconds
3.4.5   Jul 15, 2014
        -Enh (dkulp) If an empty LMS channel is part of an RGB group, don’t consider it empty.
        -Enh (dkulp) Add a checkbox to TabConvert to go ahead and map the empty channels
        -Bug fix (mbrown) Fix Custom RGB Models that have multiple nodes with the same number
        -Bug fix (djulien) fix segv due to incorrect compiler code in Piano effect
        -Bug fix (mbrown) Custom models now work with traditional strings
3.4.4	Jul 2, 2014
	-Bug (dkulp) Fix LOR Import
	The old XML parser, on the start element event, you could query if the element was empty or not.
	For the "channel" element, if it was empty, we skipped it and didn't map it to a channel.
	The new parser doesn't have that functionality.   I have to record some information at the start tag,
	determine if there are any effect child elements, and if not, undo some stuff at the end element.
	The detection and undo wasn't working properly.  Thus, any empty channels in the LMS were getting mapped.
	Why is this a problem?   Well, LMS has a habit of using extra channels for things.
	For example, the CCR's which have 50 pixels would normally just be 150 channels.
	However, in the LMS file, they are 157 channels.  I have no idea what the LR, MM, ME, CM, CS, and CI
	channels that it puts for each CCR are used for.
	In any case, those extra 7 channels (and any other empty channels) would cause large shifts.
	This release has changes to fix that.
3.4.3	Jun 30, 2014
	-Enh (dkulp) Reduce memory usage of LMS import
        -Enh (dkulp) Change XML parser to spxml to avoid loading entire file into memory
3.4.2	Jun 26, 2014
        -Bug fix (dkulp) Problems loading large LMS files causing crashes/hangs
        -Enh (dkulp) Add additional status messages while loading LMS files
3.4.1	Jun 19, 2014
	-Bug fix (sean) Butterfly effect now creates pixels at {0,1} and {1,0} correctly
	-Enh (sean) added two new butterfly effects
	-Enh (sean) Work in progress. New Papagayo tab. This tab will be used to convert Papagayo files (.pgo) into
		xLight sequences
3.4.0   Jun 5, 2014
	-Bug fix (djulien) tried to fix Piano key wrap (again); added PianoTestNotes.txt file for easier testing (in case more changes are needed)
	-Bug fix (djulien) fixed cross-fade (morph); didn't write to xseq file correctly
	-Bug fix (djulien) add missing tool tip for Text center-and-stop checkbox
	-Enh (djulien) make Preview element size editable; not sure why it wasn't; NOTE: must have a Display Element selected first
3.3.9	May 22, 2014
	-Bug fix (sean). Glediator effects were short one row in the Y direction. This has now been fixed.
3.3.8	May 13, 2014
	-Enhancement (sean) New Effect, Glediator. Save a recorder file from Glediator. Open file and put in your grid.
	-Bug fix (sean). Glediator conversion now works. Select a *.gled file in the CONVERT tab and change into any of the output
		formats.
	-Bug fix, reported (djulien) draw white correctly if it's one of the Piano colors
	-Bug fix (Sean), Fixed Preview for Wave and Glediator. You could see this effects in Nutcracker tab but not in Preview.
		all fixed.
	-Enhancement, requested (djulien) center Piano keys if Clip is unchecked
3.3.7	May 9, 2014
	-Bug fix (dkulp) Added some additional checks to fix the heartbeat.cfg error.
3.3.6	May 08, 2014
	-Bug fix (djulien) avoid horiz or vert line flashes with plasma balls (Circles effect, Plasma option)
	-Bug fix (djulien) fix piano color caching (reported by multiple people)
	-Enhancement (sean) Added direction drop down for butterfly effect. "Normal" is the effect moving from
		right to left. this is the way it has always been. "Reverse" changes direction
	-Enhancement (sean) New file type in CONVERT tab, *.gled. Create these files using the record button of Glediator.
		Conversion code is still under development.
3.3.5	April 23, 2014
	-Enhancement (Sean). New effect called Wave. This effect will plot a Sine ,
		Triangle and square wave on your display.
	-Enhancement (djulien). ToopTip text added for Morph.
3.3.4	April 17, 2014
	-Enhancement, (sean). Faces effect now creates a scalable face on your matrix or megatree
	-Bug fix, (Sean). Entering a RETURN in the text fields of the the TEXT effect no longer crashes xLights
	-Enhancement, requested (djulien): add center-and-stop option to text movement options (new checkbox);
		sync text lines; use left/right spaces to delay left/right movement or left/right "\n" to delay up/down movement
	-Bug fix (djulien): allow "," within Text lines (caused "not found" error)
	-Enhancement (djulien): add MORPH checkbox to enable gradual Effect 1 -> Effect 2 transition;
		can be used for ramp/fade, morph text or image, etc.
	-Enhancement, requested (djulien): add menu item to clear all effects on this row
	-Bug fix (djulien): copy/paste text to Text effect Line 1 - 4, don't interfere with grid
3.3.3	April 3, 2014
	-Enhancement, requested (djulien): add Copy Effect Across Row menu item; copies selected cell (random effect)
		into other cells on the same row
	-Enhancement, (sean) Added two more lines of text to text effect. Now you can have 4 lines of text.
	-Bug fix (sean). Fixed LSP export. Yeah!
	-Enhancement, requested (djulien): allow user-specified format string with Countdown Text effect;
		format is <delim>target-date<delim>format-string
		for example: #4/1/2014 12:00:00#only %S seconds until noon on April 1!
		NOTE: don't use "," within Text; there is an XML parsing bug in xLights when the file is reloaded
	-Enhancement, requested (djulien): copy same random effect to all unlocked cells horizontally
		when Shift key used with Create Random Events button
	-Enhancement (djulien): write playback checkpoints to file for crash monitoring
	-Enhancement (djulien): add flag waving option to Pictures (useful for 4th of July, Memorial Day, etc)
3.3.2	Jan 28, 2014
	-Bug fix (dkulp): When importing LOR *.lms files, certain missing data
		would cause a crash. 	conversion now fills in any of those missing
		parameters and conversion completes.
3.3.1	Jan 27, 2014
	-Enhancement (djulien): change "Overlay" to "Persistent" for clarity;
	-Bug fix(djulien): save/restore value within XML
	-Enhancement (sean): add "1 reveals 2", "2 reveals 1" as new layer methods
	-Enhancement (dkulp): Don't assume LOR units are 16 channels to try and convert CCR based sequences.
3.3.0 Jan 7, 2014
	-Enhancement (djulien): vixen 2.x routine as picture
	-Enhancement (djulien): add checkbox to Effects panel to inhibit canvas clear before effect (for layers across models)
	-Enhancement (djulien): allow cancelled schedule to re-run that same day
	-Enhancement (djulien): show serial outbuf overflow amount
	-Enhancement (djulien): add a couple of icicle options to Meteors
	-Enhancement (djulien): add wavey left-to-right up/down option for text
	-Enhancement (djulien): added additional format options for date count-down
	-Enhancement (djulien): pass controller responses to plug-in (in-coming or round-trip data); internal
		change only - should not affect standard usage, but listed here just in case
	-Enhancement (djulien): add /noauto command line option to prompt for auto-run (allows easier schedule override)
	-Bug fix/enhancement? (djulien): show channel# being tested during Chase test
	-Bug fix/enhancement? (djulien): continue execution after comm output overflow in case controller can catch up
3.2.14	Dec 18, 2013
	-Enhancement (Sean): SingleStrand effects are now moving twice as fast.
	-Enhancement (Frankr) * Added alternating bands options to the bars effect.
	-Enhancement (Frankr) Removed limitation on pasting into the timings and label columns.
3.2.13	Dec 14, 2013
	-Bug fix (djulien): display some details about channel count error
	-Bug fix (djulien): split Effects Direction into separate lists for Text and Picture (some Picture are not applicable to Text)
	-Enhancement (djulien): More movement options in the pictures effect.
	-Enhancement (Sean): Added new song to songs directory.
		(It Must've Been Ol') Santa Claus, Harry Connick Jr.
3.2.12	Dec 11, 2013
	- Enhancement (Sean) Updated all files in songs directory to have readme files showing
		link to where to purchase music. Added some new songs.
3.2.11	Dec 6, 2013
	- Enhancement (Frankr) Nodes per string increased to 65536, You can now create a model that
		will do your entire house.
	-Bug fix (djulien): set Piano shape + map default file path to Piano subfolder within xLights folder, not up one level
3.2.10	Dec 5, 2013
	- Enhancement (Djulien) Piano effect now has been released
	- Enhancement (Frankr) Number of nodes on a single string has been increased from 300 to 1000
    - Enhancement (Matt) Added cut, copy, and paste to Nutcracker tab.
	- Enhancement (Sean) Create a subdirectory in songs for each song rather than all files in one
		place.
3.2.9 Nov 24, 2013
	- Enhancement (Matt) You can now open an LOR sequence from the Nutcracker tab.
        The first time you open an LOR sequence, the timing from the LOR file will
        be imported into the grid (only the times where effects start are imported).
    - Enhancement (Sean) Single Strand now has pull down to select chase types.
    - Enhancement (Sean) Single Strand now has 3d Fade checkbox to apply to chases
3.2.8	Nov 21, 2013
	- Enhancement (Sean) Re-release of single strand effects. Now they work without need to set
			"fit to time".
3.2.7	Nov 15, 2013
	- Bug (frankr) Windows model displayed wrong channel counts.
	- Enhancement (frankr) You can now export just one model instead of every channel that is in your setup
		1st method: "export" This does not look at models and instead exports every channel that has been defined
			in your setup Tab
		2nd method (new): "Export Model". You now can pick the model that is attached to your sequence and only export
			those channels. For example, I have megatree, arches, eaves. If I select megatree only the channels
			associated to that model will be exported. This will make much, much smaller LOR lms,lcb or vixen vix,vir files.
3.2.6	Nov 10, 2013
	- Bug (Sean) Removed Linus and Lucy mp3 file from songs directory, we don't distribute mp3's
	- Enhancement (Frankr and djulien). New hot keys created.
		F3 Toggles between PLAY/STOP for far left "Play Effect" button
		F4 Toggles between PLAY/STOP for top "Play" button
		F3 -- will start to play an effect (left play) if none is playing and no sequence is playing
			if an effect is playing it will stop playing. if a sequence is playing it will
			stop playing and the current effect will play.  This allows for easy editing of an effect.
		F4 -- Starts playing a sequence (top play) from the current selected cell
			If a sequence is playing it will be stopped.
		F5 Same as clicking the "Update Grid" button
		Ctl-o Same as clicking the open button
		Ctl-s save as clicking the save file button
3.2.5	Nov 9, 2013
	- Bug (Frankr) removed 'U' for updating grid. Now only F5 is shortcut key. The 'u' caused a problem
		when typing in text.
3.2.4	Nov 7, 2013
	- Enhancement (Djulien) New  filtering on every slider and checkbox for "Create Random Effects"
	- Enhancement (Frankr) F5 or 'U' now performs an update grid
	- Enhancement (Sean) New directory "songs" has audacity label files for many songs. Find the new directory
		at C:\Program Files (x86)\xLights\songs
3.2.3	Oct 25, 2013
	- Bug: (Dan) Schedule tab missing on Linux
    - Enhancement: (Dan) When outputting e1.31, only output the used channels instead of the full 512 universe.
		(Lower network traffic, less processing needed)
	- Bug: (Dan) Text Effect not rendering on OSX.  Fixed.
	- Enhancement: (Sean) New Single Strand Effect class. Use this for arches and eaves
3.2.2	Sep 26, 2013
	- Enhancement: (Dan) When importing hlsIdata, sort by the universes number to make the import predictable and repeatable.
	- Enhancement: (Dan) When importing hlsIdata, change the file extension for the Audio from PCM to mp3.
	- Bug: (Dan) If there is a named test setup, much of the functionality on the Setup tab (up/down arrows,
		modifying a network, etc..) did not work properly and could result in modifying the wrong network.  Fixed.
	- Bug: (Dan) Preview/Nutcracker tab drawing on the Mac would result in "ghosts" of the flashing lights due to anti-aliasing. Fixed.
	- Bug: (Dan) At certain screen sizes, some pixels in mega-trees and other Nutcracker models may not display.  Fixed.

3.2.1 Sep 13, 2013
	- Enhancement: (Frank). In the Model section you now see start and end channels, not just start
	- Bug: (Sean). When exporting HLS sequences if the number of channels was not evely divisible by 3, xLights would crash. Fixed.
	- Enhancement: (Frank). Switched the grids for start channel and custom model to be in scrolling
		windows so that larger data sets can be handled.
	- Bug: (Matt). Smart RGB arches are only producing a single channel. Fixed
	- Enhancement: (Matt). F1 will bring up help if you are in the Profile tab
3.2.0 Sep 12, 2013
	- Enhancement: (Matt). In the Model creation you can now select type of string that the model will use.
		There are new string types available. You can pick non-rgb strings. These will use a single channel, For preview purposes
		you can pick RED, GREEN,BLUE, WHITE as single colour strings.
	- Enhancement: (Matt). You can now select a string type that is strobe lights. This light will flash 3 times per second
		for each bulb.
	- Enhancement: (Matt). You can now pick a multi pointed star as a display element.
	- Enhancement: (Matt). You can select a wreath as a display element.
	- Enhancement: (matt). You can select a string type of 3 channel RGB (In other words dumb rgb strings)
	NOTE! With the change to the latest wxWidgets 2.9.5 , Mac 10.6.8 is no longer supported. We now will only support 10.7 and higher
3.1.4  Sep 4, 2013
	- Enhancement: (Matt). Custom Model now uses pixel count 1..n. Models can be put on preview. For example,
		you can define a candy cane out of rgb pixels and then place candy cane in the preview window. Very Nice!
    - Enhancement: (Matt) You can now copy models.
	- Enhancement: (Matt) You can now rotate single strand models in the preview window.
	- Enhancement: (Matt) There is a Sequence open button on the Preview tab. If you use this to open a sequence there is
		an option to set the "Part of my Display" checkboxes to match the models that just got opened.
3.1.3  Aug 27,2013
	- Enhancement: (Matt). New Effect Curtain.
	- Enhancement: (Matt)  New tab PREVIEW. There is a new tab that will allow you to see all of your
		models displaying effects.
	- Bug (Frank) Fixed channel numbering for single string and Window models
3.1.2  Aug 17,2013
	- Enhancement: (Frankr). Circle effect now has new option called plasma.
	- Enhancement: (Matt Brown).  bars compress & expand so that the two halves mirror each other.
	- Bug: (Frankr).  found and fixed the issues with fire and meteors that were being discussed at DIYC.
	- Enhancement (Dan Kulp). New import from HLS now available on CONVERT tab
	- Enhancement (Frankr). New option in spirals to allow the thickness to grow or shrink
3.1.1  Aug 12,2013
	- Bug Fix: (Matt Brown). Matt has fixed the Meteor effect. He has also enhanced it so meteors
		can fall to the left or right.
		We have	renamed	Starfield to be called "explode" and "implode"
	- Enhancement: (Matt Brown)  I made some improvements to countdown & skip blank cells logic. I also changed
		a couple of method names to make them more clear. Also, the timer now skips if the last interval takes more than 50ms to process.
		Before it was blocking until the last interval finished (so if every interval took more than 50ms you had a stackup of waiting timer calls to process).
3.1.0  Aug 7,2013
	- Enhancement: (frankr) new single BACKUP option now.
		Goto to "File,Backup" and every xml file you have will be created in the new Backup sub-directory under show show directory.
		How big are xLight sequences? The xseq file for 12K channels is around 30mbytes, the xml of your sequence is around 30kbytes. As comparison the XML file for LOR for the same song is 800mbytes, for LSP 1.2gbytes. Because xlight sequence files are so small (normally < 50kbytes), we can back them all up quickly.
		How long to back up over 100 sequences? < 1 second.
		How much space does 100 sequences take? < 10 mbytes
		How many times can you do this per day? 1000's.
		How to use this feature? Just do a "File, Backup" before getting new versions.
		I would do a copy if you plan on doing lots of edits.  Basically get yourself protected.
		So after upgrading to 3.1.0, go and make a copy before opening any sequences
	- Enhancement: (Matt Brown) re-engineered the underlying architecture. This is like replacing the galvanized pipe with copper pipe on your home. Takes lots of effort, when you are done the house still looks the same. This re-architecture was needed to allows us to offer new fit to time frame option. With this option effects will be enhanced to do things like start and end exactly to the timing marks on a row. Wait for future releases to see how this is going to help. Trust me, you will like the new things coming.
	- Enhancement: (Matt Brown) has enhanced the preview screen so it is multiple times brighter than before. So much nicer now!!
	- Enhancement: (frankr) made it so that when you select none, it will not overwrite existing data. Why do we care? This enhancement means a LSP person could sequence the first 30 seconds of a megatree (mark those rows as none,none in nutcracker). Now fill the rest of the sequence with nutcracker effects. The result? LSP effects will appear for the first 30 seconds, Nutcracker effects will appear for the remainder.
	- Enhancement:(Frankr) enhanced the right click process for protecting a cell. Before you had to get focus on the cell by left clicking (turned it yellow), then you could right click. Now just ricght click any cell, you dont need left click first
	- Enhancement: (Matt Brown) made it so effects are now listed alphabetically except for effects current under development (like the piano keyboard)
	- Enhancement: (sean) Sean added Radial 3d Button. Click this button besides the Radial button to get new radial effects
	- Bug Fix: (Sean, Matt, frankkr) . Found more division by zero errors, fixed them.
	- Enhancement: (Sean), For any  person who is pulling code and compiling it (Like MaterDaddy for Linux compiles). I have renamed repository nutcracker_c to be called xLights. Nutcracker_c was just a temp name I was using. I want everyone to remember we started with the xLights code base from Matt Brown in January of this  year. Nutcracker is just one tab on xllights. People get confused is it xLights? Is it Nutcracker? All of nutcracker has been put inside of xLights so the repository should be called the same. Again thanks to Matt for making this all possible. So here is new repository: https://github.com/smeighan/xLights
	- Enhancement: Please start to enter bugs into github so we can track them there. If you want to raise a bug go to https://github.com/smeighan/xLights/issues
	- Bug fix: (Matt) Meteors has been fixed by Matt's rewrite of the preview window. This means the normal Meteor effect is now working again.
		StarField still does not display on megatrees, it does work on other models.
3.0.24 Aug 4,2013
 	- Enhancement: (frankr) New fade in and fade out boxes on every effect. Enter the number of seconds to either fade in/out.
	- Enhancement: (sean) Started development of Piano Keyboard. This is not complete. Will eventually use an input file from
        Audacity that will identify the polyphonic notes in a song.
	- Enhancement: (matt) Many changes to text. Ghosting is now fixed. You no longer have to use the temporary fix of using
	   "1 is unmask, 2 is color wash". Text now has 45 degrees rotation. Countdown timer can show seconds counting down or
	   it can show "xx d yy h zz m aa s" display. countdown text is now centered correctly.
	- Enhancement: (frankr) New effect for circles. Circles can bounce, collide. Choose radial to make a new effect
	- Bug Fix: (sean) fixed crash when creating random effects. The new effect classes (piano,circles) were causing problem. fixed
	- Bug Fix: (sean) When randomly creating Spirograph effects, the animate button was not being selected. fixed.
	- Enhancement: (frankr) Cells in your effect grid can now be protected/un-protected. Protect a cell and
	   press the "create random effects" and all cells will get a new effect unless the cell is protected.
	   Under Development: Piano Effect. This is NOT done yet. when completed it will animate a piano
	   keyboard.
	- Enhancement: (frankr) Slider next to Layer choice. This slider allows you to pick how much of effect 1 and effect 2 show
	   through.

	- UNDER DEVELOPMENT: (sean) Piano Keyboard. Not functional yet. Will be pulled forward into release 25
3.0.23 Jul 28,2013
	- Enhancement: (sean) Fire effect now allows hue shift. You can now have green, blue fire.
    - Enhancement: (sean) Fire effect now has a "Grow Fire?". If you click yes, the height of the fire will grow over time
	- Enhancement: (frankr) Before if you changed or created a sequence and then clicked exit, program would just exit and you would lose your
		change. Now, you will be prompted if you want to save before exit.
	- Enhancement: (frankr) Random effect generation. When creating a Spirograph, the Animate button will always be set and the little radius will be
	  set to its minimum value. By setting r to zero and setting animate to true, random spirographs will now always produce an animation.
	- Enhancement: (frankr) New choice in model generation. You can now select custom. We are not quite done with this but it will support having
	  coro snowflakes, stars, strings wrapped around a car, fire sticks, candles,. Basically any model. Watch this space for a future tutorial.
3.0.22 Jul 26,2013
	- Enhancement: (frankr) New button to randomly create effects for every row in your sequence
    - Enhancement: (sean) Brightness slider now goes to 400% (Old value was 100%). This will allow improving movies
	- Bug Fix: (sean) If Tree effect was selected for Effect2 and slider was all the way to left, xlights crashed. fixed
	- Bug Fix: (sean) Removed the gauge that was next to Export button. The gauge library does not work in Unix.
3.0.21 Jul 25,2013
	- Enhancement: (frankr) In the Model setup screen, you can now set the starting channel for each string
    - Enhancement: (frankr) In the Model setup screen, you can now choose how the strings wrap. 3 more choices are now available
		bottom left, upper left, upper right, bottom right
	- Enhancement: (sean) Meteor Effect class has 3 new effects
		  1) Swirl1. As meteors fall they are moved in a sine wave
		  2) Swirl2. Same as Swirl 1 except odd strands move one way, even strands move the other
		  3) Strarfield. Meteor effect starts in center of your display and moves outwards
3.0.20 Jul 19,2013
	- Enhancement: (frankr) Highlight cell you are editing in so you know where "update" will apply
    - Bug Fix: (frankr) Opening a music file twice, the second time will not prompt for a new file name. Instead, it overwrites the first file. Fixed
	- Enhancement: (frankr) Ability to read an Audacity or a XML file when creating a blank sequence from a music file
	- Enhancement: (frankr) 1 is Unmask, 2 is Unmask now keeps the brightness value of the mask. This is big! Use a picture as a mask on a colorwash.
		  The new picture will be a mono tone version. Use text as a mask, the edges will feather but in a solid color.
		  This fixes the antio-aliasing issue of text. To fix use text as effect 1, color wash as effect2. set effect 2 color what you want your text
		  to look like
	- Bug Fix: (frankr) Deleting some models caused crash, fixed.
	- Bug Fix: (frankr) If you had multiple models attached to a sequence, deleting anything other than 1st model caused crash. fixed
3.0.19 Jul 17,2013
	- Bug Fix: (matt) Crash on exit fixed
    - Bug fix: (matt) Empty export files (LOR, hls,vixen,conductor) fixed
	- Enhancement: (sean) When reading in your sequence, missing attributes will be added if your file is from pre ver 18.
	- Enhancement: (sean) Changed rotation slider in Spirals so it does ten times more rotations.
	- Bug fix: (matt) If you have two lines of text but only one color selected, both lines will be same color
	NOTE!! Please make a backup of all your xml files. This release will be fixing your sequences, protect yourself.
3.0.18 Jul 06,2013
	- (sean) Added new Brightness and Contrast sliders. Located next to Sparkle slider it allows dimming the overall effect.
        Contrast will make brights brighter and sim colors go towards black.
3.0.17 Jul 01,2013
	- (sean) Fixed text in Effect2. Added Twinkle, Tree, Spirograph and Fireworks to Effect2
3.0.16 Jun 28,2013
	- (sean) New option added to the Text effect, Countdown timer.
3.0.15 Jun 27,2013
	- (sean) Added check to make it so empty picture selection does not cause hundreds of pop up windows
    - (sean) fixed LOR lms and lcb export
	- (sean) added check to make sure model's have at least 1 pixel wide and high
	- (sean) new feature in Meteors effect. Meteors can now fall up
3.0.14 Jun 13,2013
	- New movie files added to Picture Effect
        Use web page to change your movies into jpg files. http://image.online-convert.com/convert-to-jpg
    - Strobe in Twinkle Effect is now random lights
3.0.13 May 30,2013
	- New Strobe Effect now available in the Twinkle Effect Class
    - Twinkle Effect Class now allows different steps of twinkle.
3.0.12 May 26,2013
	- New type of vertical text where letters are not rotated but stacked on
      top of each other. Finally we have Line 1 one color, line 2 second color.
	  Each line of text can have a different font, a different direction.
	  There is a position bar for each line of text.
3.0.11 May 15,2013
	- Text can now be rotated 0, 90, 180 or 270 degrees
3.0.10 May 12,2013
	- Changed 32 character limit in the Text effect to be 256 characters
3.0.9  May 11,2013
	- Animated Gifs added to the Pictures effect
3.0.8  Apr 25,2013
	- Added export for LSP to create UserPatterns.xml
        The actual file created will have a *.user extension. rename it to use it
	- New Effect, Fireworks!. Dave Pitts coded up this new effect.
3.0.7  Apr 23,2013
	- Fixed Pallet 3 button. It was not picking up color when it was clicked.
    - Fixed export of Meteors. This effected all sequencers
	- Fixed export of Life. This effected all sequencers
	- Fixed export of Snowstorm. This effected all sequencers
3.0.6  Mar 11,2013
	- Corrected another bug in LOR lcb export.
    - Modifed Nutcracker tab to keep a minimum size screen on left window.
    - Added color selection based on distance from center of object for Spirograph.
    - Added the ability to animate the d parameter in spirograph
    - Moved all effects into their own source file.
    RGBEffects.cpp is now smaller and there are 20 new cpp
        files. RenderBars.cpp, RenderMeteors.cpp, .etc.
3.0.5  Mar 08,2013
	- Corrected color swap in lcb files. Corrected bug in HLS export.
3.0.4  Mar 04,2013
	- Added new effects, Tree and Spirograph.
3.0.3  Mar 02,2013
	- Added new effect, Twinkle.
3.0.2  Feb 28,2013
	- Corrected unsigned 24bit value for HLS *.hlsnc export.
3.0.1  Feb 26,2013
	- Added wxGauge to show process for exporting sequence files.
3.0.0  Feb 22,2013
	- Initial Release. This is Matt Browns original porting of Nutcracker into xLights.









