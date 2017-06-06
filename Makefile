.PHONY: all clean

CC = g++
CPPFLAGS = -O3 -Wall -Wno-deprecated-declarations
MACFLAG = -framework GLUT -framework OpenGL -framework Cocoa
LINUXFLAG = -lGL -lGLU -lglut

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    FLAGS = $(CPPFLAGS) $(LINUXFLAG)
else
	FLAGS = $(CPPFLAGS) $(MACFLAG)
endif

OBJ = \
	DataStructs/DoubleRecurse.o \
	DataStructs/KdTree.o \
	Graphics/BumpMapFunction.o \
	Graphics/CameraView.o \
	Graphics/DirectLight.o \
	Graphics/Extents.o \
	Graphics/Material.o \
	Graphics/MaterialCookTorrance.o \
	Graphics/PixelArray.o \
	Graphics/RgbImage.o \
	Graphics/TextureAffineXform.o \
	Graphics/TextureBilinearXform.o \
	Graphics/TextureCheckered.o \
	Graphics/TextureMapBase.o \
	Graphics/TextureMultiFaces.o \
	Graphics/TextureRgbImage.o \
	Graphics/TextureSequence.o \
	Graphics/TransformViewable.o \
	Graphics/ViewableBase.o \
	Graphics/ViewableBezierSet.o \
	Graphics/ViewableCone.o \
	Graphics/ViewableCylinder.o \
	Graphics/ViewableEllipsoid.o \
	Graphics/ViewableParallelepiped.o \
	Graphics/ViewableParallelogram.o \
	Graphics/ViewableSphere.o \
	Graphics/ViewableTorus.o \
	Graphics/ViewableTriangle.o \
	OpenglRender/GlutRenderer.o \
	RayTraceKd/RayTraceKd.o \
	RayTraceKd/RayTraceSetup2.o \
	RayTraceKd/RayTraceStats.o \
	RaytraceMgr/LoadNffFile.o \
	RaytraceMgr/LoadObjFile.o \
	RaytraceMgr/SceneDescription.o \
	VrMath/Aabb.o \
	VrMath/LinearR2.o \
	VrMath/LinearR3.o \
	VrMath/LinearR4.o \
	VrMath/Parallelepiped.o \
	VrMath/PolygonClip.o \
	VrMath/PolynomialRC.o \
	VrMath/Quaternion.o \

raytracekd.out: $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o raytracekd.out

clean: 
	@rm $(OBJ) raytracekd.out 2>/dev/null || true

