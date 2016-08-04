/*
 * test_rrtvis.cpp
 *
 *  Created on: Aug 3, 2016
 *      Author: rdu
 */

#include "vis/rrt_vis.h"

#include "map/map_info.h"
#include "planner/rrts_planner.h"
#include "common/planning_types.h"
#include "vis/vis_utils.h"

using namespace srcl_ctrl;
using namespace cv;

int main(int argc, char** argv)
{
	Mat vis_img;
	MapInfo map_info;

	map_info.map_size_x = 1500;
	map_info.map_size_y = 1500;
	map_info.SetWorldSize(1.0, 1.0);
	RRTVis::CreateRRTCanvas(map_info.map_size_x,map_info.map_size_y, vis_img);
	BoundingBox bbox;
	bbox.x.min = 500;
	bbox.x.max = 1000;
	bbox.y.min = 500;
	bbox.y.max = 1000;
	VisUtils::FillRectangularArea(vis_img, bbox, cv::Scalar(0));
	ImageUtils::BinarizeImage(vis_img, vis_img, 100);

	RRTStarPlanner planner;
	std::vector<Position2Dd> path;

	planner.ConfigLocalPlanner();
	planner.UpdateOccupancyMap(vis_img, map_info);

	Position2Dd start,goal;
	start.x = 0;
	start.y = 0;
	goal.x = 1;
	goal.y = 1;
	planner.SetStartAndGoal(start, goal);
	planner.SearchSolution(start, goal, path);

	RRTVis::VisRRTPath(path, map_info, vis_img, vis_img);

	// display visualization result
	namedWindow("Processed Image", WINDOW_NORMAL ); // WINDOW_AUTOSIZE
	imshow("Processed Image", vis_img);
	//imshow("Processed Image", planner.validity_checker_2d_->occupancy_map_);

	waitKey(0);
}


