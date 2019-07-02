#ifndef WEB_INTERFACE__H
#define WEB_INTERFACE__H

// Standard includes
#include <iostream>

// Empirical includes
#include "web/Animate.h"
#include "web/Button.h"
#include "web/web.h"
#include "web/Canvas.h"
#include "web/JSWrap.h"

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
    // UI::Document hist_viewer;               ///< Object in charge of histogram view.
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
                << UI::Button(
                [this]()
                {
                    this->DoReset();
                }, "Reset", "reset_btn")
                << " Press to start/stop simulation!" 
                << "<br style='line-height: 30px' />";

            // Add the viewing of the world statistics!
            stats_viewer << "<u>World Statistics</u>:"
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
                    return world.GetNumOrgs();
                }
            )
            << " | id_map Size: "
            << UI::Live(
                [this]()
                {
                    return world.GetIDSize();
                }
            )
            << " | # of Deaths: "
            << UI::Live(
                [this]()
                {
                    return world.GetStv() + world.GetEat() + world.GetPop();
                }
            )
            << "<br>";

            // Adding the canvas to draw organsisms!
            beaker_viewer << UI::Canvas(config.WORLD_X(), config.WORLD_Y(), "beaker_view");
            UI::Draw(beaker_viewer.Canvas("beaker_view"), world.GetSurface(), heat_map);

            emp::JSWrap([this](){return world.GetBlue();}, "GetBlue", false);
            emp::JSWrap([this](){return world.GetCyan();}, "GetCyan", false);
            emp::JSWrap([this](){return world.GetLime();}, "GetLime", false);
            emp::JSWrap([this](){return world.GetYellow();}, "GetYellow", false);
            emp::JSWrap([this](){return world.GetRed();}, "GetRed", false);
            emp::JSWrap([this](){return world.GetWhite();}, "GetWhite", false);
            emp::JSWrap([this](){return world.GetStv();}, "GetStv", false);
            emp::JSWrap([this](){return world.GetEat();}, "GetEat", false);
            emp::JSWrap([this](){return world.GetPop();}, "GetPop", false);
        }

        /* Web/UI Functions*/

        void Redraw();                  ///< Function dedicated to redrawing objects on screen
        void DoStart();                 ///< Function responsible for start button actions
        void DoStep();                  ///< Function responsible for step buttion actions [TODO]
        void DoReset();                 ///< Function responsible for reset button actions
        void DoFrame();                 ///< Function responsible for drawing a frame *overloaded*
        void Config_HM();               ///< Function dedicated to configuring the heat map
        void RedrawChart();
};

void WebInterface::Redraw() ///< Function dedicated to redrawing objects on screen
{
    stats_viewer.Redraw();

    if(world.GetRedraw())
    {
        RedrawChart();        
        world.SetRedraw(false);
    }
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
  heat_map.push_back(emp::ColorRGB(173,255,47));
  // Level 3 heat: Yellow
  heat_map.push_back(emp::ColorRGB(255,255,0));
  // Level 4 heat: Red
  heat_map.push_back(emp::ColorRGB(255,0,0));
  // Level 5 heat: White
  heat_map.push_back(emp::ColorRGB(245,245,255));
  // Level 6 (resource only: magenta
  heat_map.push_back(emp::ColorRGB(255,0,255));
}

void WebInterface::RedrawChart()
{
    EM_ASM({
            var data = [];
            var data1 = [];
            var ultimateColors = [];
            ultimateColors.push('rgb(0,0,225)');
            ultimateColors.push('rgb(0,255,255)');
            ultimateColors.push('rgb(173,255,47)');
            ultimateColors.push('rgb(255,255,0)');
            ultimateColors.push('rgb(255,0,0)');
            ultimateColors.push('rgb(245,245,255)');
            marker = {colors: ultimateColors};

            data.push
            ({ values: [emp.GetBlue(), emp.GetCyan(), emp.GetLime(), emp.GetYellow(), emp.GetRed(), emp.GetWhite()],
                labels: ['Blue', 'Cyan', 'Lime', 'Yellow', 'Red', 'White'],
                domain: {row: 0},
                name: 'Popluation',
                marker,
                hoverinfo: 'label+percent+name+value',
                hole: .4,
                type: 'pie'
            });
            data.push
            ({
                values: [emp.GetStv(), emp.GetEat(), emp.GetPop()],
                labels: ['Starving', 'Eaten', 'Apoptosis'],
                text: 'CO2',
                textposition: 'inside',
                domain: {row: 1},
                name: 'Death',
                hoverinfo: 'label+percent+name+value',
                hole: .4,
                type: 'pie'
            });

            ann = [];

            ann.push({font: {size: 16},
                      showarrow: false,
                      text: 'Population Distribution',
                      y: 1.06
                      });

            ann.push({font: {size: 16},
                      showarrow: false,
                      text: 'Death Distribution'
                      });

            var layout = {annotations: ann};
            layout['height'] = 400;
            layout['width'] = 300;
            layout['showlegend'] = false;
            layout['grid'] = {};
            layout['grid']['rows'] = 2;
            layout['grid']['columns'] = 1;
            layout['margin'] = {};
            layout['margin']['l'] = 0;
            layout['margin']['r'] = 0;
            layout['margin']['b'] = 0;
            layout['margin']['t'] = 18;

            Plotly.newPlot('emp_hist1', data, layout);
        });
}

#endif