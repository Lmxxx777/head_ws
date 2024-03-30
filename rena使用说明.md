rena使用说明：

把功能包分别放在rena和虚拟机上，修改绝对路径，使用build.sh编译，虽然虚拟机上的voiceinteraction功能包因为没有hid_lib无法编译成功，但是不在他上面运行。

在运行之前要确认两个ubuntu都在同一个网段下，即相互可以ping通，热点就可以，而且虚拟机的NET要设置成桥接模式下的复制物理网络连接状态。

在虚拟机上运行run.sh里面的listen_v2、spark_v2、face_detection功能包。

在rena上运行剩余功能包。

如果rena上的listen_v2可以使用讯飞控制板的USB麦克风，就可以把所有的功能包放在rena上运行。

如果出现麦克风被占用，需要关闭电脑，关闭绿色的电源键，再开机。

如果不接入语音启动使用head_console，记得打开那个timer_callback，若语音控制，则关闭它。
