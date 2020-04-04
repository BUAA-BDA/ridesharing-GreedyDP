GreedyDP: A Unified Approach to Route Planning for Shared Mobility
========================================================================

This repository stores the source code of the proposed algorithm called GreedyDP and pruneGreedyDP in the following paper.
[1] **A Unified Approach to Route Planning for Shared Mobility.**
*Yongxin Tong, Yuxiang Zeng, Zimu Zhou, Lei Chen, Jieping Ye, Ke Xu.* PVLDB 11(11): 1633-1646 (2018). [link](http://www.vldb.org/pvldb/vol11/p1633-tong.pdf) [slides](http://yongxintong.group/static/paper/2018/VLDB2018_A%20Unified%20Approach%20to%20Route%20Planning%20for%20Shared%20Mobility_Slides.pptx)

If you find this work helpful in your research, please consider citing our paper and the bibtex are listed below:
```  
@article{DBLP:journals/pvldb/TongZZCYX18,  
  author    = {Yongxin Tong and   
               Yuxiang Zeng and   
               Zimu Zhou and   
               Lei Chen and    
               Jieping Ye and   
               Ke Xu},   
  title     = {A Unified Approach to Route Planning for Shared Mobility},   
  journal   = {{PVLDB}},   
  volume    = {11},   
  number    = {11},  
  pages     = {1633--1646},   
  year      = {2018},   
}
```  



Usage of the algorithms
---------------

### Environment

gcc/g++ version: 7.4.0 

OS: Ubuntu

### Compile the algorithms

cd algorithm && make pruneGDP GDP

pruneGDP: the pruneGreedyDP algorithm in the paper

GDP: the GreedyDP algorithm in the paper

### Run the algorithms

./pruneGDP ./road/chengdu.node ./road/chengdu.edge ./road/chengdu.label ./road/chengdu.order ./taxi.txt ./order.txt ./res.txt

./GDP ./road/chengdu.node ./road/chengdu.edge ./road/chengdu.label ./road/chengdu.order ./taxi.txt ./order.txt ./res.txt

chengdu.node:  the vertex coordinates of the road network of Chengdu

chengdu.edge:  the edges of the road network of Chengdu, the unit of the edge weight is second.

chengdu.label: the labels for on-the-fly shortest distance/path query

chengdu.order: the orders for on-the-fly shortest distance/path query

taxi.txt:      the major parameters

order.txt:     the input of the taxi-calling orders

res.txt:       the output of the program

##### Road network
Due to space limit, please download the files of road network (i.e., chengdu.*) by this [url](https://drive.google.com/open?id=1w6IGkA-LcW7KUe4B93v9gx4mnwNt4em5).
We recommend to use the hub labeling based shortest path algorithms [url](https://github.com/BUAA-BDA/sspexp_clone) to process the road network. 

##### Real-world orders
For the taxi-calling orders in Chengdu city, please request for the dataset in the [website of GAIA](https://outreach.didichuxing.com/research/opendata/).
Here we only provide a sample file (i.e., order.txt).


Description of the data generator
---------------

### Environment

Python: 2.7

### Run the scripts

genChengTaxi.py: a script to generate the varied parameters in the experiments



Related other resources
------------------------
We have maintained a paper list of the studies on ridesharing. [link](https://github.com/BUAA-BDA/ridesharing-paperlist)



Contact
------------
- Yuxiang Zeng: yzengal@cse.ust.hk
- Yongxin Tong: yxtong@buaa.edu.cn

