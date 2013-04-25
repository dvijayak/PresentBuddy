Present Buddy BB10 TODO
-------------------------------------------

Features
--------------
- (DONE) Implement the Preview page
- Implement the Practise feature
	- Find a good icon for this feature
	- Same as Perform except that you get to manually switch between slides
	- Automatically record time elapsed for each slide and update (to a certain threshold) if different from existing value
	- (optional) Start from a specific slide
- Implement create new presentation feature
- Integrate app with BBM
	- Send an invitation to friends to download the app
	- Share your list of presentations with a friend
	- (optional) Share a single presentation with a friend
	- (optional) Collaborate together with a group of friends
		- Have everyone view your slideshow in real-time
	- BBM Features that sound relevant to me:
		- Invite BBM contacts to download your app or initiate a social interaction within your app.
		- Determine which BBM contact has downloaded your app.
		- Share content with BBM contacts from within your apps.
		- Stream data between users of your applications for real-time activities such as communication, gaming, and location tracking.
- (tentative) Change the UI to have buttons separate from list items
	- Only the Play button must exist in the ListItemComponent
	- All presentation actions appear in the context menu for each presentation (longpress anywhere on the element)
		- Context actions: Practise, Preview, Edit, Delete, (optional) Share
	- Clicking on a presentation takes you directly to the Edit page (or maybe the Preview page?)
	- Slides: each slide list item has context actions (longpress the slide number to access it)
		- Context actions: Delete (maybe without the confirmation box?), (optional) Reset, (optional) Swap
- (DONE) Make sure the app reads and writes to the correct file (pbuddy-data.json)
- (DONE) (optional) Produce a toast when launching app and reading an empty file OR when file does not exist OR when error opening file
- (tentative) Make all list views display items asynchronously
- (optional) Implement the slide reset and slide swap features
- (optional) Ability to start from a specific slide if desired
- (optional) Add progress bars (per slide and/or for the whole progress) to perform (and maybe practise)

Bugs
--------------
- After committing changes to a presentation, they won't be visible right away on preview page. You have to go forward and go back (or w/e) to see the changes
- (DONE) When going back from the perform page that was accessed from the preview page, the screen orientation remains in landscape mode until you go back to the main page
- (DONE) KeepAwake mode does not properly get disabled when returning to the main page
- (DONE) App crashes when I use Reset on a presentation that has 6 or more slides
- (DONE) App crashes when I move the Total Time Slider after deleting all slides

Fine-Tuning
---------------
- Adjust colour transition time periods
- Implement a better slider limiting logic
- Improve the time sliders to allow choosing more discretely
	- (optional) Allow a Setting to specify a snap-to value (and implement this feature) if desired or free movement
- Action bar takes up valuable space at the bottom of the screen while performing a slideshow
- (tentative) Replace time sliders into time pickers
- (DONE) (tentative) Refactor classes and code
- (optional) When there are no presentations and/or no slides, display a simple message "No presentations/slides"

Submission
-------------------
- Create app icon + splash screen + other submission assets
- Precompile assets (QML files)
- Compile a release version

