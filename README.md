This is a 2D-3D Pose Estimation application, implemented using modern OpenGL (Version 3.3) and the OpenCV library, for an Advanced Computer Graphics course.
A 3D world is created out of out a 2D heatmap. The user can move in the right hand viewport, choose corresponding locations in the left (Global) and right viewports, for the applications to then solve and display the estimated Pose.
Automatic Picking based on predefined markers with distinct colors is also supported.

Instructions for automatic picking:
1) You can move about the scene pressing 'W' or 'S' respectively to go in or against the direction of the scene's camera principal axis. To pitch and yaw the camera, use the right/left and up/down buttons to respectively pitch or yaw the camera.
2) Then by pressing 'B' each time, the application will record and calculate the estimated Pose based on the 2D markers it identified. See stdout logs to see the approximation in details (i.e the calculted pose - camera position in 3D and principal direction vector).
3) By pressing Shift+R , you can enter toggle mode to toggle between the different scenes. An illustration of the camera view will be rendered on the Global View as a triangle facing in the camera's direction. To change scenes, press the left and right buttons.
4) To enter Compute Mode, press 'C'. This mode shows the approximated against the original scene snapshot, based on the Pose Estimation calculations. 
