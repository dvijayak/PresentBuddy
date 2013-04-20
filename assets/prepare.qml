import bb.cascades 1.0
import bb.system 1.0

Page {
    id: preparePage    
    objectName: "preparePage"
    
    actions: [
        ActionItem {
            title: "New Slide"
            objectName: "newButton"
            
            imageSource: "asset:///icons/ic_add.png"
            ActionBar.placement: ActionBarPlacement.OnBar
        } ,
        ActionItem {
            title: "Done"
            objectName: "doneButton"
            
            imageSource: "asset:///icons/ic_done.png"
            ActionBar.placement: ActionBarPlacement.OnBar            
            onTriggered: {                
                // TODO Save all changes to the respective presentation object
                savedChangesToast.show();
                Qt.navigationPane.pop();                                
            }
            attachedObjects: [
                SystemToast {
                    id: savedChangesToast
                    body: "Successfully saved changes."                    
                }
            ]
        }        
    ]
	
    paneProperties: NavigationPaneProperties {
        // Custom back button
        backButton: ActionItem {
            title: "Back"
            imageSource: "asset:///icons/ic_previous.png"
            onTriggered: {
                discardedChangesToast.show();
                Qt.navigationPane.pop();
            }
            attachedObjects: [
                SystemToast {
                    id: discardedChangesToast
                    body: "Discarded all changes."
                }
            ]
        }
    }

    content: Container {        
        
        // Presentation MetaData
        Container {
            layout: StackLayout {}
            
            // Presentation Name
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                verticalAlignment: VerticalAlignment.Center
                Label {
                    text: "Name"
                    verticalAlignment: VerticalAlignment.Center
                }
                TextField {
                    id: nameText
                    objectName: "nameText"                    
                    hintText: "Presentation Name"
                    verticalAlignment: VerticalAlignment.Center

                    onCreationCompleted: {
                        nameText.textChanged.connect(Qt.appUI.bufferNameChange);
                    }
                }
            }
            
            // Presentation Total Time
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                
                verticalAlignment: VerticalAlignment.Center
                Label {
                    text: "Total Time"
                    verticalAlignment: VerticalAlignment.Center
                }
                Label {
                    id: totalTimeValueLabel
                    objectName: "totalTimeValueLabel"
                    preferredWidth: 120
                    maxWidth: 120
                    textStyle.color: Color.Cyan
                    textStyle.textAlign: TextAlign.Left                    
                }
            }    
            
            // Presentation Total Time Slider
            Slider {
                id: totalTimeSlider
                objectName: "totalTimeSlider"
                property double maxTimeRemaining; // The maximum amount of time remaining that can be allocated to a slide
                fromValue: 1.0
                toValue: 3600.0
                horizontalAlignment: HorizontalAlignment.Fill
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1.0
                }

                onImmediateValueChanged: {
                    var value = Math.floor(immediateValue);
                    var minute = Math.floor(value / 60);
                    var second = Math.floor(value % 60);
                    totalTimeValueLabel.text = minute + ":" + second;
                }
                
                onCreationCompleted: {
                    totalTimeSlider.valueChanged.connect(Qt.appUI.bufferTotalTimeChange);
                }

            }
        } // end of MetaData

        Divider {}

		// Slide list visuals
		Container {
            ListView {
                id: slideListView
                objectName: "slideListView"
                // TODO Might have to manually declare data model

                listItemComponents: [                    
                    ListItemComponent {
                        type: ""
                        Container {
                            id: listItemRoot
                            objectName: "slideListItem" // Allows to findChildren<Container*>("slideListItem") and work with each individual form object
                            
                            Container {                                
                                layout: StackLayout {
                                    orientation: LayoutOrientation.LeftToRight
                                }
                                // Image View/Slide Number
                                Container {                                                                    
                                    Label {
//                                        preferredWidth: 150
                                        text: listItemRoot.ListItem.indexInSection + 1
                                        verticalAlignment: VerticalAlignment.Fill
                                        horizontalAlignment: HorizontalAlignment.Fill
                                        textStyle {
                                            base: SystemDefaults.TextStyles.BigText
                                            textAlign: TextAlign.Center
                                            fontWeight: FontWeight.Bold
                                            fontSize: FontSize.PointValue
                                            fontSizeValue: 35
                                        }

                                    }
                                }
                                // Slide Data
                                Container {
                                    layout: StackLayout {
                                        orientation: LayoutOrientation.TopToBottom
                                    }
                                    // Slide title
                                    Container {
                                        layout: StackLayout {
                                            orientation: LayoutOrientation.LeftToRight
                                        }
                                        Label {
                                            text: "Title"
                                            verticalAlignment: VerticalAlignment.Center
                                        }
                                        TextField {
                                            objectName: "slideTitleText"
                                            text: ListItemData.title
                                            hintText: "Slide title/heading"
                                            verticalAlignment: VerticalAlignment.Center
                                            
                                            onTextChanged: {
                                                Qt.appUI.bufferSlideTitleChange(listItemRoot.ListItem.indexInSection, text);                                                
                                            }
                                        }
                                    }
                                    // Slide time allocation labels
                                    Container {
                                        layout: StackLayout {
                                            orientation: LayoutOrientation.LeftToRight
                                        }
                                        Label {
                                            text: "Time"
                                            verticalAlignment: VerticalAlignment.Center
                                        }
                                        Label {
                                            id: timeValueLabel
                                            objectName: "slideTimeValue"
                                            
                                            textStyle.color: Color.Cyan
                                            textStyle.textAlign: TextAlign.Left

                                            onCreationCompleted: {
                                                var value = Math.floor(slideTimeSlider.immediateValue);
                                                var minute = Math.floor(value / 60);
                                                var second = Math.floor(value % 60);
                                                timeValueLabel.text = minute + ":" + second;
                                            }
                                        }
                                    }
                                    // Slide time allocation slider
                                    Container {
                                        Slider {
                                            id: slideTimeSlider
                                            objectName: "slideTimeSlider"
                                            
                                            horizontalAlignment: HorizontalAlignment.Fill
                                            layoutProperties: StackLayoutProperties {
                                                spaceQuota: 1.0
                                            }
                                            value: (60 * ListItemData.time.minutes) + ListItemData.time.seconds
                                            fromValue: 1.0
                                            toValue: 3600.0
                                            onImmediateValueChanged: {
                                                var value = Math.floor(immediateValue);
                                                var minute = Math.floor(value / 60);
                                                var second = Math.floor(value % 60);
                                                timeValueLabel.text = minute + ":" + second;
                                            }

                                            onValueChanged: {
                                                Qt.appUI.bufferSlideTimeChange(listItemRoot.ListItem.indexInSection, value);
                                            }
                                        }
                                    }
                                }                                
                            }
                            
                            Divider {}
                        }
                    } // end of ListItemComponent
                ] // end of listItemComponents
            } // end of ListView
        }        
		
    } // End of content Container
}
