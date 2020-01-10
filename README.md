# DComputing_Course
并行与分布式计算作业用


##关于最后的大作业
  最后的大作业有两个要求一个是服务端要能够实现同时处理多个客户端的连接，二是客户端根据一个类似BT种子的文件，多线程从多台主机下载同一个文件的不同部分然后进行拼接，期间使用互斥量进行写入控制。
  实现服务器端需要使用winsock的select I/O模型，基于这个编写，每接收一个请求就新起一个线程处理。
  为了实现客户端，我设计了一个如下图所示的BT种子结构（为了方便我直接写成了一个txt文档）
  ![BT文件内容](https://github.com/Royce-DaDaDa/DComputing_Course/blob/master/pics/6.png)
  文件开头是种子对应的文件名（占用20个字节），然后是文件大小（占用5个字节），之后就是每一块的信息，包括IP地址(15个字节)，端口（5个字节），该分片的文件名（20个字节），该分片大小（5个字节）。客户端会从BT文件中读取对应信息，起多个线程去获取文件分片。
  
  我把测试文件的文件全都放在了本机，这样客户端起多个线程去访问同一个服务端的不同文件分片，刚好可以同时测试服务端和客户端的功能。
  ![文件分片1](https://github.com/Royce-DaDaDa/DComputing_Course/blob/master/pics/3.png)
  ![文件分片2](https://github.com/Royce-DaDaDa/DComputing_Course/blob/master/pics/4.png)
  ![文件分片3](https://github.com/Royce-DaDaDa/DComputing_Course/blob/master/pics/7.png)
  ![客户端获取的结果](https://github.com/Royce-DaDaDa/DComputing_Course/blob/master/pics/4.png)
  ![客户端运行截图](https://github.com/Royce-DaDaDa/DComputing_Course/blob/master/pics/1.png)
  ![服务端运行截图](https://github.com/Royce-DaDaDa/DComputing_Course/blob/master/pics/2.png)
  
  
 由于整个项目文件太大Git上来很麻烦，我在FileClient和FileServer文件夹中放了项目的源代码，要运行的话需要建两个project把源代码放进去编译运行，还需要把hello_bt.txt放到FileClient的工作目录下，把hello_1.txt,hello_2.txt,hello_3.txt放到FileServer的工作目录下。
