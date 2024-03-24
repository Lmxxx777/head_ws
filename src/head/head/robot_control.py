#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import time
import rclpy
from rclpy.node import Node
from chat_msgs.msg import Servo
from servo_control import ServoCtrl

class RobotCtrlNode(Node, ServoCtrl):
    def __init__(self, name, arg, *args, **kwargs):
        super().__init__(name)
        super(ServoCtrl, self).__init__(arg, *args, **kwargs)
        self.sub = self.create_subscription(Servo, "robot_state", self.listener_callback, 10)
        self.get_logger().info('robot_control node init')

    def listener_callback(self, msg):
        # self.get_logger().info('call back')
 
        # TODO: 换一种方式打包 msg
        msgs = [msg.left_blink, msg.left_smile, msg.left_eye_erect, msg.left_eye_level, msg.left_eyebrow,
                msg.right_blink, msg.right_smile, msg.right_eye_erect, msg.right_eye_level, msg.right_eyebrow,
                msg.head_dian, msg.head_yao, msg.head_bai,
                msg.mouth]
        self.send(msgs)
        time.sleep(0.001)

def main(args=None):
    rclpy.init(args=args)
    node = RobotCtrlNode("facial_features", '/dev/ttyUSB0', 115200, 8, 'N', 1)
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


