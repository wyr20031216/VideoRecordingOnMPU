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
MPU 视频录制系统项目说明
本项目基于嵌入式 MPU 平台，开发了一个功能完整的视频录制系统，支持图像处理、用户交互、定时控制等多种功能。项目主要面向资源受限设备（如工业相机、边缘计算节点等）的视频采集与处理需求，具有较强的实用性与可拓展性。
本系统通过对图像数据的实时处理与控制逻辑的有机结合，能够实现灵活的录制控制、视觉信息增强、用户状态反馈等操作，适用于需要低功耗、高集成度的视频监控与采集应用场景。
系统结构清晰，软硬件协同设计，便于二次开发和移植，适合用于嵌入式视频应用的开发学习、产品验证或定制部署。

已实现的主要功能
画面翻转
支持视频画面上下、左右方向的翻转，以适配不同安装方向或设备外壳布局。

OSD 自动反色
在图像上叠加时间戳或状态信息时，系统可根据背景亮度自动调整文字颜色，提高可视性与识别率。

遮罩区域变色控制（Mask）
允许通过物理按键控制画面上指定遮罩区域的颜色变化，可用于指示某些状态或遮挡敏感区域。

定时录制
支持按设定时间段自动开始和停止录制，或实现周期性定时分段保存，例如每分钟生成一个视频文件。

标准时间显示
实时显示当前系统标准时间，可用于视频叠加或文件命名，便于后期管理与追溯。

按钮与指示灯驱动
集成硬件按键与LED小灯控制逻辑，可用于启动/停止录像、切换模式、显示系统状态等交互功能。
