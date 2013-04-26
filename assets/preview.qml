import bb.cascades 1.0
import bb.system 1.0

Page {
    id: previewPage    
    objectName: "previewPage"
    
    actions: [
        ActionItem {
            title: "Perform"
            objectName: "playButton"
            
            imageSource: "asset:///icons/9-av-play81.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            
            onTriggered: {
                var page = Qt.performPageDefinition.createObject();
                Qt.navigationPane.push(page);
            }
        } ,
        ActionItem {
            title: "Prepare"
            objectName: "editButton"
            
            imageSource: "asset:///icons/ic_edit.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            
            onTriggered: {
                // Push the prepare page on to the stack (go to the prepare page)
                var page = Qt.preparePageDefinition.createObject();
                Qt.navigationPane.push(page);
            }
        } ,
        ActionItem {
            title: "Perish"
            objectName: "deleteButton"
            
            imageSource: "asset:///icons/ic_delete.png"
            ActionBar.placement: ActionBarPlacement.OnBar

            attachedObjects: [
                SystemDialog {
                    id: deletePresentationDialog
                    title: "Confirm delete presentation"
                    body: "Are you sure you want to delete this presentation?" //Qt.appUI.activePresentation.name
                    onFinished: {
                        if (deletePresentationDialog.result == SystemUiResult.ConfirmButtonSelection) {
                            Qt.appUI.deletePresentation();
                            
                            // Return to the main page
                            Qt.navigationPane.pop();
                        }
                    }
                }
            ]

            onTriggered: {
                deletePresentationDialog.show();
            }
        } ,
        InvokeActionItem {
            id: nfcShareButton
            title: "Share via NFC"
            imageSource: "asset:///icons/ic_pixle_subway_nfc_stroke.png"                        

            onTriggered: {                
            }

            query {                
                mimeType: "application/vnd.rim.nfc.ndef"
                invokeActionId: "bb.action.SHARE"
            }
        } ,
        ActionItem {
            id: createCalendarEventButton
            title: "Mark on Calendar"
            imageSource: "asset:///icons/ic_pixle_subway_add_calendar_event.png"
            
            onTriggered: {                                
                Qt.appUI.createCalendarEvent();
            }            
        }
    ]

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
                    
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 0.5
                    }
                }
                Label {
                    id: nameValueLabel
                    objectName: "nameValueLabel"
                                        
                    textStyle.color: Color.Cyan
                    textStyle.textAlign: TextAlign.Left
                    multiline: true

                    verticalAlignment: VerticalAlignment.Center

                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 3.0
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
        } // end of MetaData

        Divider {}

		// Slide list visuals
		Container {
            ListView {
                id: slideListView
                objectName: "slideListView"                

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
                                        text: listItemRoot.ListItem.indexInSection + 1

                                        textStyle {
                                            base: SystemDefaults.TextStyles.BigText
                                            textAlign: TextAlign.Center
                                            fontWeight: FontWeight.Bold
                                            fontSize: FontSize.PointValue
                                            fontSizeValue: 35
                                        }

                                        verticalAlignment: VerticalAlignment.Fill
                                        horizontalAlignment: HorizontalAlignment.Fill
                                        rightMargin: 40.0
                                    }
                                }
                                // Slide Data
                                Container {
                                    layout: StackLayout {
                                        orientation: LayoutOrientation.TopToBottom
                                    }
                                    
                                    verticalAlignment: VerticalAlignment.Center
                                    
                                    // Slide title
                                    Container {
                                        layout: StackLayout {
                                            orientation: LayoutOrientation.LeftToRight
                                        }
                                        Label {
                                            text: "Title"
                                            
                                            verticalAlignment: VerticalAlignment.Center
                                        }
                                        Label {
                                            objectName: "slideTitleText"
                                            
                                            text: ListItemData.title
                                            textStyle.color: Color.Cyan
                                            textStyle.textAlign: TextAlign.Left
                                            multiline: true

                                            verticalAlignment: VerticalAlignment.Center
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

                                            text: Qt.appUI.minSecToText(ListItemData.time.minutes, ListItemData.time.seconds)
                                            textStyle.color: Color.Cyan
                                            textStyle.textAlign: TextAlign.Left

                                            verticalAlignment: VerticalAlignment.Center
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
