import os
import sys
import shutil

print("OnPostBuild begin")

isDebug = sys.argv[1]=="Debug"

copyTo = "../x64/"
if isDebug:
	copyTo += "Debug/"
else:
	copyTo += "Release/"

def Copy(src, dst):
	if os.path.isdir(src):
		if os.path.isdir(dst):
			shutil.rmtree(dst)
		shutil.copytree(src,dst)
	elif os.path.isfile(src):
		shutil.copy(src,dst)
	else:
		print("未识别文件或目录:"+src)
	print("copy:"+src+"->"+dst)

class CopyPair(object):
	"""docstring for CopyPair"""
	def __init__(self, src, dst):
		self.src = src
		self.dst = dst

copyArray = []

# rml
rmlUiFrom = "../deps/RmlUi-vs2017-win64/Bin/"
if isDebug:
	rmlUiFrom += "Dynamic-Debug/"
else:
	rmlUiFrom += "Dynamic-Release/"

copyArray.append(CopyPair(rmlUiFrom + "RmlCore.dll", copyTo+"RmlCore.dll"))
copyArray.append(CopyPair(rmlUiFrom + "RmlDebugger.dll", copyTo+"RmlDebugger.dll"))

freeTypeFrom = "../deps/RmlUi-vs2017-win64/Dependencies/freetype-2.10.1/freetype.dll"
freeTypeTo = copyTo + "freetype.dll"
copyArray.append(CopyPair(freeTypeFrom, freeTypeTo))

# config
copyArray.append(CopyPair("./config.json", copyTo+"config.json"))
copyArray.append(CopyPair("./ResDef", copyTo+"ResDef"))
copyArray.append(CopyPair("./UILayout", copyTo+"UILayout"))

# copy all
for pair in copyArray:
	Copy(pair.src, pair.dst)

print("OnPostBuild end")