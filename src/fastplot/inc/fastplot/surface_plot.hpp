/* 
 * surface_plot.hpp
 * 
 * Created on: Jan 10, 2018 14:15
 * Description: 
 * 
 * Copyright (c) 2018 Ruixiang Du (rdu)
 */

#ifndef SURFACE_PLOT_HPP
#define SURFACE_PLOT_HPP

#include <cmath>

#include <Eigen/Dense>

#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

namespace librav
{
class SurfacePlot
{
public:
  SurfacePlot();
  ~SurfacePlot() = default;

  template <typename DerivedVector1, typename DerivedVector2, typename DerivatedMatrix>
  void ShowSurface(const Eigen::MatrixBase<DerivedVector1> &x, const Eigen::MatrixBase<DerivedVector2> &y, const Eigen::MatrixBase<DerivatedMatrix> &z, bool do_warp = false, bool show_box = true, bool show_axes = true, bool show_bar = true)
  {
    // Get size of the surface
    const int size_x = x.rows();
    const int size_y = y.rows();
    double wrap_scale = 1.0;

    // std::cout << "size_x : " << size_x << std::endl;
    // std::cout << "size_y : " << size_y << std::endl;
    // std::cout << "size_z : " << z.rows() << " by " << z.cols() << std::endl;

    assert(size_x == z.rows());
    assert(size_y == z.cols());

    double x_range = std::abs(x(0) - x(size_x - 1));
    double y_range = std::abs(y(0) - y(size_y - 1));
    double z_range = std::abs(z.maxCoeff() - z.minCoeff());
    // std::cout << "range x : " << x_range << std::endl;
    // std::cout << "range y : " << y_range << std::endl;
    // std::cout << "range z : " << z_range << std::endl;

    if (do_warp)
    {
      double xy_range = std::min(x_range, y_range);
      wrap_scale = xy_range / z_range;
    }

    // setup camera according to data
    focal_position_[0] = x.minCoeff() + x_range / 2.0;
    focal_position_[1] = y.minCoeff() + y_range / 2.0;
    focal_position_[2] = z.minCoeff() + z_range / 2.0;

    // create a grid
    vtkSmartPointer<vtkStructuredGrid> structured_grid =
        vtkSmartPointer<vtkStructuredGrid>::New();

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

    for (unsigned int j = 0; j < size_y; j++)
      for (unsigned int i = 0; i < size_x; i++)
        points->InsertNextPoint(x(i), y(j), z(i, j));

    // Specify the dimensions of the grid
    structured_grid->SetDimensions(size_x, size_y, 1);
    structured_grid->SetPoints(points);

    vtkSmartPointer<vtkFloatArray> colors = vtkSmartPointer<vtkFloatArray>::New();
    colors->SetNumberOfComponents(1);
    colors->SetNumberOfTuples(size_x * size_y);
    int k = 0;
    for (int j = 0; j < size_y; j++)
      for (int i = 0; i < size_x; i++)
      {
        colors->InsertComponent(k, 0, z(i, j));
        k++;
      }
    structured_grid->GetPointData()->SetScalars(colors);

    RenderSurface(structured_grid, do_warp, wrap_scale, show_box, show_axes, show_bar);
  }

  void SetCameraPosition(double cam_pos[3]);
  void SetFocalPosition(double foc_pos[3]);

private:
  vtkSmartPointer<vtkStructuredGrid> structured_grid_;
  vtkSmartPointer<vtkRenderer> renderer_;
  vtkSmartPointer<vtkRenderWindow> render_window_;
  vtkSmartPointer<vtkRenderWindowInteractor> render_window_interactor_;

  double camera_position_[3] = {-1.0, -1.0, 1.0};
  double focal_position_[3] = {0, 0, 0};

  void RenderSurface(vtkSmartPointer<vtkStructuredGrid> structured_grid, bool do_warp, double wrap_scale, bool show_box, bool show_axes, bool show_bar);
};
}

#endif /* SURFACE_PLOT_HPP */
