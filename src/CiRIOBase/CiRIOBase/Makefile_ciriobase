VPATH	= .


# target might be WINDOWS, OS_X or LINUX

TARGET = OS_X

# point to the inclue directory
#
#INCLUDE = -Isource  -I/sw/include -I../include/ -I/usr/X11/include/freetype2/ -I/sw/include
INCLUDE = -Isource   -I../include/ 
SOURCE = source


# compiler specific flags
CFLAGS = -O3 -D$(TARGET)
# -heap-arrays -vec-report0
FFLAGS = -framework OpenGL
# -framework Carbon 

# Ususally, one doesn't need to bother with the rest of the makefile


PROJECT = libCiRIOBase

obj 	=  CUtil.o CVector.o CMath.o CGeometry.o OGLShader.o CApplication.o CBitmap.o COpenGL.o CTexture.o C2DField.o VoronoiDiagramGenerator.o CTextureMachine.o CDots.o CShaders.o


C 	= g++

default: $(PROJECT)

$(PROJECT):   $(obj) 
	ar cr $(PROJECT).a *.o
	cp $(PROJECT).a ../libs


$(PROJECT).o: $(PROJECT).cpp  $(obj)
	$(C) $(CFLAGS) $(INCLUDE)  -c $(PROJECT).cpp 



CUtil.o: $(SOURCE)/CUtil.cpp
	$(C) $(CFLAGS)  $(INCLUDE)  -c  $(SOURCE)/$(UTIL)/CUtil.cpp

CMath.o: $(SOURCE)/CMath.cpp
	$(C) $(CFLAGS)  $(INCLUDE)  -c  $(SOURCE)/$(UTIL)/CMath.cpp

CTentacle.o: $(SOURCE)/CTentacle.cpp
	$(C) $(CFLAGS)  $(INCLUDE)  -c  $(SOURCE)/$(UTIL)/CTentacle.cpp

CVector.o: $(SOURCE)/CVector.cpp
	$(C) $(CFLAGS)  $(INCLUDE)  -c  $(SOURCE)/$(UTIL)/CVector.cpp

OGLShader.o: $(SOURCE)/OGLShader.cpp
	$(C) $(CFLAGS)  $(INCLUDE)  -c  $(SOURCE)/$(OGL)/OGLShader.cpp

CGeometry.o: $(SOURCE)/CGeometry.cpp
	$(C) $(CFLAGS)  $(INCLUDE)  -c  $(SOURCE)/$(OGL)/CGeometry.cpp

CBuilding.o: $(SOURCE)/CBuilding.cpp
	$(C) $(CFLAGS)  $(INCLUDE)  -c  $(SOURCE)/$(OGL)/CBuilding.cpp

CApplication.o: $(SOURCE)/CApplication.cpp
	$(C) $(CFLAGS)  $(INCLUDE)  -c  $(SOURCE)/CApplication.cpp


CBitmap.o: $(SOURCE)/CBitmap.cpp
	$(C) $(CFLAGS)  $(INCLUDE)  -c  $(SOURCE)/CBitmap.cpp


COpenGL.o: $(SOURCE)/COpenGL.cpp
	$(C) $(CFLAGS)  $(INCLUDE)  -c  $(SOURCE)/COpenGL.cpp

CBitMap.o: $(SOURCE)/CBitMap.cpp
	$(C) $(CFLAGS)  $(INCLUDE)  -c  $(SOURCE)/CBitMap.cpp

CTexture.o: $(SOURCE)/CTexture.cpp
	$(C) $(CFLAGS)  $(INCLUDE)  -c  $(SOURCE)/CTexture.cpp

CTextureMachine.o: $(SOURCE)/CTextureMachine.cpp
	$(C) $(CFLAGS)  $(INCLUDE)  -c  $(SOURCE)/CTextureMachine.cpp

C2DField.o: $(SOURCE)/C2DField.cpp
	$(C) $(CFLAGS)  $(INCLUDE)  -c  $(SOURCE)/C2DField.cpp

CDots.o: $(SOURCE)/CDots.cpp
	$(C) $(CFLAGS)  $(INCLUDE)  -c  $(SOURCE)/CDots.cpp


CShaders.o: $(SOURCE)/CShaders.cpp
	$(C) $(CFLAGS)  $(INCLUDE)  -c  $(SOURCE)/CShaders.cpp

VoronoiDiagramGenerator.o: $(SOURCE)/VoronoiDiagramGenerator.cpp
	$(C) $(CFLAGS)  $(INCLUDE)  -c  $(SOURCE)/VoronoiDiagramGenerator.cpp





clean:	
	rm *.o *.o	


