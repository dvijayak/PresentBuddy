import bb.cascades 1.0

NavigationPane {    
    id: navigationPane

    attachedObjects: [
        // Attach additional pages of the application dynamically
        // Note: would be better to separate the main page as well
        ComponentDefinition {
            id: pageDefinition
            source: "perform.qml"
        },
        ComponentDefinition {
            id: preparePageDefinition
            source: "prepare.qml"
        }
    ]    

    onCreationCompleted: {
        Qt.pageDefinition = pageDefinition;
        Qt.preparePageDefinition = preparePageDefinition;
        Qt.navigationPane = navigationPane;        
        Qt.DISPLAY_DATE_TIME_FORMAT = applicationUIPropertyMap.DISPLAY_DATE_TIME_FORMAT;        

		// TODO Also, these QML components are created BEFORE the presentations list are read from JSON. Interesting!        
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
                    
                    sortingKeys: [ "dateModified" ]
                }
            ]			

            ListView { 
                id: listView
                objectName: "listView"

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
                                StandardListItem {                                    
                                    title: ListItemData.name
                                    status: ListItemData.totalTime.minutes + ":" + ListItemData.totalTime.seconds
                                    description: "Last Modified: " + Qt.formatDateTime(ListItemData.dateModified, Qt.DISPLAY_DATE_TIME_FORMAT)
                                }                                
                            }
                            Container {
                                layout: StackLayout {
                                    orientation: LayoutOrientation.LeftToRight
                                }                                
                                Button {
                                    id: performButton                                    
                                    text: "Perform"
                                    
                                    onClicked: {
                                        var page = Qt.pageDefinition.createObject();
                                        Qt.navigationPane.push(page);
                                    }                                    
                                }
                                Button {
                                    id: practiseButton
                                    text: "Practise"                                                                    
                                }
                                Button {
                                    id: previewButton
                                    imageSource: "asset:///icons/ic_view_details.png"
                                }
                                Button {
                                    id: editButton
                                    objectName: "editButton"                                    
                                    imageSource: "asset:///icons/ic_edit.png"
                                    
                                    onClicked: {                                        
                                        var page = Qt.preparePageDefinition.createObject();                                        
                                        Qt.navigationPane.push(page);                                        
                                    }
                                }
                                Button {
                                    id: deleteButton
                                    imageSource: "asset:///icons/ic_delete.png"
                                }
                            }
                            
                            Divider {}
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