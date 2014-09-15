Present Buddy BB10 Task List
-------------------------------------------

Features
--------------
- (DONE) Implement the Preview page
- Implement the Practise feature
	- Find a good icon for this feature
	- Same as Perform except that you get to manually switch between slides
	- Automatically record time elapsed for each slide and update (to a certain threshold) if different from existing value
	- (optional) Start from a specific slide
- (DONE) Implement create new presentation feature
- Integrae app with Invocation Framework
	- (DONE) Create a calendar event
	- (optional) Allow users to manually edit the data file in a text editor or e-mail it to someone
	- (optional) Share a single presentation with a friend
	- (optional) Share your list of presentations with a friend
- Share all your presentations via NFC
	- (optional) Share a single presentation via NFC
- (DONE) Integrate app with BBM
	- Send an invitation to friends to download the app
	- (Possible through Invocation Framework) Share your list of presentations with a friend
	- (Possible through Invocation Framework) (optional) Share a single presentation with a friend
	- (NOT CURRENTLY POSSIBLE IN BB10) (optional) Collaborate together with a group of friends
		- Have everyone view your slideshow in real-time
	- BBM Features that sound relevant to me:
		- Invite BBM contacts to download your app or initiate a social interaction within your app.
		- Determine which BBM contact has downloaded your app.
		- (Possible through Invocation Framework) Share content with BBM contacts from within your apps.
		- (NOT CURRENTLY POSSIBLE IN BB10) Stream data between users of your applications for real-time activities such as communication, gaming, and location tracking.
- (DONE) (tentative) Change the UI to have buttons separate from list items
	- Only the Play button must exist in the ListItemComponent
	- All presentation actions appear in the context menu for each presentation (longpress anywhere on the element)
		- Context actions: Practise, Preview, Edit, Delete, (optional) Share
	- Clicking on a presentation takes you directly to the Edit page (or maybe the Preview page?)
	- Slides: each slide list item has context actions (longpress the slide number to access it)
		- Context actions: Delete (maybe without the confirmation box?), (optional) Reset, (optional) Swap
- (DONE) Make sure the app reads and writes to the correct file (pbuddy-data.json)
- (DONE) (optional) Produce a toast when launching app and reading an empty file OR when file does not exist OR when error opening file
- (optional) Implement the slide reset and slide swap features
- (optional) Ability to start from a specific slide if desired
- (optional) Implement preview page as a Card or Sheet instead of a whole page
- (ONLY ONE COLOUR SUPPORTED) (optional) Add progress bars (per slide and/or for the whole progress) to perform (and maybe practise)

Bugs
--------------
- (DONE) Time at the end of a presentation should change to LA FIN, instead of just a -1
- (DONE) Delete Presentation functionality is extremely buggy. Problems creep up when deleting presentations that were created using New Presentation.
- (DONE) App crashes when trying to perform a slideshow of a presentation with no slides
- After committing changes to a presentation, they won't be visible right away on preview page. You have to go forward and go back (or w/e) to see the changes
- (DONE) When going back from the perform page that was accessed from the preview page, the screen orientation remains in landscape mode until you go back to the main page
- (DONE) KeepAwake mode does not properly get disabled when returning to the main page
- (DONE) App crashes when I use Reset on a presentation that has 6 or more slides
- (DONE) App crashes when I move the Total Time Slider after deleting all slides

Fine-Tuning
---------------
- Adjust colour transition time periods
- Implement a better slider limiting logic
	- Min value of total time should be equal to or greater than max value of the highest slider immediate value
	- Sum of times of slides should be equal to or less than Total time value
- Improve the time sliders to allow choosing more discretely
	- (optional) Allow a Setting to specify a snap-to value (and implement this feature) if desired or free movement
- (DONE) Action bar takes up valuable space at the bottom of the screen while performing a slideshow
- (tentative) Replace time sliders into time pickers
- (DONE) (tentative) Refactor classes and code
- (tentative) Memory management (tip - attach classes that inherit from QObject by default to the parent)
- (optional) Make all list views display items asynchronously
- (optional) When there are no presentations and/or no slides, display a simple message "No presentations/slides"

Submission
-------------------
- Create app icon + splash screen + other submission assets
- Precompile assets (QML files)
- Clear folder of unnecessary files
- Compile a release version

