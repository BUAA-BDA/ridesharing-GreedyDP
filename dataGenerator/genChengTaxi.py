#!/usr/bin/env python

import numpy as np
from random import randint, sample
import sys
import os
import commands

class constForDataSet:
	gridList = [1000, 2000, 3000, 4000, 5000]
	workerNumList = [2000, 5000, 10000, 20000, 30000]
	capList = [3, 4, 6, 10, 20]
	alpha = 1
	ddlList = [x*60 for x in [5, 10, 15, 20, 25]]
	prList = [2, 5, 10, 20, 30]
	defaultValues = [
		10000,
		capList[1],
		alpha, 
		ddlList[1], 
		prList[2], 
		gridList[0],
	]

class CFDS(constForDataSet):
	pass

class baseGenerator:

	def gen(self, n):
		pass

class worker_t:

	def __init__(self, sloc=[0,0], dloc=[0,0], releaseTime=-1, expireTime=-1):
		self.sloc = sloc
		self.dloc = dloc
		self.releaseTime = releaseTime
		self.expireTime = expireTime

	def __cmp__(self, other):
		return self.releaseTime < other.releaseTime

	def __lt__(self, other):
		return self.releaseTime < other.releaseTime

	def __str__(self):
		ret = "1 %s %s %s %s %s %s" % (self.sloc[0], self.sloc[1], self.dloc[0], self.dloc[1], self.releaseTime, self.expireTime)
		return ret

class task_t:

	def __init__(self, loc=[0,0], releaseTime=0, expireTime=0, ut=1):
		self.loc = loc
		self.rt = releaseTime
		self.et = expireTime
		self.ut = ut

	def __cmp__(self, other):
		return self.rt < other.rt

	def __lt__(self, other):
		return self.rt < other.rt

	def __str__(self):
		ret = "0 %s %s %s %s %s" % (self.loc[0], self.loc[1], self.rt, self.et, self.ut)
		return ret

class randomGenerator(baseGenerator):

	def __init__(self, mx):
		self.mx = mx

	def setMx(self, mx):
		self.mx = mx

	def gen(self, n):
		return [randint(0, self.mx-1) for i in xrange(n)]

		
def genDataSetName(ch, m, c, alpha, ddl, pr, g):
	return "%s_%05d_%02d_%d_%02d_%s_%s" % (ch, m, c, alpha, ddl, pr, g)

	
def genDataSet(desFileName, m, c, alpha, pos, ddl, pr, g):
	with open(desFileName, "w") as fout:
		line = "%s %s %s %s\n" % (m, c, g, alpha)
		fout.write(line)
		for i in xrange(m):
			fout.write("%s %d\n" % (pos[i], c))
		fout.write("%s %s\n" % (ddl, pr))

			
def	genOneSet(desFilePath, dataSetId, Vmax, token):
	if not os.path.exists(desFilePath):
		os.mkdir(desFilePath)
	Mmax = max(CFDS.workerNumList)
	pos = randomGenerator(Vmax).gen(Mmax)
	m, c, alpha, ddl, pr, g = CFDS.defaultValues
	
	# varying of workerNum
	for m in CFDS.workerNumList:
		tmpFilePath = genDataSetName(token, m, c, alpha, ddl, pr, g)
		tmpFilePath = os.path.join(desFilePath, tmpFilePath)
		if not os.path.exists(tmpFilePath):
			os.mkdir(tmpFilePath)
		desFileName = "data_%02d.txt" % (dataSetId)
		desFileName = os.path.join(tmpFilePath, desFileName)
		if os.path.exists(desFileName):
			continue
		genDataSet(desFileName, m, c, alpha, pos, ddl, pr, g)
	m, c, alpha, ddl, pr, g = CFDS.defaultValues

	# varying of capacity
	for c in CFDS.capList:
		tmpFilePath = genDataSetName(token, m, c, alpha, ddl, pr, g)
		tmpFilePath = os.path.join(desFilePath, tmpFilePath)
		if not os.path.exists(tmpFilePath):
			os.mkdir(tmpFilePath)
		desFileName = "data_%02d.txt" % (dataSetId)
		desFileName = os.path.join(tmpFilePath, desFileName)
		if os.path.exists(desFileName):
			continue
		genDataSet(desFileName, m, c, alpha, pos, ddl, pr, g)
	m, c, alpha, ddl, pr, g = CFDS.defaultValues


	# varying of deadline
	for ddl in CFDS.ddlList:
		tmpFilePath = genDataSetName(token, m, c, alpha, ddl, pr, g)
		tmpFilePath = os.path.join(desFilePath, tmpFilePath)
		if not os.path.exists(tmpFilePath):
			os.mkdir(tmpFilePath)
		desFileName = "data_%02d.txt" % (dataSetId)
		desFileName = os.path.join(tmpFilePath, desFileName)
		if os.path.exists(desFileName):
			continue
		genDataSet(desFileName, m, c, alpha, pos, ddl, pr, g)
	m, c, alpha, ddl, pr, g = CFDS.defaultValues


	# varying of penalty
	for pr in CFDS.prList:
		tmpFilePath = genDataSetName(token, m, c, alpha, ddl, pr, g)
		tmpFilePath = os.path.join(desFilePath, tmpFilePath)
		if not os.path.exists(tmpFilePath):
			os.mkdir(tmpFilePath)
		desFileName = "data_%02d.txt" % (dataSetId)
		desFileName = os.path.join(tmpFilePath, desFileName)
		if os.path.exists(desFileName):
			continue
		genDataSet(desFileName, m, c, alpha, pos, ddl, pr, g)
	m, c, alpha, ddl, pr, g = CFDS.defaultValues
	
	
	# varying of grid
	for g in CFDS.gridList:
		tmpFilePath = genDataSetName(token, m, c, alpha, ddl, pr, g)
		tmpFilePath = os.path.join(desFilePath, tmpFilePath)
		if not os.path.exists(tmpFilePath):
			os.mkdir(tmpFilePath)
		desFileName = "data_%02d.txt" % (dataSetId)
		desFileName = os.path.join(tmpFilePath, desFileName)
		if os.path.exists(desFileName):
			continue
		genDataSet(desFileName, m, c, alpha, pos, ddl, pr, g)
	m, c, alpha, ddl, pr, g = CFDS.defaultValues

	
def genChengdu(desFilePath, dataSetN, Vmax = 214440):	
	for i in xrange(31,dataSetN):
		genOneSet(desFilePath, i, Vmax, 'C')

def exp0(dataSetN = 61):
	desFilePath = "./chengduTaxi"
	if not os.path.exists(desFilePath):
		os.mkdir(desFilePath)
	genChengdu(desFilePath, dataSetN)
	
	
if __name__ == "__main__":
	exp0()
	
