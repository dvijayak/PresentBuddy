import bb.cascades 1.0

Page {
    id: helpPage

    titleBar: TitleBar {
        id: titleBar
        kind: TitleBarKind.Default
        title: "Who are you, Buddy?"        
    }

    content: Container {
        layout: StackLayout {}

        verticalAlignment: VerticalAlignment.Center
        horizontalAlignment: HorizontalAlignment.Center
        
        ImageView {
            horizontalAlignment: HorizontalAlignment.Center
            imageSource: "asset:///info/app_icon_480x480.png"
            preferredWidth: 288
            preferredHeight: 288
        }
        
        // Welcome
        Divider {}
        
        Label {
            text: "Thanks for asking! I am here to help you improve on your talks and presentations by assisting you in mastering an important skill: keeping good time." 
            multiline: true
        }

        Divider {}
        
        Label {
            text: "You can use me to plot out the timeframe of your presentations and practise them in real-time. I will also indicate to you how far you are in each slide through colourful feedback."
            multiline: true
        }
        
        Divider {}
        
        Label {
            text: "When you feel that you have practised enough, feel free to take me with you to be your presentationbuddy during your actual talk!"
            multiline: true
        }

        Divider {}

//        Container {            
//            Label {
//                text: "If you like me, please tell your friends on BBM about me!"
//                multiline: true
//                textStyle {
//                    base: SystemDefaults.TextStyles.BigText
//                    textAlign: TextAlign.Center
//                    fontWeight: FontWeight.Bold
//                    fontSize: FontSize.PointValue
//                    fontSizeValue: 6
//                }
//                horizontalAlignment: HorizontalAlignment.Center
//            }
//            horizontalAlignment: HorizontalAlignment.Center
//        }

        /*// Main Screen
        Container {
            layout: StackLayout {}
            Label {
                text: "Main Page"	                
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
                    textAlign: TextAlign.Center
                    fontWeight: FontWeight.Bold
                    fontSize: FontSize.PointValue
                    fontSizeValue: 10
                }
                horizontalAlignment: HorizontalAlignment.Center
            }
            Divider {}
         }
        
        // New presentation
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }

            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center

            ImageView {
                imageSource: "asset:///icons/ic_add.png"
            }
            Label {                    
                text: "Create a new presentation"                
                multiline: true
            }
        }
        
        // Long press
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            ImageView {
                imageSource: "asset:///icons/ic_one_finger_hold.png"
            }
            Label {
                text: "Press and hold any presentation item for a few seconds and a context menu will creep up on the right providing the following listed options"
                multiline: true
            }
        }
        
        // Perform
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            ImageView {
                imageSource: "asset:///icons/9-av-play81.png"
            }
            Label {
                text: "<html><b>Perform: </b>play a slideshow of the presentation</html>"
                multiline: true
            }
        }
        // Preview
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            ImageView {
                imageSource: "asset:///icons/ic_view_details.png"
            }
            Label {
                text: "<html><b>Preview:</b>view the contents of the presentation</html>"
                multiline: true
            }
        }
        
        // Prepare
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            ImageView {
                imageSource: "asset:///icons/ic_edit.png"
            }
            Label {
                text: "<html><b>Prepare:</b>edit the contents of the presentation</html>"
                multiline: true
            }
        }
        
        // Perish
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            ImageView {
                imageSource: "asset:///icons/ic_delete.png"
            }
            Label {
                text: "<html><b>Perish:</b>Delete the entire presentation (irreversible)</html>"
                multiline: true
            }
        }*/
    }    
}