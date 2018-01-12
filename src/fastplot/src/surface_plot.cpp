/* 
 * surface_plot.cpp
 * 
 * Created on: Jan 10, 2018 14:15
 * Description: 
 * 
 * Copyright (c) 2018 Ruixiang Du (rdu)
 */

#include "fastplot/surface_plot.hpp"

#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkOutlineFilter.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkWarpScalar.h>
#include <vtkProperty.h>
#include <vtkTextProperty.h>
#include <vtkCamera.h>
#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkCaptionActor2D.h>
#include <vtkTextActor.h>
#include <vtkScalarBarActor.h>
#include <vtkActorCollection.h>

using namespace librav;

SurfacePlot::SurfacePlot()
{
    structured_grid_ = vtkSmartPointer<vtkStructuredGrid>::New();
    renderer_ = vtkSmartPointer<vtkRenderer>::New();
    render_window_ = vtkSmartPointer<vtkRenderWindow>::New();
    render_window_interactor_ = vtkSmartPointer<vtkRenderWindowInteractor>::New();

    renderer_->GetActiveCamera()->SetViewUp(0, 0, 1);
    renderer_->GetActiveCamera()->SetPosition(camera_position_);
    renderer_->GetActiveCamera()->SetFocalPoint(focal_position_);
}

void SurfacePlot::SetCameraPosition(double cam_pos[3])
{
    for(int i = 0; i < 3; i++)  
        camera_position_[i] = cam_pos[i];
}

void SurfacePlot::SetFocalPosition(double foc_pos[3])
{
    for(int i = 0; i < 3; i++)  
        focal_position_[i] = foc_pos[i];
}

void SurfacePlot::RenderSurface(vtkSmartPointer<vtkStructuredGrid> structured_grid, bool do_warp, double wrap_scale, bool show_box, bool show_axes, bool show_bar)
{
    /**************************** Setup the grid ****************************/
    vtkSmartPointer<vtkStructuredGridGeometryFilter> geometryFilter =
        vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
    geometryFilter->SetInputData(structured_grid);
    geometryFilter->Update();

    // create a warper
    vtkSmartPointer<vtkWarpScalar> warp = vtkSmartPointer<vtkWarpScalar>::New();
    warp->SetInputConnection(geometryFilter->GetOutputPort());
    warp->XYPlaneOn();
    warp->SetScaleFactor(wrap_scale);

    // create a grid mapper and actor
    double grid_scalar_range[2];
    vtkSmartPointer<vtkDataSetMapper> gridMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    vtkSmartPointer<vtkActor> gridActor = vtkSmartPointer<vtkActor>::New();
    if (do_warp)
        gridMapper->SetInputConnection(warp->GetOutputPort());
    else
        gridMapper->SetInputConnection(geometryFilter->GetOutputPort());
    structured_grid->GetScalarRange(grid_scalar_range);
    gridMapper->SetScalarRange(grid_scalar_range[0], grid_scalar_range[1]);
    gridActor->SetMapper(gridMapper);
    gridActor->GetProperty()->EdgeVisibilityOn();
    // gridActor->GetProperty()->SetEdgeColor(0, 0, 1);

    /**************************** Setup the outline ****************************/
    // add outline to the surface
    vtkSmartPointer<vtkOutlineFilter> outlineFilter = vtkSmartPointer<vtkOutlineFilter>::New();
    if (do_warp)
        outlineFilter->SetInputData(warp->GetOutput());
    else
        outlineFilter->SetInputData(geometryFilter->GetOutput());
    outlineFilter->Update();
    vtkSmartPointer<vtkPolyDataMapper> outlineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    outlineMapper->SetInputConnection(outlineFilter->GetOutputPort());

    // create outline actor
    vtkSmartPointer<vtkActor> outlineActor = vtkSmartPointer<vtkActor>::New();
    outlineActor->SetMapper(outlineMapper);
    outlineActor->GetProperty()->SetColor(0, 0, 0);

    /**************************** Setup the axes ****************************/
    vtkSmartPointer<vtkAxesActor> axesActor = vtkSmartPointer<vtkAxesActor>::New();
    // axesActor->SetShaftTypeToLine();
    axesActor->GetXAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
    axesActor->GetYAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
    axesActor->GetZAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();

    vtkSmartPointer<vtkTextProperty> text_prop_ax = vtkSmartPointer<vtkTextProperty>(axesActor->GetXAxisCaptionActor2D()->GetCaptionTextProperty());
    text_prop_ax->SetColor(0.0, 0.0, 0.0);
    text_prop_ax->SetFontFamilyToArial();
    text_prop_ax->SetFontSize(20);
    // text_prop_ax->SetOpacity(0.35);
    axesActor->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->ShallowCopy(text_prop_ax);
    axesActor->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->ShallowCopy(text_prop_ax);

    /**************************** Setup the colorbar ****************************/
    // create colorbar
    vtkSmartPointer<vtkScalarBarActor> colorbarActor = vtkSmartPointer<vtkScalarBarActor>::New();
    colorbarActor->SetLookupTable(gridMapper->GetLookupTable());
    colorbarActor->SetWidth(0.065);
    colorbarActor->SetHeight(0.2);
    colorbarActor->SetPosition(0.9, 0.1);
    vtkSmartPointer<vtkTextProperty> text_prop_cb = vtkSmartPointer<vtkTextProperty>(colorbarActor->GetLabelTextProperty());
    text_prop_cb->SetColor(1.0, 1.0, 1.0);
    colorbarActor->SetLabelTextProperty(text_prop_cb);

    /**************************** Setup the rendering ****************************/
    // Remove existing actors
    if (renderer_->GetActors() != nullptr)
    {
        vtkProp *next = renderer_->GetActors()->GetNextActor();
        while (next != nullptr)
        {
            renderer_->RemoveActor(next);
            next = renderer_->GetActors()->GetNextActor();
        }
    }

    // Add the actor to the scene
    renderer_->AddActor(gridActor);
    if (show_box)
        renderer_->AddActor(outlineActor);
    if (show_axes)
        renderer_->AddActor(axesActor);
    if (show_bar)
        renderer_->AddActor(colorbarActor);
    renderer_->SetBackground(.2, .3, .4);

    renderer_->ResetCamera();

    // setup renderer, render window, and interactor
    render_window_->AddRenderer(renderer_);
    render_window_interactor_->SetRenderWindow(render_window_);

    // Render and interact
    render_window_->Render();
    render_window_interactor_->Start();
}