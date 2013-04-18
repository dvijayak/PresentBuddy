import bb.cascades 1.0

// Import the library that exposes the registered QTimer class
import my.library 1.0

// Performing page
Page {
    id: performPage
    objectName: "performPage"
    
//    property alias containerBackground: container.background
	onCreationCompleted: {
	    Qt.maxTime = Math.floor(600/3);
	    Qt.orangeTime = Math.floor(Qt.maxTime*0.65); // The time at which the colour indicator turns orange - about 65% of total time
	    console.log(Qt.orangeTime + "ORANGE TIME");
        Qt.rValue = Qt.maxTime; // R
        Qt.gValue = Qt.maxTime; // G
        Qt.bValue = Qt.maxTime; // B
        Qt.rgbDiff = 1; // Increment/Decrement parameter
    }	
    content: Container {
        id: container        
        background: Color.create("#ffffffff")
        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }

        Container {
            
            background: Color.Black            
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
            translationY: 100.0
            Label {
                text: "Slide Title Appears Here"
            }
        }
        Container {

            background: Color.Black
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
            translationY: 200.0
            Label {
                text: "Countdown Appears Here"
            }
        }

        onCreationCompleted: {
            timer.start();
        }
        
        attachedObjects: [
            QTimer {
                id: timer
                interval: 100
                onTimeout: {
//                    console.log("R: " + Qt.rValue + " G: " + Qt.gValue + " B: " + Qt.bValue);
                    // White to Green
                    if (Qt.rValue <= Qt.maxTime && Qt.gValue == Qt.maxTime && Qt.bValue <= Qt.maxTime 
                        	&& Qt.rValue != 0 && Qt.bValue != 0) {
                        Qt.rValue -= Qt.rgbDiff;
                        Qt.bValue -= Qt.rgbDiff;
                    }
                    // Green to Yellow
                    else if (Qt.rValue >= 0 && Qt.gValue == Qt.maxTime && Qt.bValue == 0 
                        	&& Qt.rValue != Qt.maxTime) {
                        Qt.rValue += Qt.rgbDiff;
                    } 
                    // Yellow to Orange
                    else if (Qt.rValue == Qt.maxTime && Qt.gValue <= Qt.maxTime && Qt.bValue == 0 
                        	&& Qt.gValue != Qt.orangeTime) {                        
                        Qt.gValue -= Qt.rgbDiff;
                    }
                    // Orange to Red (can technically be combined with Yellow to Orange)
                    else if (Qt.rValue == Qt.maxTime && Qt.gValue <= Qt.orangeTime && Qt.bValue == 0 
                        	&& Qt.gValue != 0) {
                        Qt.gValue -= Qt.rgbDiff;
                    }
                                        
                    Qt.rValue = timer.correctRGBValues(Qt.rValue, Qt.maxTime);
                    Qt.gValue = timer.correctRGBValues(Qt.gValue, Qt.maxTime);
                    Qt.bValue = timer.correctRGBValues(Qt.bValue, Qt.maxTime);

                    // Divide by maxTime to get a double/float percentage
					var R = Qt.rValue/Qt.maxTime, 
						G = Qt.gValue/Qt.maxTime, 
						B = Qt.bValue/Qt.maxTime;
					
                    container.background = Color.create(R, G, B);
                    console.log("R: " + R*256 + " G: " + G*256 + " B: " + B*256);

                    // We have reached Red
                    if (Qt.rValue == Qt.maxTime && Qt.gValue == 0 && Qt.bValue == 0) {
                        timer.stop();
                    }
                    else {
                        timer.start();
                    }                    
                }

                // Boundary cases where values < 0 or > maxTime are possible when dec or inc by some numbers
                function correctRGBValues (value, maxTime) {                    
                    if (value < 0) {                        
                        return 0;
                    }
                    else if (value > maxTime) {                        
                        return maxTime;
                    }
                    else {                        
                        return value;
                    }
                }
            }
        ]
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
