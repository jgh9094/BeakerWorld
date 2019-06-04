#ifndef WEB_INTERFACE__H
#define WEB_INTERFACE__H

// Standard includes
#include <iostream>

// Empirical includes
#include "web/Animate.h"
#include "web/Button.h"
#include "web/web.h"
#include "web/Canvas.h"

// Experiment includes
#include "BeakerWorld.h"
#include "config.h"

namespace UI = emp::web;

class WebInterface : public UI::Animate
{
    BeakerWorldConfig config;               ///< Configurations we are uploading.
    UI::Document control_viewer;            ///< Object in charge of the controls.
    UI::Document beaker_viewer;             ///< Object in charge of beaker view.
    UI::Document stats_viewer;              ///< Object in charge of stats view.
    BeakerWorld world;                      ///< Object in charge of managing the world.
    emp::vector<std::string> heat_map;      ///< Variable that holds the heat map colors

    public:     

        WebInterface(): control_viewer("emp_controls"), beaker_viewer("emp_beaker"),
                        stats_viewer("emp_stats"), world(config)
        {
            Config_HM();

            // Adding the start/stop button!
            control_viewer << UI::Button(
                [this]()
                {
                    this->DoStart();
                }, "Start", "start_btn")
                << " Press to start/stop simulation!" 
                << "<br style='line-height: 30px' />";

            // Adding the restart button!
            control_viewer << UI::Button(
                [this]()
                {
                    this->DoReset();
                }, "Reset", "reset_btn")
                << " Press to reset the simulation to the beginning!" 
                << "<br>";
            stats_viewer << "World Statistics: "
            << "<br>" 
            << "Update @: " << UI::Live(
                [this]()
                {
                    return world.GetUpdate();
                }
            )
            << " | Population Size: "
            << UI::Live(
                [this]()
                {
                    return world.GetSize();
                }
            )
            << " | # of Deaths: "
            << UI::Live(
                [this]()
                {
                    return (size_t) world.GetDeaths();
                }
            )
            << "<br>";
            // Adding the canvas to draw organsisms!
            beaker_viewer << UI::Canvas(config.WORLD_X(), config.WORLD_Y(), "beaker_view");
            UI::Draw(beaker_viewer.Canvas("beaker_view"), world.GetSurface(), heat_map);
        }

        void Redraw();  ///< Function dedicated to redrawing objects on screen
        void DoStart(); ///< Function responsible for start button actions
        void DoStep();  ///< Function responsible for step buttion actions [TODO]
        void DoReset(); ///< Function responsible for reset button actions
        void DoFrame(); ///< Function responsible for drawing a frame *overloaded*

        void Config_HM();              ///< Function dedicated to configuring the heat map
};

void WebInterface::Redraw() ///< Function dedicated to redrawing objects on screen
{
    stats_viewer.Redraw();
    
}

void WebInterface::DoStart() ///< Function responsible for start button actions
{
    auto start_btn = control_viewer.Button("start_btn");
    auto reset_btn = control_viewer.Button("reset_btn");

    // If animation is actvie...
    if(GetActive())
    {
        reset_btn.SetDisabled(false);
        start_btn.SetLabel("Start");
        ToggleActive();
    }
    // If button is on
    else
    {
        reset_btn.SetDisabled(true);
        start_btn.SetLabel("Stop");
        ToggleActive();
    }
}

void WebInterface::DoStep() ///< Function responsible for step button actions
{

}

void WebInterface::DoReset() ///< Function responsible for reset button actions
{

}

void WebInterface::DoFrame() ///< Function responsible for drawing a frame *overloaded*
{
    if(GetActive())
    {
        world.Update();
        UI::Draw(beaker_viewer.Canvas("beaker_view"), world.GetSurface(), heat_map);
        WebInterface::Redraw();
    }
}

void WebInterface::Config_HM() ///< Function dedicated to configuring the heat map
{
  // Level 0 heat: Blue
  heat_map.push_back(emp::ColorRGB(0,0,225));
  // Level 1 heat: Cyan
  heat_map.push_back(emp::ColorRGB(0,255,255));
  // Level 2 heat: Green Yellow
  heat_map.push_back(emp::ColorRGB(173, 255, 47));
  // Level 3 heat: Yellow
  heat_map.push_back(emp::ColorRGB(255, 255, 0));
  // Level 4 heat: Red
  heat_map.push_back(emp::ColorRGB(255, 0, 0));
  // Level 5 heat: White
  heat_map.push_back(emp::ColorRGB(245, 245, 255));
  // Level 6 (resource only: magenta
  heat_map.push_back(emp::ColorRGB(255, 0, 255));
}


#endif