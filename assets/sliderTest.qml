// Default empty project template
import bb.cascades 1.0

// creates one page with a label
Page {
    Container {
        id: containerID
        layout: StackLayout {}
//        topPadding: 20.0
        background: Color.create("#262626")
        Container {
            layout: DockLayout {}
            horizontalAlignment: HorizontalAlignment.Center
            ImageView {
                id: night
                opacity: 1.0
                imageSource: "asset:///images/night.jpg"
                horizontalAlignment: HorizontalAlignment.Center
            }
            ImageView {
                id: day
                opacity: 0.0
                imageSource: "asset:///images/day.jpg"
                horizontalAlignment: HorizontalAlignment.Center
            }

        }
        Container {
            leftPadding: 20.0
            rightPadding: 20.0
            topPadding: 25.0
            bottomPadding: 25.0
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            ImageView {
                id: leftSliderIcon
                imageSource: "asset:///images/moon.png"
                verticalAlignment: VerticalAlignment.Center

            }
            Slider {
                leftMargin: 20.0
                rightMargin: 20.0
                horizontalAlignment: HorizontalAlignment.Fill
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1.0

                }
                onImmediateValueChanged: {
                    day.opacity = immediateValue;
                    night.opacity = 1.0 - immediateValue;
                    leftSliderIcon.opacity = 1.0 - immediateValue;
                    rightSliderIcon.opacity = immediateValue;
                }

            }
            ImageView {
                id: rightSliderIcon
                imageSource: "asset:///images/sun.png"
                verticalAlignment: VerticalAlignment.Center
                opacity: 0.0
            }
            Button {
                id: button1                
                opacity: 0.8
                imageSource: "asset:///images/moon.png"
            }

        }
    }

}

