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
        Qt.appUI = appUI;
        
        Qt.getMatchedPresentationIndex = navigationPane.getMatchedPresentationIndex;

		// TODO Also, these QML components are created BEFORE the presentations list are read from JSON. Interesting!        
    }
    

    
    // Main Page (list of presentations)
    Page {
        id: mainPage
        
        // Initialize this page, including its data model
        onCreationCompleted: {
            // TODO Apparently a page is created BEFORE the navigationPane completes creation
        }
        
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
                    id: groupDataModel
                    objectName: "groupDataModel"
                    grouping: ItemGrouping.None
                    sortedAscending: false // Latest objects to appear first

                    // Sort the data first by last name, then by first
                    // name
                    sortingKeys: [ "dateModified" ]
                }
            ]			

            ListView { 
                id: listView
                objectName: "listView"

                // Associate the GroupDataModel from the attached objects
                // list with this ListView
                dataModel: groupDataModel

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
//                                    text: "Perform"                                    
                                    //                                imageSource: "asset:///icons/ic_delete.png"
                                    
                                    onCreationCompleted: {
                                        var presentations = Qt.appUI.presentations;
//                                        var index = 0;
//                                        for (var i = 0; i < presentations.length; i++) {
//                                            if (presentations[i].name == listItemRoot.ListItem.data.name) {
//                                                index = i;
//                                                break;
//                                            }
//                                        }
//                                        performButton.text = presentations[index].name;
                                        var name = listItemRoot.ListItem.data.name;
                                        performButton.text = presentations[Qt.getMatchedPresentationIndex(name)].name;                                        
                                    }
                                    
                                    onClicked: {
                                        var page = Qt.pageDefinition.createObject();
                                        Qt.navigationPane.push(page);
                                    }                                    
                                }
                                Button {
                                    id: practiseButton
                                    text: "Practise"
                                    //                                imageSource: "asset:///icons/ic_delete.png"
                                    onClicked: {
                                        var presentations = Qt.appUI.presentations;
                                        for (var i = 0; i < presentations.length; i++) {
                                            var output = "Name: " + presentations[i].name + ", Total Time: " + presentations[i].totalTime.minutes + " mins "  
                                            + presentations[i].totalTime.seconds + " secs " + ", Last Modified: " + presentations[i].dateModified;
                                            var slides = presentations[i].slides;
                                            output += " Slides: [";
                                            for (var j = 0; j < slides.length; j++) {
                                                output += "Title: " + slides[j].title + ", Time: " + slides[j].time.minutes + " mins " + slides[j].time.seconds + " secs ";
                                                output += " | ";
                                            }
                                            output += "]";
                                            console.log(output);
                                        }
                                    }
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
                                        var name = listItemRoot.ListItem.data.name;
                                        Qt.chosenPresentationIndex = Qt.getMatchedPresentationIndex(name);
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

//                // Override the itemType() function to return the proper type
//                // for each item in the list. Because a GroupDataModel has only
//                // two levels, use the index path to determine whether the item
//                // is a header item or a list item.
//                function itemType(data, indexPath) {
//                    if (indexPath.length == 1) {
//                        // If the index path contains a single integer, the item
//                        // is a "header" type item
//                        return "header";
//                    } else {
//                        // If the index path contains more than one integer, the
//                        // item is a "listItem" type item
//                        return "listItem";
//                    }
//                }
            } // end of ListView
        } // end of Container            
    }    // end of Main Page
    onPopTransitionEnded: { 
        page.destroy(); 
    }
    
    function getMatchedPresentationIndex(name) {
        var presentations = Qt.appUI.presentations;
        for (var i = 0; i < presentations.length; i++) {
            if (presentations[i].name == name) {
                console.log("HAHAHAHA " + i);
                return i;
            }
        }
        console.log("HAHAHAHA " + i);
        return -1;
    }
}