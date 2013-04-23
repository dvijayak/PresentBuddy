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
        Qt.slideIndex = 0;
        Qt.elapsedTime = 0; // Total time elapsed in the presentation
        Qt.currentSlide = Qt.activePresentation.slides[Qt.slideIndex];
        performPage.initializeSlide(Qt.currentSlide);
        Qt.rgbDiff = 1; // Increment/Decrement parameter
        
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
                    // Transition the colours
                    // White to Green
                    if (Qt.rValue <= Qt.colorMax && Qt.gValue == Qt.colorMax && Qt.bValue <= Qt.colorMax 
                        	&& Qt.rValue != 0 && Qt.bValue != 0) {
                        Qt.rValue -= Qt.rgbDiff;
                        Qt.bValue -= Qt.rgbDiff;
                    }
                    // Green to Yellow
                    else if (Qt.rValue >= 0 && Qt.gValue == Qt.colorMax && Qt.bValue == 0 
                        	&& Qt.rValue != Qt.colorMax) {
                        Qt.rValue += Qt.rgbDiff;
                    } 
                    // Yellow to Red
                    else if (Qt.rValue == Qt.colorMax && Qt.gValue <= Qt.colorMax && Qt.bValue == 0 
                        	&& Qt.gValue != 0) {                        
                        Qt.gValue -= Qt.rgbDiff;
                    }
                    
                    // Rectify invalid values
                    Qt.rValue = colorTimer.correctRGBValues(Qt.rValue, Qt.colorMax);
                    Qt.gValue = colorTimer.correctRGBValues(Qt.gValue, Qt.colorMax);
                    Qt.bValue = colorTimer.correctRGBValues(Qt.bValue, Qt.colorMax);

                    // Divide by colorMax to get a double/float percentage
					var R = Qt.rValue/Qt.colorMax, 
						G = Qt.gValue/Qt.colorMax, 
						B = Qt.bValue/Qt.colorMax;
					
					// Update the color of elements to provide the visual feedback to the user
					performPage.feedbackColor = Color.create(R, G, B);				
                    console.log("R: " + Math.floor(R*256) + " G: " + Math.floor(G*256) + " B: " + Math.floor(B*256));
//                    console.log("R: " + Math.floor(Qt.rValue) + " G: " + Math.floor(Qt.gValue) + " B: " + Math.floor(Qt.bValue));

                    // We have reached Red
                    if (Qt.rValue == Qt.colorMax && Qt.gValue == 0 && Qt.bValue == 0) {
                        colorTimer.stop();                                        
                    }
                    else {
                        colorTimer.start();
                    }                    
                }                                

                // Boundary cases where values < 0 or > maxTime are possible when dec or inc by some numbers
                function correctRGBValues (value, max) {                    
                    if (value < 0) {                        
                        return 0;
                    }
                    else if (value > max) {                        
                        return max;
                    }
                    else {                        
                        return value;
                    }
                }
            }
        ]
    }
    
    /* Switch to the next slide and re-initialize */
    function nextSlide() {
    	Qt.slideIndex++;    	
        Qt.currentSlide = Qt.activePresentation.slides[Qt.slideIndex];
        performPage.initializeSlide(Qt.currentSlide);
        performPage.updateUIElements(Qt.currentSlide);
    }
    
    /* Initialize the color transition logic for the specified slide */
    function initializeSlide(slide) {
        console.log("Slide " + Qt.slideIndex);
        Qt.maxTime = Qt.appUI.timeFromMinSecs(slide.time.minutes, slide.time.seconds);
        Qt.colorMax = Math.floor((Qt.maxTime * Math.floor(countdownTimer.interval / colorTimer.interval)) / 3); // We divide by 3 since we have three colour transitions
        Qt.currentTime = Qt.maxTime; // The time that counts down        
        console.log(Qt.maxTime + "," + Qt.colorMax);
        Qt.orangeTime = Math.floor(Qt.colorMax * 0.65); // The time at which the feedback colo is orange - about 65% of max time
        console.log("ORANGE TIME = " + Qt.orangeTime);
        Qt.rValue = Qt.colorMax; // R
        Qt.gValue = Qt.colorMax; // G
        Qt.bValue = Qt.colorMax; // B
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
