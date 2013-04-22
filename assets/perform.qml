import bb.cascades 1.0

// Import the library that exposes the registered QTimer class
//import my.library 1.0

// Performing page
Page {
    id: performPage
    objectName: "performPage"
//    property variant activePresentation;
    property variant feedbackColor;

    actions: [
        ActionItem {
            objectName: "playButton"
            
            title: "Begin Presenting"
            imageSource: "asset:///icons/9-av-play81.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            
            onTriggered: {
                timer.start();
            }
        }
    ]
    
    
    // Initialize the UI with default values based on the active presentation
    function onPerformInitialized() {
        Qt.activePresentation = Qt.appUI.activePresentation;
        performPage.feedbackColor = Color.White;

		// Initialize color transition logic
        Qt.maxTime = Math.floor(600 / 3);
        Qt.orangeTime = Math.floor(Qt.maxTime * 0.65); // The time at which the colour indicator turns orange - about 65% of total time
        console.log(Qt.orangeTime + "ORANGE TIME");
        Qt.rValue = Qt.maxTime; // R
        Qt.gValue = Qt.maxTime; // G
        Qt.bValue = Qt.maxTime; // B
        Qt.rgbDiff = 1; // Increment/Decrement parameter
    }
    
	onCreationCompleted: {
	    // Connect signals with slots
	    Qt.appUI.performInitialized.connect(performPage.onPerformInitialized);
    }	
    content: Container {
        id: container

        background: Color.Black
        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }
        
        // Presentation Name
        Container {
            Label {
                id: presentationName
                objectName: "performName"
                
                text: "Presentation Name"
                textStyle.color: performPage.feedbackColor
            }
        }
        
        // Slide Data 
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.TopToBottom
            }
            
            // Slide Title
            Label {
                id: slideTitle
                objectName: "performTitle"
                
                text: "Slide Title"
                textStyle.color: performPage.feedbackColor
            }
            
            // Slide Time Countdown
            Label {
                id: slideTime
                objectName: "performTime"
                
                text: "Slide Countdown"
                textStyle.color: performPage.feedbackColor
            }
        }
        
        // Time Info (footer)
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            
            // Time Elapsed
            Label {
                id: timeElapsed
                objectName: "performElapsed"
                
                text: "Time Elapsed"
                textStyle.color: performPage.feedbackColor
            }
            
            // Presentation Total Time
            Label {
                id: presentationTotalTime
                objectName: "performTotalTime"
                
                text: "Total Time"
                textStyle.color: performPage.feedbackColor
            }
        }


        
//        attachedObjects: [
//            QTimer {
//                id: timer
//                interval: 100
//                onTimeout: {
////                    console.log("R: " + Qt.rValue + " G: " + Qt.gValue + " B: " + Qt.bValue);
//                    // White to Green
//                    if (Qt.rValue <= Qt.maxTime && Qt.gValue == Qt.maxTime && Qt.bValue <= Qt.maxTime 
//                        	&& Qt.rValue != 0 && Qt.bValue != 0) {
//                        Qt.rValue -= Qt.rgbDiff;
//                        Qt.bValue -= Qt.rgbDiff;
//                    }
//                    // Green to Yellow
//                    else if (Qt.rValue >= 0 && Qt.gValue == Qt.maxTime && Qt.bValue == 0 
//                        	&& Qt.rValue != Qt.maxTime) {
//                        Qt.rValue += Qt.rgbDiff;
//                    } 
//                    // Yellow to Orange
//                    else if (Qt.rValue == Qt.maxTime && Qt.gValue <= Qt.maxTime && Qt.bValue == 0 
//                        	&& Qt.gValue != Qt.orangeTime) {                        
//                        Qt.gValue -= Qt.rgbDiff;
//                    }
//                    // Orange to Red (can technically be combined with Yellow to Orange)
//                    else if (Qt.rValue == Qt.maxTime && Qt.gValue <= Qt.orangeTime && Qt.bValue == 0 
//                        	&& Qt.gValue != 0) {
//                        Qt.gValue -= Qt.rgbDiff;
//                    }
//                                        
//                    Qt.rValue = timer.correctRGBValues(Qt.rValue, Qt.maxTime);
//                    Qt.gValue = timer.correctRGBValues(Qt.gValue, Qt.maxTime);
//                    Qt.bValue = timer.correctRGBValues(Qt.bValue, Qt.maxTime);
//
//                    // Divide by maxTime to get a double/float percentage
//					var R = Qt.rValue/Qt.maxTime, 
//						G = Qt.gValue/Qt.maxTime, 
//						B = Qt.bValue/Qt.maxTime;
//					
//                    container.background = Color.create(R, G, B);
//                    console.log("R: " + R*256 + " G: " + G*256 + " B: " + B*256);
//
//                    // We have reached Red
//                    if (Qt.rValue == Qt.maxTime && Qt.gValue == 0 && Qt.bValue == 0) {
//                        timer.stop();
//                    }
//                    else {
//                        timer.start();
//                    }                    
//                }
//
//                // Boundary cases where values < 0 or > maxTime are possible when dec or inc by some numbers
//                function correctRGBValues (value, maxTime) {                    
//                    if (value < 0) {                        
//                        return 0;
//                    }
//                    else if (value > maxTime) {                        
//                        return maxTime;
//                    }
//                    else {                        
//                        return value;
//                    }
//                }
//            }
//        ]
    }
    
//    paneProperties: NavigationPaneProperties {
//        backButton: ActionItem {
//            title: "Previous Page"
//            imageSource: "asset:///icons/ic_previous.png"
//            onTriggered: {
//                navigationPane.pop();
//            }
//        }
//    }
}
