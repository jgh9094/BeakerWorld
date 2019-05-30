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
    BeakerWorldConfig config;                  ///< Configurations we are uploading.
    UI::Document control_viewer;    ///< Object in charge of the controls.
    UI::Document beaker_viewer;     ///< Object in charge of beaker view.
    BeakerWorld world;            ///< Object in charge of managing the world.


    public:

        WebInterface(): control_viewer("emp_controls"), beaker_viewer("emp_beaker"),
                        world(config)
        {
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

            // Adding the canvas to draw organsisms!
            beaker_viewer << UI::Canvas(config.WORLD_X(), config.WORLD_Y(), "beaker_view");
            UI::Draw(beaker_viewer.Canvas("beaker_view"), world.GetSurface(), emp::GetHueMap(360));
        }

        void Redraw();
        void DoStart();
        void DoStep();
        void DoReset();
};

void WebInterface::Redraw()
{

}

void WebInterface::DoStart()
{

}

void WebInterface::DoStep()
{

}

void WebInterface::DoReset()
{

}

#endif