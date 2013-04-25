import bb.cascades 1.0
import bb.system 1.0

Page {
    id: preparePage    
    objectName: "preparePage"
    
    onCreationCompleted: {
        Qt.totalTimeSlider = totalTimeSlider;
    }
    
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
                savedChangesToast.show();
                Qt.navigationPane.pop();                                
            }
            attachedObjects: [
                SystemToast {
                    id: savedChangesToast
                    body: "Successfully saved changes."                    
                }
            ]
        } ,
        ActionItem {
            title: "Reset"
            objectName: "resetButton"
            
            imageSource: "asset:///icons/ic_pixle_subway_undo.png"
            onTriggered: {
                resetToast.show();
            }
            attachedObjects: [
                SystemToast {
                    id: resetToast
                    body: "Changes have been reset."
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

                    textStyle.color: Color.Cyan
                    textStyle.textAlign: TextAlign.Left                    
                }
            }    
            
            // Presentation Total Time Slider
            Container{
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                Slider {
                    id: totalTimeSlider
                    objectName: "totalTimeSlider"
                    property double maxTimeRemaining; // The maximum amount of time remaining that can be allocated to a slide
                    fromValue: 1.0
                    toValue: 5999.0
                    horizontalAlignment: HorizontalAlignment.Fill
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 3.0
                    }

                    onImmediateValueChanged: {
                        totalTimeValueLabel.text = Qt.appUI.timeToText(Math.floor(immediateValue));
                    }

                    onCreationCompleted: {
                        totalTimeSlider.valueChanged.connect(Qt.appUI.bufferTotalTimeChange);
                    }

                }
                // Lock button
                ImageToggleButton {
                    id: totalTimeLock
                    imageSourcePressedUnchecked: "asset:///icons/ic_pixle_subway_unlock2.png"
                    imageSourcePressedChecked: "asset:///icons/ic_pixle_subway_lock2.png"
                    imageSourceDefault: "asset:///icons/ic_pixle_subway_unlock2.png"
                    imageSourceChecked: "asset:///icons/ic_pixle_subway_lock2.png"
                    
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                    
                    onCheckedChanged: {
                        if (totalTimeLock.checked) {
                            totalTimeSlider.enabled = false;
                        }
                        else {
                            totalTimeSlider.enabled = true;
                        }
                                                
                    }

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
                                                timeValueLabel.text = Qt.appUI.timeToText(Math.floor(slideTimeSlider.immediateValue));
                                            }
                                        }
                                    }
                                    // Slide time allocation slider
                                    Container {
                                        layout: StackLayout {
                                            orientation: LayoutOrientation.LeftToRight
                                        }
                                        Slider {
                                            id: slideTimeSlider
                                            objectName: "slideTimeSlider"
                                            
                                            horizontalAlignment: HorizontalAlignment.Fill
                                            layoutProperties: StackLayoutProperties {
                                                spaceQuota: 3.0
                                            }
                                            value: Qt.appUI.timeFromMinSecs(ListItemData.time.minutes, ListItemData.time.seconds)
                                            fromValue: 1.0
                                            toValue: Qt.totalTimeSlider.immediateValue
                                            onImmediateValueChanged: {
                                                var validValue;
                                                if (immediateValue > Qt.totalTimeSlider.immediateValue) {
                                                    validValue = Qt.totalTimeSlider.immediateValue;
                                                }
                                                else {
                                                    validValue = immediateValue;
                                                }
                                                timeValueLabel.text = Qt.appUI.timeToText(Math.floor(validValue));
                                            }

                                            onValueChanged: {
                                                Qt.appUI.bufferSlideTimeChange(listItemRoot.ListItem.indexInSection, value);
                                            }
                                        }
                                        // Lock button
                                        ImageToggleButton {
                                            id: slideTimeLock
                                            imageSourcePressedUnchecked: "asset:///icons/ic_pixle_subway_unlock2.png"
                                            imageSourcePressedChecked: "asset:///icons/ic_pixle_subway_lock2.png"
                                            imageSourceDefault: "asset:///icons/ic_pixle_subway_unlock2.png"
                                            imageSourceChecked: "asset:///icons/ic_pixle_subway_lock2.png"
                                            
                                            verticalAlignment: VerticalAlignment.Center
                                            horizontalAlignment: HorizontalAlignment.Center

                                            onCheckedChanged: {
                                                if (slideTimeLock.checked) {
                                                    slideTimeSlider.enabled = false;
                                                } else {
                                                    slideTimeSlider.enabled = true;
                                                }
                                            }
                                        }
                                    }
                                    Container {
	                                    Button {
	                                        id: deleteSlideButton
	                                        imageSource: "asset:///icons/ic_delete.png"
	
	                                        attachedObjects: [
	                                            SystemDialog {
	                                                id: deleteSlideDialog
	                                                title: "Confirm delete slide"
	                                                body: "Are you sure you want to delete " + listItemRoot.ListItem.data.title + "?"
	                                                onFinished: {
	                                                    if (deleteSlideDialog.result == SystemUiResult.ConfirmButtonSelection) {	                                                        
	                                                        var index = listItemRoot.ListItem.indexInSection;
                                                            Qt.appUI.deleteSlide(index);
	                                                    }
	                                                }
	                                            }
	                                        ]
	
	                                        onClicked: {
	                                            deleteSlideDialog.show();
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
