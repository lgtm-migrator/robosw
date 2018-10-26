/* 
 * lattice_viz.cpp
 * 
 * Created on: Oct 25, 2018 11:35
 * Description: 
 * 
 * Copyright (c) 2018 Ruixiang Du (rdu)
 */

#include "ugvnav_viz/lattice_viz.hpp"

#include "ugvnav_viz/details/lattice_draw.hpp"

using namespace librav;

void LightViz::ShowMotionStateTrajectory(const std::vector<MotionState> &states, int32_t pixel_per_unit, std::string window_name, bool save_img)
{
    CartesianCanvas canvas(pixel_per_unit);
    canvas.SetupCanvas(-8, 8, -5, 5);

    LatticeDraw ldraw(canvas);
    ldraw.DrawTrajectoryPoints(states);

    ShowImage(canvas.paint_area, window_name, save_img);
}

void LightViz::ShowMotionPrimitive(MotionPrimitive mp, double step, int32_t pixel_per_unit, std::string window_name, bool save_img)
{
    CartesianCanvas canvas(pixel_per_unit);
    canvas.SetupCanvas(-5, 15, -8, 8);

    LatticeDraw ldraw(canvas);
    ldraw.DrawMotionPrimitive(mp, step);

    ShowImage(canvas.paint_area, window_name, save_img);
}