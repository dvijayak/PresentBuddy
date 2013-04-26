import bb.cascades 1.0
// Import the library that exposes the registered QTimer class
import my.library 1.0

Sheet {
    id: loadingScreen
    objectName: "loadingScreen"
    
    attachedObjects: [
        QTimer {
            id: timer
            interval: 1000
            onTimeout: {
                loadingScreen.close()
            }
        }
    ]
    
    onCreationCompleted: {                
        if (!loadingIndicator.running) {
        	loadingIndicator.start();
        }
        timer.start();
    }
    
    content: Page {
	    Container {
	        layout: StackLayout{}
	        bottomPadding: 50
	        	        
	        verticalAlignment: VerticalAlignment.Center
	        
	        Label {
	            text: "Present Buddy"
	            multiline: true
                textStyle.color: Color.White
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
                    textAlign: TextAlign.Center
                    fontWeight: FontWeight.Bold
                    fontSize: FontSize.PointValue
                    fontSizeValue: 30
                }
                horizontalAlignment: HorizontalAlignment.Center
            }	        
	        
	        ImageView {
                horizontalAlignment: HorizontalAlignment.Center
                imageSource: "asset:///info/app_icon_480x480.png"                
            }
	        
            ActivityIndicator {
                id: loadingIndicator                
                horizontalAlignment: HorizontalAlignment.Center
            }                    
        }
    }
}