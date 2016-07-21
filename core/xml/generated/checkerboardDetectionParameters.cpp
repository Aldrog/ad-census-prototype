/**
 * \file checkerboardDetectionParameters.cpp
 * \attention This file is automatically generated and should not be in general modified manually
 *
 * \date MMM DD, 20YY
 * \author autoGenerator
 */

#include <vector>
#include <stddef.h>
#include "checkerboardDetectionParameters.h"

/**
 *  Looks extremely unsafe because it depends on the order of static initialization.
 *  Should check standard if this is ok
 *
 *  Also it's not clear why removing "= Reflection()" breaks the code;
 **/

namespace corecvs {
template<>
Reflection BaseReflection<CheckerboardDetectionParameters>::reflection = Reflection();
template<>
int BaseReflection<CheckerboardDetectionParameters>::dummy = CheckerboardDetectionParameters::staticInit();
} // namespace corecvs 

SUPPRESS_OFFSET_WARNING_BEGIN

int CheckerboardDetectionParameters::staticInit()
{

    ReflectionNaming &nameing = naming();
    nameing = ReflectionNaming(
        "Checkerboard Detection Parameters",
        "Checkerboard Detection Parameters",
        ""
    );
     

    fields().push_back(
        new BoolField
        (
          CheckerboardDetectionParameters::ESTIMATE_UNDISTORTED_FROM_DISTORTED_ID,
          offsetof(CheckerboardDetectionParameters, mEstimateUndistortedFromDistorted),
          true,
          "Estimate undistorted from distorted",
          "Estimate undistorted from distorted",
          "Estimate undistorted from distorted"
        )
    );
    fields().push_back(
        new BoolField
        (
          CheckerboardDetectionParameters::USE_UNDISTORTION_ID,
          offsetof(CheckerboardDetectionParameters, mUseUndistortion),
          true,
          "Use Undistortion",
          "Use Undistortion",
          "Use Undistortion"
        )
    );
    fields().push_back(
        new EnumField
        (
          CheckerboardDetectionParameters::ALGORITHM_ID,
          offsetof(CheckerboardDetectionParameters, mAlgorithm),
          0,
          "Algorithm",
          "Algorithm",
          "Algorithm",
          new EnumReflection(2
          , new EnumOption(0,"Homebrew Detector")
          , new EnumOption(1,"OpenCV Detector")
          )
        )
    );
    fields().push_back(
        new EnumField
        (
          CheckerboardDetectionParameters::CHANNEL_ID,
          offsetof(CheckerboardDetectionParameters, mChannel),
          3,
          "Channel",
          "Channel",
          "Channel",
          new EnumReflection(15
          , new EnumOption(0,"R")
          , new EnumOption(1,"G")
          , new EnumOption(2,"B")
          , new EnumOption(3,"Alpha")
          , new EnumOption(4,"Y")
          , new EnumOption(5,"Cr")
          , new EnumOption(6,"Cb")
          , new EnumOption(7,"U")
          , new EnumOption(8,"V")
          , new EnumOption(9,"Chroma")
          , new EnumOption(10,"Gray")
          , new EnumOption(11,"Luma")
          , new EnumOption(12,"Hue")
          , new EnumOption(13,"Saturation")
          , new EnumOption(14,"Value")
          )
        )
    );
    fields().push_back(
        new DoubleField
        (
          CheckerboardDetectionParameters::CELL_SIZE_HOR_ID,
          offsetof(CheckerboardDetectionParameters, mCellSizeHor),
          50,
          "Cell Size Hor",
          "Cell Size Hor",
          "Cell Size Hor"
        )
    );
    fields().push_back(
        new DoubleField
        (
          CheckerboardDetectionParameters::CELL_SIZE_VERT_ID,
          offsetof(CheckerboardDetectionParameters, mCellSizeVert),
          50,
          "Cell Size Vert",
          "Cell Size Vert",
          "Cell Size Vert"
        )
    );
    fields().push_back(
        new BoolField
        (
          CheckerboardDetectionParameters::CLEAN_EXISTING_ID,
          offsetof(CheckerboardDetectionParameters, mCleanExisting),
          true,
          "Clean existing",
          "Clean existing",
          "Clean existing"
        )
    );
    fields().push_back(
        new IntField
        (
          CheckerboardDetectionParameters::PRECISE_DIAMETER_ID,
          offsetof(CheckerboardDetectionParameters, mPreciseDiameter),
          50,
          "Precise Diameter",
          "Precise Diameter",
          "Precise Diameter",
          true,
         0,
         999999
        )
    );
    fields().push_back(
        new IntField
        (
          CheckerboardDetectionParameters::ITERATION_COUNT_ID,
          offsetof(CheckerboardDetectionParameters, mIterationCount),
          50,
          "Iteration count",
          "Iteration count",
          "Iteration count",
          true,
         1,
         999999
        )
    );
    fields().push_back(
        new DoubleField
        (
          CheckerboardDetectionParameters::MIN_ACCURACY_ID,
          offsetof(CheckerboardDetectionParameters, mMinAccuracy),
          0.01,
          "Min accuracy",
          "Min accuracy",
          "Min accuracy",
          true,
         0,
         999999
        )
    );
    fields().push_back(
        new BoolField
        (
          CheckerboardDetectionParameters::PARTIAL_BOARD_ID,
          offsetof(CheckerboardDetectionParameters, mPartialBoard),
          true,
          "Partial Board",
          "Partial Board",
          "Partial Board"
        )
    );
    fields().push_back(
        new BoolField
        (
          CheckerboardDetectionParameters::FAST_BOARD_SPEEDUP_ID,
          offsetof(CheckerboardDetectionParameters, mFastBoardSpeedup),
          true,
          "Fast board Speedup",
          "Fast board Speedup",
          "Fast board Speedup"
        )
    );
    fields().push_back(
        new BoolField
        (
          CheckerboardDetectionParameters::DRAW_SGFS_ON_BOARDS_ID,
          offsetof(CheckerboardDetectionParameters, mDrawSGFsOnBoards),
          true,
          "Draw SGFs on boards",
          "Draw SGFs on boards",
          "Draw SGFs on boards"
        )
    );
    fields().push_back(
        new BoolField
        (
          CheckerboardDetectionParameters::SKIP_UNDISTORTED_WITH_NO_DISTORTED_BOARD_ID,
          offsetof(CheckerboardDetectionParameters, mSkipUndistortedWithNoDistortedBoard),
          false,
          "Skip undistorted with no distorted board",
          "Skip undistorted with no distorted board",
          "Skip undistorted with no distorted board"
        )
    );
   return 0;
}

SUPPRESS_OFFSET_WARNING_END


