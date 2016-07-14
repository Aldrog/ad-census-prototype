isEmpty(OPENCV_WRAPPER_DIR) {
    message(Incorrect usage of opencv.pri with empty OPENCV_WRAPPER_DIR. OpenCV is switched off!)
} else {
    include(opencvLibs.pri)
}

contains(DEFINES, WITH_OPENCV) {                    # if it's installed properly with found path for lib

    INCLUDEPATH += $$OPENCV_WRAPPER_DIR

    HEADERS += \
		$$OPENCV_WRAPPER_DIR/featureDetectorCV.h \
		$$OPENCV_WRAPPER_DIR/KLTFlow.h \
		$$OPENCV_WRAPPER_DIR/openCvDescriptorExtractorWrapper.h \
		$$OPENCV_WRAPPER_DIR/openCvDescriptorMatcherWrapper.h \
		$$OPENCV_WRAPPER_DIR/openCvFeatureDetectorWrapper.h \
		$$OPENCV_WRAPPER_DIR/openCvFileReader.h \
		$$OPENCV_WRAPPER_DIR/openCvKeyPointsWrapper.h \
		$$OPENCV_WRAPPER_DIR/OpenCVTools.h \
		$$OPENCV_WRAPPER_DIR/semiGlobalBlockMatching.h \

    SOURCES += \
		$$OPENCV_WRAPPER_DIR/openCvFeatureDetectorWrapper.cpp \
		$$OPENCV_WRAPPER_DIR/featureDetectorCV.cpp \
		$$OPENCV_WRAPPER_DIR/OpenCVTools.cpp \
		$$OPENCV_WRAPPER_DIR/openCvDescriptorMatcherWrapper.cpp \
		$$OPENCV_WRAPPER_DIR/openCvFileReader.cpp \
		$$OPENCV_WRAPPER_DIR/KLTFlow.cpp \
		$$OPENCV_WRAPPER_DIR/openCvKeyPointsWrapper.cpp \
		$$OPENCV_WRAPPER_DIR/openCvDescriptorExtractorWrapper.cpp \
		$$OPENCV_WRAPPER_DIR/semiGlobalBlockMatching.cpp \


    # Face Detection
    #
    INCLUDEPATH += $$OPENCV_WRAPPER_DIR/faceDetect

    HEADERS     += $$OPENCV_WRAPPER_DIR/faceDetect/faceDetect.h
    SOURCES     += $$OPENCV_WRAPPER_DIR/faceDetect/faceDetect.cpp

#    with_chessdetector {
    HEADERS += \
                $$OPENCV_WRAPPER_DIR/openCvCheckerboardDetector.h

    SOURCES += \
                $$OPENCV_WRAPPER_DIR/openCvCheckerboardDetector.cpp
#    }

}
