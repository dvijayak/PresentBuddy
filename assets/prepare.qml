import bb.cascades 1.0
import bb.system 1.0

Page {
    id: preparePage    
    objectName: "preparePage"
    
    actions: [
        ActionItem {
            title: "Done"
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
            imageSource: "asset:///icons/ic_back.png"
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
                    text: ""
                    hintText: "Presentation Name"
                    verticalAlignment: VerticalAlignment.Center
                    
                    onCreationCompleted: {
                        nameText.text = Qt.appUI.presentations[Qt.chosenPresentationIndex].name;
                        console.log("HAHAH: " + Qt.chosenPresentationIndex);
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
                Slider {
                    id: totalTimeSlider
                    property double maxTimeRemaining; // The maximum amount of time remaining that can be allocated to a slide
                    value: 600.0
                    fromValue: 0.0
                    toValue: 7200.0
                    horizontalAlignment: HorizontalAlignment.Fill
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 1.0
                    }
                    
                    onImmediateValueChanged: {                     
                    }

                }
            }                                            
        } // end of MetaData

        Divider {}

		// Slide list visuals
		Container {
            attachedObjects: [
                // TODO Should I have a data model here?
            ]

            ListView {
                id: slideListView
                objectName: "slideListView"
                // TODO Might have to manually declare data model

                listItemComponents: [
                    ListItemComponent {
                        type: "item"
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            // Image View/Slide Number
                            Container {
                                ImageView {
                                    imageSource: "asset:///images/sun.png"
                                    verticalAlignment: VerticalAlignment.Fill
                                    horizontalAlignment: HorizontalAlignment.Fill
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
                                        hintText: "Slide title/heading"
                                        verticalAlignment: VerticalAlignment.Center
                                    }
                                }
                                // Slide time allocation
                                Container {
                                    layout: StackLayout {
                                        orientation: LayoutOrientation.LeftToRight
                                    }
                                    Label {
                                        text: "Title"
                                        verticalAlignment: VerticalAlignment.Center
                                    }
                                    Slider {
                                        id: slideTimeSlider
                                        horizontalAlignment: HorizontalAlignment.Fill
                                        layoutProperties: StackLayoutProperties {
                                            spaceQuota: 1.0
                                        }
                                        value: 600.0

                                        fromValue: 0.0
                                        toValue: 7200.0

                                        onCreationCompleted: {
                                            var maxValue = totalTimeSlider.value;
                                        }

                                        onImmediateValueChanged: {
                                        }

                                    }
                                }
                            }
                        }
                    } // end of ListItemComponent
                ] // end of listItemComponents
            } // end of ListView
        }        
		
    } // End of content Container
}
