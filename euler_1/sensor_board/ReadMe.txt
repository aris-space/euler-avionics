This is the repository for the Sensor Board Software


############################################## SENSOR BOARD SOFTWARE REV 2 ##########################################################

For this implementation we use cmsis2-freertos, which is a sub implementation of freertos that is already given in the stm32cubeIDE package.

So the project should work as is. What is important tough:

if one opens the Sensor_Board_rev2.ioc file it will open in the cubeMX perspective. If one changes stuff in there, the project will generate code again. How that is handled,
is that all the tasks are written in the main.c file. I dont like that. This is why I wrote for every task an own file. The issue is that every time we change something in Sensor_Board_rev2.ioc
the IDE will regenerate all the stuff in the main.c file. This is why we have to delete the implementation of the task in there.


As of now, the code should be deployable to the chip. It should work if the right chip select for the imu is set. The code only misses the BMI160 driver. Once this is implemented
the software should be finished.

The code has to be tested and debugged yet. I am sure that there are still plenty of bugs hidden everywhere!


For the Software concept one can look at http://wiki.aris-space.ch/pages/viewpage.action?pageId=2625943

############################################## SENSOR BOARD SOFTWARE REV 3 ##########################################################

For this implementation we use cmsis2-freertos, which is a sub implementation of freertos that is already given in the stm32cubeIDE package.

So the project should work as is. What is important tough:

if one opens the Sensor_Board_rev2.ioc file it will open in the cubeMX perspective. If one changes stuff in there, the project will generate code again. How that is handled,
is that all the tasks are written in the main.c file. I dont like that. This is why I wrote for every task an own file. The issue is that every time we change something in Sensor_Board_rev2.ioc
the IDE will regenerate all the stuff in the main.c file. This is why we have to delete the implementation of the task in there.


As of now, the code should be deployable to the chip. Just need to change the conifguration of the IMU once we know which IMU is being used.

The code has to be tested and debugged yet. I am sure that there are still plenty of bugs hidden everywhere!


For the Software concept one can look at http://wiki.aris-space.ch/pages/viewpage.action?pageId=2625943