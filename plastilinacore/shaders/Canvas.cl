/***************************************************************************
*   Copyright (C) 2016 by Juan Roberto Cabral Flores                      *
*   roberto.cabral@gmail.com                                              *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

kernel void clear(
    float4 color,
    write_only image2d_t dst
)
{
    uint gid_x = get_global_id(0);
    uint gid_y = get_global_id(1);

    write_imagef(dst, (int2)(gid_x, gid_y), color);
}

kernel void drawRectangle(
    write_only image2d_t dst
    , float4 lineColor
    , float4 fillColor
    , uint x, uint y
    , uint w, uint h
)
{
    uint gid_x = get_global_id(0);
    uint gid_y = get_global_id(1);

    if ( (gid_x < x) || (gid_x > x + w)
        || (gid_y < y) || (gid_y > y + h) )
    {
        return;
    }
    write_imagef(dst, (int2)(gid_x, gid_y), fillColor);
}

kernel void drawEllipse(
    write_only image2d_t dst
    , float4 lineColor
    , float4 fillColor
    , uint x, uint y
    , uint w, uint h
)
{
    uint gid_x = get_global_id(0);
    uint gid_y = get_global_id(1);

    float cx = (float)(gid_x) - (float)(x);  // - 25
    float cy = (float)(gid_y) - (float)(y);  //    0
    float eval = (cx*cx) + (cy*cy); // 625 + 0
    float r_sqr = w*w*0.25f; // 50*50*0.25 = 2500/4 = 625
    float r2_sqr = (w-10)*(w-10)*0.25f; // 50*50*0.25 = 2500/4 = 625

    if (eval < r_sqr && eval >r2_sqr)
    {
        write_imagef(dst, (int2)(gid_x, gid_y), lineColor);
    } 
    else if (eval <=r2_sqr)
    {
        write_imagef(dst, (int2)(gid_x, gid_y), fillColor);
    }
}


kernel void ink_drop(
    write_only image2d_t dst
    , read_only image2d_t src
    , float4 color
    )
{
    uint gid_x = get_global_id(0);
    uint gid_y = get_global_id(1);

    float4 c = read_imagef(src, (int2)(gid_x, gid_y));
    c += color;
    write_imagef(dst, (int2)(gid_x, gid_y), c);
}


kernel void ink_step(
    write_only image2d_t dst
    , read_only image2d_t src
    , global float4 * fieldSpeed
    , global float4 * fieldSpeed0
)
{
    uint gid_x = get_global_id(0);
    uint gid_y = get_global_id(1);
    float4 c = (float4)(0,0,0,0);

    if (gid_x == 0 || gid_y == 0 || gid_x == 1278 || gid_y == 718)
	{
		c = read_imagef(src, (int2)(gid_x, gid_y)) * 0.8f;
		write_imagef(dst, (int2)(gid_x, gid_y), c);
        return;
	}

    float mask[9] = { 0.00f, 0.00f, 0.00f
        , 0.00f, 0.25f, 0.00f
        , 0.25f, 0.25f, 0.25f };


    uint x = gid_x - 1;
    uint y = gid_y - 1;

    for(uint j = 0; j < 3; ++j)
    {
        for (uint i = 0; i < 3; ++i)
        {
            c += read_imagef(src, (int2)(x+i, y+j)) * mask[j*3+i];
        }
    }
    c.w = 1.0f;
    write_imagef(dst, (int2)(gid_x, gid_y), c);
}

