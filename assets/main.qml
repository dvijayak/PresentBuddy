import bb.cascades 1.0
import bb.system 1.0

NavigationPane {    
    id: navigationPane    
    
    property string activePresentationID // Expose the active presentation to C++ via its ID property

    // Add the application menu using a MenuDefinition
    Menu.definition: MenuDefinition {

        // Specify the actions that should be included in the menu
        actions: [
            ActionItem {
                title: "Settings"
                imageSource: "asset:///icons/ic_settings.png"
            }, 
            ActionItem {
                title: "Invite Friends"
                imageSource: "asset:///icons/ic_bbm.png"
                
                onTriggered: {
                    // Register with the BBM Social Platform
                    _registrationHandler.registerApplication();
                    _registrationHandler.finishRegistration();
                    
                    // N.B. Upon successful registeration, a signal is emitted and a slot sends out an invite to contacts
                }
            }
        ] // end of actions list
    } // end of MenuDefinition

    attachedObjects: [
        // Attach additional pages of the application dynamically
        // Note: would be better to separate the main page as well
        ComponentDefinition {
            id: performPageDefinition
            source: "perform.qml"
        } ,
        ComponentDefinition {
            id: previewPageDefinition
            source: "preview.qml"
        } ,
        ComponentDefinition {
            id: preparePageDefinition
            source: "prepare.qml"
        }
    ]    

    onCreationCompleted: {
        // Make the various page references globally accessible (needed in order to be accessibly by list items)
        Qt.performPageDefinition = performPageDefinition;
        Qt.previewPageDefinition = previewPageDefinition;
        Qt.preparePageDefinition = preparePageDefinition;
        Qt.navigationPane = navigationPane;        
        Qt.DISPLAY_DATE_TIME_FORMAT = applicationUIPropertyMap.DISPLAY_DATE_TIME_FORMAT;

		// TODO Also, these QML components are created BEFORE the presentations list are read from JSON. Interesting!
		Qt.appUI = appUI;		
    }
    

    
    // Main Page (list of presentations)
    Page {
        id: mainPage
        objectName: "mainPage"        
        
        actions: [
            ActionItem {
                title: "New Presentation"
                imageSource: "asset:///icons/ic_add.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                
                onTriggered: {
                    // Indicate that a new presentation is to be created
                    Qt.navigationPane.activePresentationID = -1;
                    
                    var page = Qt.preparePageDefinition.createObject();
                    Qt.navigationPane.push(page);
                }
            }
        ]
        
        content: Container {
            attachedObjects: [
                // Add the data model as an attached object. Make sure
                // to specify a value for the objectName property,
                // which is used to access the model from C++.
                GroupDataModel {
                    id: mainDataModel
                    objectName: "mainDataModel"
                    grouping: ItemGrouping.None
                    sortedAscending: false // Latest modified presentations to appear first
                    
                    sortingKeys: [ "dateModified", "name" ]
                }
            ]			

            ListView { 
                id: listView
                objectName: "listView"

                onTriggered: {
                    // Set the active presentation to be the one that was selected
                    Qt.navigationPane.activePresentationID = mainDataModel.data(indexPath).id;

                    // Push the prepare page on to the stack (go to the prepare page)
                    var page = Qt.previewPageDefinition.createObject();
                    Qt.navigationPane.push(page);
                }

                // Associate the GroupDataModel from the attached objects
                // list with this ListView
                dataModel: mainDataModel

                listItemComponents: [
                    // Use a ListItemComponent to define the appearance of
                    // list items (that is, those with a type of "listItem")
                    ListItemComponent {                        
                        type: "item"


						
                        // Each list item is represented by a StandardListItem
                        // whose text fields are populated with data from the
                        // item
                        Container {
                            id: listItemRoot
                            layout: StackLayout {}
                            
                            Container {
                                Divider {}
                                StandardListItem {                                    
                                    title: ListItemData.name
                                    status: Qt.appUI.minSecToText(ListItemData.totalTime.minutes, ListItemData.totalTime.seconds)
                                    description: "Last Modified: " + Qt.formatDateTime(ListItemData.dateModified, Qt.DISPLAY_DATE_TIME_FORMAT)                                                                        
                                    
                                    contextActions: [
                                        ActionSet {
                                            actions: [
                                                ActionItem {
                                                    id: performButton
                                                    title: "Perform"
                                                    imageSource: "asset:///icons/9-av-play81.png"
                                                    
                                                    onTriggered: {
                                                        // Set the active presentation to be the one that was selected
                                                        Qt.navigationPane.activePresentationID = listItemRoot.ListItem.data.id;

                                                        var page = Qt.performPageDefinition.createObject();
                                                        Qt.navigationPane.push(page);
                                                    }
                                                } ,
                                                ActionItem {
                                                    id: previewButton
                                                    title: "Preview"
                                                    imageSource: "asset:///icons/ic_view_details.png"
                                                    
                                                    onTriggered: {
                                                        // Set the active presentation to be the one that was selected
                                                        Qt.navigationPane.activePresentationID = listItemRoot.ListItem.data.id;

                                                        // Push the prepare page on to the stack (go to the prepare page)
                                                        var page = Qt.previewPageDefinition.createObject();
                                                        Qt.navigationPane.push(page);
                                                    }
                                                } ,
                                                ActionItem {
                                                    id: editButton
                                                    title: "Prepare"                                                
                                                    imageSource: "asset:///icons/ic_edit.png"
                                                    
                                                    onTriggered: {
                                                        // Set the active presentation to be the one that was selected
                                                        Qt.navigationPane.activePresentationID = listItemRoot.ListItem.data.id;

                                                        // Push the prepare page on to the stack (go to the prepare page)
                                                        var page = Qt.preparePageDefinition.createObject();
                                                        Qt.navigationPane.push(page);
                                                    }
                                                } ,
                                                ActionItem {
                                                    id: deleteButton
                                                    title: "Perish"
                                                    imageSource: "asset:///icons/ic_delete.png"

                                                    attachedObjects: [
                                                        SystemDialog {
                                                            id: deletePresentationDialog
                                                            title: "Confirm delete presentation"
                                                            body: "Are you sure you want to delete " + listItemRoot.ListItem.data.name + "?"
                                                            onFinished: {
                                                                if (deletePresentationDialog.result == SystemUiResult.ConfirmButtonSelection) {
                                                                    // Set the active presentation to be the one that was selected
                                                                    Qt.navigationPane.activePresentationID = listItemRoot.ListItem.data.id;

                                                                    Qt.appUI.deletePresentation();
                                                                }
                                                            }
                                                        }
                                                    ]

                                                    onTriggered: {
                                                        deletePresentationDialog.show();
                                                    }
                                                }
                                            ]
                                        }
                                    ]
                                }                                
                            }                                                                                    
                        }
                    }
                ]
            } // end of ListView
        } // end of Container            
    }    // end of Main Page
    
    // Destroy page after leaving it in order to avoid memory leaks
    onPopTransitionEnded: { 
        page.destroy(); 
    }    
}