# facial_expression 节点

[facial_expression](../ulaahead/robot_action.py) 是应用功能节点，通过面部特征接口，实现表情控制

## Code Information
- 节点名称：*facial_expression*
```python
node = ActionSubscriber("facial_expression")
```

### 订阅者 *sub*
- 订阅话题 *action* 

```python
self.sub = self.create_subscription(String, 'action', self.action_callback, 10) # 订阅std
```

-  *action_callback()* 
    - 判断表情类型

-  *Std_action()* 
    - 从 *xml_read.py* 读取对应表情信息，放入 *facial_features* 接口

### 订阅者 *sub_addr*
- 订阅话题 *action* (暂定)

```python
self.sub_addr = self.create_subscription(String, 'action', self.addr_callback, 10) 
```

-  *addr_callback()* 
    - 随机选择微表情，放入 *facial_features* 接口

---
### [xml_read.py](../ulaahead/xml_read.py) 提取.xml表情文件中舵机的信息
- *xml_moto()* 
    - 读取[motoSet.xml](../ulaahead/motoSet.xml) 各舵机的基本信息
-  *xml_act()* 
    - 读取[moto](../ulaahead/moto)文件夹中表情的舵机信息
    - 舵机信息　＝　舵机ＩＤ　：｛运动时间＇tim＇，舵机位置＇pos＇｝
    - <u>**添加其他表情**</u>，可以将表情文件 **.xml** 放入[moto](../ulaahead/moto)文件夹中
-  *Timetable()* 
    - 根据 *xml_act()* 信息生成舵机运动时间表
    - 例子：
        1. 假设 *xml_act* 提取得到：41:{tim:5,pos:100}，41:{tim:10,pos:120}，42:{tim:5,pos:95}，42:{tim:16,pos:120}；
        2. *Timetable* 生成 time_tip 字典：{5:{41:{tim:5,pos:100},42:{tim:5,pos:95}},15:{41:{tim:10,pos:120}},30:{42:{tim:15,pos:120}}}

   
