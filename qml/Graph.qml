/*
    Copyright (C) 2014 David Fries <David@Fries.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0

/* Draw the data in a psuedo 3D way (or not) since this is just a 2D canvas
 */
Canvas
{
	id: graph

	property var model
	property int dim: 5
	property real x_inc: width/(dim+1)
	property real y_inc: height/(dim+1)
	onModelChanged: requestPaint()

	renderStrategy: Canvas.FramebufferObject
	function drawLayer(ctx, c)
	{
		for(var x=0; x<dim; ++x)
		for(var y=0; y<dim; ++y)
		{
			ctx.beginPath()
			ctx.arc(x * x_inc, y * y_inc, 9, Math.PI, 0)
			if(model[x][c][y])
				ctx.fill();
			else
				ctx.stroke();
		}
	}
	onPaint:
	{
		var ctx = getContext('2d')
		ctx.reset()
		ctx.strokeStyle = "#60ffffff"
		ctx.fillStyle = "white"
		ctx.lineWidth = 1.9

		// some centering
		ctx.translate(x_inc*1.4, y_inc*1.4)
		// something of a perspective look
		var s = .98
		for(var c=0; c<dim; ++c)
		{
			drawLayer(ctx, c)
			ctx.translate(-x_inc/9, -y_inc/5)
			ctx.scale(s, s)
		}
	}
}
