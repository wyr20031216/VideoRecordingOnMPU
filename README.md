# VideoRecordingOnMPU
This project is developed based on an embedded MPU platform and implements a fully functional video recording system that supports image processing, user interaction, timed control, and more. It is designed for video acquisition and processing on resource-constrained devices such as industrial cameras and edge computing nodes, offering strong practicality and scalability.

By integrating real-time image processing with control logic, the system enables flexible recording control, visual information enhancement, and user status feedback. It is well-suited for video monitoring and data acquisition scenarios requiring low power consumption and high integration.

The system features a clear structure and a hardware-software co-design approach, making it easy to port and extend. It is ideal for embedded video application development, prototyping, and customized deployment.

### Implemented Features

**Image Flipping**
Supports vertical and horizontal flipping of video frames to adapt to different installation orientations or device layouts.

**Automatic OSD Color Inversion**
Automatically adjusts the overlay text (such as timestamps or status info) color based on background brightness to improve visibility and readability.

**Mask Area Color Switching**
Allows users to change the color of designated mask regions via physical buttons, which can indicate status changes or hide sensitive areas.

**Timed Recording**
Supports automatic start and stop of recording based on preset durations, or periodic segmented recording (e.g., one video file per minute).

**Standard Time Display**
Displays the current system time in real-time for overlay purposes or file naming, aiding in later management and traceability.

**Button and LED Control**
Integrates drivers for physical buttons and indicator LEDs, enabling functions such as recording control, mode switching, and system status indication.
