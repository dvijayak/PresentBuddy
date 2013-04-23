import bb.cascades 1.0

// Import the library that exposes the registered QTimer class
import my.library 1.0

// Performing page
Page {
    id: performPage
    objectName: "performPage"
    property variant feedbackColor;
    
	onCreationCompleted: {
	    // Connect signals with slots
	    Qt.appUI.performInitialized.connect(performPage.onPerformInitialized);
    }

    /* Initialize the UI with default values based on the active presentation */
    function onPerformInitialized() {
        Qt.activePresentation = Qt.appUI.activePresentation;
        performPage.feedbackColor = Color.White;

        // Initialize color transition logic
        Qt.hueTable = { // Pre-defined constants on the H axis for the specific colours we are working with
            green : 120,
            yellow: 60,
            red: 0
        }
        Qt.whiteToGreen = 0.10; // The time (percentage) spent in the initial transition from white to green        
        Qt.slideIndex = 0;
        Qt.elapsedTime = 0; // Total time elapsed in the presentation
        Qt.currentSlide = Qt.activePresentation.slides[Qt.slideIndex];
        performPage.initializeSlide(Qt.currentSlide);        
        
        // Start the presentation!
        countdownTimer.start();
        colorTimer.start();
    }

    content: Container {
        id: container

        background: Color.Black
        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }
        
        // Presentation Name
        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            Label {
                id: presentationName
                objectName: "performName"
                
                text: "Presentation Name"
                textStyle.color: performPage.feedbackColor
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
                textStyle.textAlign: TextAlign.Center
            }
        }
        
        // Slide Data 
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.TopToBottom
            }

            horizontalAlignment: HorizontalAlignment.Fill
            // Slide Title
            Label {
                id: slideTitle
                objectName: "performTitle"
                
                text: "Slide Title"
                textStyle.color: performPage.feedbackColor
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
                textStyle.textAlign: TextAlign.Center
            }
            
            // Slide Time Countdown
            Label {
                id: slideTime
                objectName: "performTime"
                
                text: "Slide Countdown"

                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
                    color: performPage.feedbackColor
                    textAlign: TextAlign.Center
                    fontWeight: FontWeight.Bold
                    fontSize: FontSize.PointValue
                    fontSizeValue: 80
                }
            }
        }
        
        // Time Info (footer)
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }

            horizontalAlignment: HorizontalAlignment.Fill
            // Time Elapsed
            Label {
                id: timeElapsed
                objectName: "performElapsed"
                
                text: "Time Elapsed"
                textStyle.color: performPage.feedbackColor
                verticalAlignment: VerticalAlignment.Center
            }
            
            // Presentation Total Time
            Label {
                id: presentationTotalTime
                objectName: "performTotalTime"
                
                text: "Total Time"
                textStyle.color: performPage.feedbackColor
                verticalAlignment: VerticalAlignment.Center
            }
        }


        
        attachedObjects: [
            QTimer {
                id: countdownTimer
                interval: 1000 // Time counts down by 1 second
                onTimeout: {
                    // Update the current time and the elapsed time
                    Qt.currentTime--;
                    Qt.elapsedTime++;
                    slideTime.text = Qt.appUI.timeToText(Qt.currentTime);                    
                    timeElapsed.text = Qt.appUI.timeToText(Qt.elapsedTime);
                    
                    // Count down until 0
                    if (Qt.currentTime > 0) {
                    	countdownTimer.start();
                    }
                    else {
                        // Switch to the next slide                                                
                        performPage.nextSlide();
                        
                        // Repeat until we finish the last slide
                        if (Qt.slideIndex < Qt.activePresentation.slides.length) {
                            countdownTimer.start();
                            colorTimer.start();
                        }
                        else {
                            countdownTimer.stop();
                            colorTimer.stop();
                        }
                    }
                }
            } ,            
            QTimer {
                id: colorTimer
                interval: 100
                onTimeout: {
                    /* Transition from white to red along the HSV color space */
                    
                    // White to Green
                    if (Qt.transitionValue <= Qt.colorMax 
                        && Qt.transitionValue >= Qt.whiteToGreenMax) {
                            // Only the Saturation (representing the 'whiteness') is incremented
                        	Qt.S = (Qt.colorMax-Qt.transitionValue)/(Qt.colorMax-Qt.whiteToGreenMax);
                    }                    
                    // Green to Red
                    else if (Qt.transitionValue < Qt.whiteToGreenMax
                        && Qt.transitionValue >= 0) {
                            // Only the Hue (representing the colour) is incremented
                            Qt.H = (Qt.transitionValue*(Qt.hueTable.green/360))/(Qt.whiteToGreenMax);
                    }

                    console.log("H: " + Math.floor(Qt.H*360) + " S: " + Math.floor(Qt.S*100) + " V: " + Math.floor(Qt.V*100));
//                    console.log("H: " + Qt.H + " S: " + Qt.S + " V: " + Qt.V);

                    // Convert HSV to RGB
                    var convertedColor = colorTimer.convertHSVToRGB(Qt.H, Qt.S, Qt.V);
                    var R = convertedColor.R,
                    	G = convertedColor.G,
                    	B = convertedColor.B;
                    
                    console.log("R: " + Math.floor(R*255) + " G: " + Math.floor(G*255) + " B: " + Math.floor(B*255));
//                    console.log("R: " + R + " G: " + G + " B: " + B);

                    // Update the color of elements to provide the visual feedback to the user
                    performPage.feedbackColor = Color.create(R, G, B);
                    
                    // Exit condition: when the time is up
                    if (Qt.transitionValue == 0) {
                        colorTimer.stop();
                    }
                    else {
                        // Decrement the time
                        Qt.transitionValue--;
                        
                        // Restart the timer
                        colorTimer.start();
                    }                                                                     
                }                                
                
                /* Convert a color from the HSV space to the RGB space.
                 * Implementation adapted from http://www.cs.rit.edu/~ncs/color/t_convert.html
                 * Retrieved April 23rd 2013. */
                function convertHSVToRGB (H, S, V) {                    
                    var R, G, B;                    
                    if (S == 0.0) {
                        R = V;
                        G = V;
                        B = V;
                    } else {
                        var h = H*6;
                        var i = Math.floor(h);
                        var p = V*(1-S);
                        var q = V*(1-S*(h-i));
                        var t = V*(1-S*(1-(h-i)));

                        if (i == 0) {
                            R = V;
                            G = t;
                            B = p;
                        } else if (i == 1) {
                            R = q;
                            G = V;
                            B = p;
                        } else if (i == 2) {
                            R = p;
                            G = V;
                            B = t;
                        } else if (i == 3) {
                            R = p;
                            G = q;
                            B = V;
                        } else if (i == 4) {
                            R = t;
                            G = p;
                            B = V;
                        } else {
                            R = V;
                            G = p;
                            B = q;
                        }                                        
                    }

                    return {"R": R, "G": G, "B": B};
                }

            }
        ]
    }
    
    /* Switch to the next slide and re-initialize */
    function nextSlide() {
    	Qt.slideIndex++;
    	if (Qt.slideIndex < Qt.activePresentation.slides.length) {
	        Qt.currentSlide = Qt.activePresentation.slides[Qt.slideIndex];
	        performPage.initializeSlide(Qt.currentSlide);
	        performPage.updateUIElements(Qt.currentSlide);
        }
    }
    
    /* Initialize the color transition logic for the specified slide */
    function initializeSlide(slide) {
        console.log("Slide " + Qt.slideIndex);
        Qt.maxTime = Qt.appUI.timeFromMinSecs(slide.time.minutes, slide.time.seconds);
        Qt.currentTime = Qt.maxTime; // The time that counts down
        // Initialize the components of the HSB/V (Hue, Saturation, Brightness/Value) colour space
        Qt.colorMax = Math.floor((Qt.maxTime * Math.floor(countdownTimer.interval / colorTimer.interval)));
        Qt.whiteToGreenMax = Math.floor(Qt.colorMax*(1-Qt.whiteToGreen)); // The amount of time spent in transitioning from white to green                    
        Qt.H = Qt.hueTable.green/360; // Initialize at green, as the first transition is white to green
        Qt.S = 0; // No saturation of the hue results in a pure white colour
        Qt.V = 1.0; // Maximum brightness at all times removes black shades
        Qt.transitionValue = Qt.colorMax; // Parameter representing the decrementing time
        Qt.transitionH = Qt.colorMax; // Parameter representing Hue transition
        Qt.transitionS = 0; // Parameter representing Saturation transition
        // No transition parameter for Brightness/Value as this never changes
    }
    
    /* Update UI controls to reflect the current slide */
    function updateUIElements(slide) {
        var title = slide.title;
        var time = Qt.appUI.timeToText(slide.time.minutes, slide.time.seconds);
        slideTitle.text = title;
        slideTime.text = time;
        performPage.feedbackColor = Color.White;
    }
}
