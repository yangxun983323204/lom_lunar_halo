import sys
import shutil

print("OnPostBuild begin")

isDebug = sys.argv[1]=="Debug"

copyTo = "../x64/"
if isDebug:
  copyTo += "Debug/"
else:
  copyTo += "Release/"

def CopyFile(src, dst):
  shutil.copy(src,dst)
  print("copy:"+src+"->"+dst)

rmlUiFrom = "../deps/RmlUi-vs2017-win64/Bin/"
if isDebug:
  rmlUiFrom += "Dynamic-Debug/"
else:
  rmlUiFrom += "Dynamic-Release/"

rmlUiFrom += "RmlCore.dll"
rmlUiTo = copyTo+"RmlCore.dll"
CopyFile(rmlUiFrom,rmlUiTo)

freeTypeFrom = "../deps/RmlUi-vs2017-win64/Dependencies/freetype-2.10.1/freetype.dll"
freeTypeTo = copyTo + "freetype.dll"
CopyFile(freeTypeFrom,freeTypeTo)

CopyFile("./config.json",copyTo+"config.json")
CopyFile("./wil.json",copyTo+"wil.json")

print("OnPostBuild end")