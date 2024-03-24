import xml.dom.minidom
import os
import os.path
import re
from xml.etree.ElementTree import parse

#TODO：初始化位置
class XmlData:
    def __init__(self,moto_add="/motoSet.xml"
                    ,acts_add="/moto/"):
        self.abs_path = os.path.abspath(os.path.dirname(__file__))
        self.moto_add = self.abs_path + moto_add
        self.acts_add = self.abs_path + acts_add
        self.nameall = []
        # print("电机xml地址\n",self.moto_add  ,"表情xml地址\\n", self.acts_add )
        print('xml_read init\n')
    # 从xml文件直接读取配置 xml_data 就是全部数据
    def xml_moto(self):
        #(文件名更改)
        dom = xml.dom.minidom.parse(self.moto_add)
        root = dom.documentElement
        #属性名更改
        itemlist = root.getElementsByTagName('motoSys')
        #创建字典 {name1 : {id：, jdStart：...} , name2 :  {id：, jdStart：...} , }
        item_data = {}
        for item in  itemlist :
            item_data[int(item.getAttribute("id"))] = {
                    "jdStart" :int(item.getAttribute("jdStart")),
                    "jdMax"   :int(item.getAttribute("jdMax")),
                    "jdMin"   :int(item.getAttribute("jdMin")),
                    "fScale"  :float(item.getAttribute("fScale")),
                    "fOffSet" :int(item.getAttribute("fOffSet"))
                    }         
        print ("moto_data finish\n")#, item_data
        return item_data
    

    def xml_act(self):
        add = self.acts_add
        files=os.listdir(add)  #得到文件夹下所有文件名称  
        files.sort()
        # print('action files read finished\n')#,files
        xml_actall = {} #所有表情的字典
        Action     = {} #单个表情字典
        Act_num    = {} #动作分解步骤

        for xmlFile in files:
            if os.path.splitext(xmlFile)[1] == ".xml": #判断是否是。XML文件夹
                #提取.xml文件名中文部分 + 读取英文和最后数字
                # nameC_xml = re.sub("[0-9,。]", "", os.path.splitext(xmlFile)[0][:-2]) + os.path.splitext(xmlFile)[0][-2:]
                nameC_xml = re.sub("[A-Za-z0-9,。]", "", os.path.splitext(xmlFile)[0])
                # print(nameC_xml)
                #提取.xml文件信息
                f = open(add + xmlFile)
                et = parse(f)
                root = et.getroot()
                for mP_node in root.iter('motorAction'):
                    id = mP_node.get('id')
                    num = 0
                    for node in mP_node.iter('mPosition'):
                        Act_num [num] = {"tim" :int(node.get('X')),"pos" :int(node.get('Y'))} 
                        num = num + 1
                    if bool(Act_num) == 1: #判断是否有动作
                        Action[int(id)] = Act_num #组成 {id : {时间x,位置y} ,id...}一个表情所有动作 的字典
                        Act_num={}
                        id= 0
                xml_actall[nameC_xml] = Action
                self.nameall.append(nameC_xml)
                
                Action = {}
        # print("self.nameall所有中文名:\n",self.nameall)
        print('xml_actall finish\n')#,xml_actall
        return xml_actall

        

class TimeTable :
    def __init__(self,acts_add="/moto/"):
        self.xmldata = XmlData(acts_add = acts_add)
        self.actdata = self.xmldata.xml_act()
        self.motodata = self.xmldata.xml_moto()
        self.timeline = []

    def Timetable(self, action):
        time_tip = {}  #单个舵机的动作时间表
        first_id = 0   #第一组舵机是否存储完毕标志
        finish = 0     #每个时间点的指令是否存储完毕标志
        
        # print('表情xml参数\n',action,self.actdata[action])
        for Act_num , id in zip (self.actdata[action].values(),self.actdata[action]): #进入小动作的步骤字典 
            time_all = 0
            data_id = {}
            #print(Act_num)
            for timepere in Act_num.values() : 
                time_all = time_all + int(timepere['tim'])
                data_id = {int(id):timepere}
                data1 =  {time_all:data_id}
                finish = 0
                if first_id == 0 :
                    time_tip.update(data1)
                else:
                    for test in time_tip :
                        if finish == 0 :
                            if time_all == test :
                                time_tip[time_all].update(data_id)
                                finish = 1
                    if finish == 0 :
                        time_tip.update(data1)
                        finish = 1
                data_id = {}
            first_id = 1
        self.timeline = sorted(time_tip)#按照键的大小排序
        time_tip = {k: time_tip[k] for k in sorted(time_tip)}
        # print('----------TIMETABLE------------------')
        # print('timeline\n',self.timeline)
        # print('time_tip\n',time_tip)
        # print('----------------------------')
        print('time_tip finish\n')
        return time_tip



if __name__ == '__main__':

    xmlData = XmlData()
    data = xmlData.xml_moto()

    test_action = '挤眉弄眼（调戏）'
    # test_action = '初始'
    timeTable = TimeTable()
    data = timeTable.Timetable(test_action)
    print ("test timeTable", data)

