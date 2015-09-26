// $Id: ctrlTable.cpp 9357 2014-04-25 15:35:25Z FloSoft $
//
// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

///////////////////////////////////////////////////////////////////////////////
// Header
#include "defines.h"
#include "ctrlStatisticTable.h"
#include "ctrlButton.h"
#include "ctrlPercent.h"

#include "Loader.h"
#include "ogl/glArchivItem_Font.h"
#include <sstream>
#include <cstdarg>

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


ctrlStatisticTable::ctrlStatisticTable(Window* parent,
                     unsigned int id,
                     unsigned short x,
                     unsigned short y,
                     unsigned short width,
                     unsigned short height, 
                     unsigned num_columns,
                     unsigned max_num_rows)
    : Window(x, y, id, parent, width, height), _num_columns(num_columns), _max_num_rows(max_num_rows), _num_rows(0)
{
    s.col_width = ScaleX(100);
    s.player_col_width = ScaleX(100);

    s.x_start = s.player_col_width;
    s.y_start = 0;

    s.x_grid = (width - s.player_col_width) / (num_columns - 1);
    s.y_grid = height / (8 + 1); // fix for now, TODO: add scrollbar
}


ctrlStatisticTable::~ctrlStatisticTable(void)
{

}


void ctrlStatisticTable::Resize_(unsigned short width, unsigned short height)
{
    // TODO: is this needed?
}

void ctrlStatisticTable::AddPlayerInfos(const std::vector<EndStatisticData::PlayerInfo> &player_infos)
{
    assert(player_infos.size() == _max_num_rows);

    _players = player_infos;

    SetScale(false);

    // Add column header
    AddText(1, 
        s.player_col_width/4, 
        5, 
        _("Player"), COLOR_YELLOW, glArchivItem_Font::DF_LEFT, NormalFont);


    for (unsigned i = 0; i < player_infos.size(); ++i)
    {
        unsigned id = 10 + i;

        AddText(id, 
            (s.player_col_width/4), 
            (i+1) * s.y_grid, 
            player_infos[i].name, COLOR_YELLOW, glArchivItem_Font::DF_LEFT, NormalFont);

        // TODO draw team marker
        // TODO draw nation marker
        // TODO draw alive/winner marker
    }

    SetScale(true);
}

void ctrlStatisticTable::AddColumn(unsigned col_idx, 
    const std::string& title,
    bool is_button,
    const std::string& tooltip,
    const std::vector<unsigned> &points)
{
    assert(col_idx > 0);
    assert(points.size() == _max_num_rows);

    SetScale(false);

    // Button/Title für die Spalte hinzufügen
    if (is_button)
    {
        AddTextButton(col_idx + 1, 
            s.x_start + (col_idx-1) * s.x_grid + (s.x_grid/2) - (s.col_width/2), 
            0, 
            s.col_width, 22, TC_GREY, title, NormalFont);
    }
    else
    {
        ctrlText *text = AddText(col_idx + 1, 
            s.x_start + (col_idx-1) * s.x_grid + (s.x_grid/2), 
            5, 
            title, COLOR_YELLOW, glArchivItem_Font::DF_CENTER, NormalFont);

        text->SetToolTipText(tooltip);
    }
    

    unsigned max_points = *std::max_element(points.begin(), points.end());

    for(unsigned short i = 0; i < points.size(); ++i)
    {
        unsigned id = 10 + col_idx * _max_num_rows + i;

        std::stringstream ss;

        ss << points[i];
        unsigned box_height = ScaleY(22);
        unsigned height_center_offset = ScaleY(40)/2 - (box_height/2) - 4; // TODO why 4?

        AddColorBar(id, 
            s.x_start + (col_idx - 1) * s.x_grid + (s.x_grid/2) - (s.col_width/2),
            s.y_start + (i + 1) * s.y_grid - height_center_offset,
            s.col_width, box_height, 
            TC_GREY,
            COLOR_YELLOW, NormalFont, COLORS[i], points[i], max_points);

        if (points[i] == max_points && max_points != 0)
        {

            AddImage(100 + col_idx * _max_num_rows + i,
                s.x_start + (col_idx - 1) * s.x_grid + (s.x_grid/2) - (s.col_width/2) + 16,
                s.y_start + (i + 1) * s.y_grid - height_center_offset + 16,
                LOADER.GetImageN("io_new", 14), _("best in category"));

        }
    }

    SetScale(true);
}

bool ctrlStatisticTable::Draw_()
{
    SetScale(false);
    //Draw3D(GetX(), GetY(), width, height, tc, 2);

    // Die farbigen Zeilen malen
    for(unsigned i = 0; i < _max_num_rows; ++i)
    {
        // Rechtecke in Spielerfarbe malen mit entsprechender Transparenz
        // Font Height = 12
        unsigned box_height = ScaleY(40);
        unsigned height_center_offset = (box_height / 2) - (12/2);
        Window::DrawRectangle(GetX(), GetY() + (i+1) * s.y_grid - height_center_offset, width_, box_height, (COLORS[_players[i].color] & 0x00FFFFFF) | 0x40000000);
    }
    SetScale(true);
    DrawControls();



    return true;
}


void ctrlStatisticTable::Msg_ButtonClick(const unsigned int ctrl_id)
{
    if (ctrl_id > 1 && ctrl_id < _num_columns + 1)
    {
        parent_->Msg_StatisticGroupChange(id_, ctrl_id - 1);
    }
}

bool ctrlStatisticTable::Msg_LeftDown(const MouseCoords& mc)
{
    return RelayMouseMessage(&Window::Msg_LeftDown, mc);
}

bool ctrlStatisticTable::Msg_MouseMove(const MouseCoords& mc)
{
    //for (
    //if(Coll(mc.x, mc.y, GetX(), GetY(), width, height))
    //{
    //    WINDOWMANAGER.SetToolTip(this, tooltip);
    //}
    //else
    //{
    //    WINDOWMANAGER.SetToolTip(this, "");
    //}

    // ButtonMessages weiterleiten
    return RelayMouseMessage(&Window::Msg_MouseMove, mc);
}

bool ctrlStatisticTable::Msg_LeftUp(const MouseCoords& mc)
{
    return RelayMouseMessage(&Window::Msg_LeftUp, mc);
}

