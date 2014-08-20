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
	onModelChanged: requestPaint()

	renderStrategy: Canvas.FramebufferObject
	onPaint:
	{
		var ctx = getContext('2d')
		ctx.reset()
		ctx.strokeStyle = "white"
		ctx.lineWidth = 1.9
		ctx.ellipse(width/4, height/4, width/5, height/7)
		ctx.stroke()
		console.log("model.length", model.length)
		for(var pl in model)
		{
			var layer = model[pl]
			console.log("layer", pl)
			for(var pc in layer)
			{
				var column = layer[pc]
				console.log("column", pc, column)
				/*
				console.log("\trow len", column.length)
				console.log("column", column)
				for(var pr in column)
				{
					var row = column[pr]
					console.log("\trow", row)
				}
				*/
			}
		}
		/*
		ctx.save()
		ctx.translate( , )
		ctx.scale(.9)
		ctx.restore();
		*/
	}
}
