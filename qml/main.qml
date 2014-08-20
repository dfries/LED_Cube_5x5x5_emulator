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
import QtQuick.Controls 1.1

Rectangle
{
	width: 640
	height: 480
	color: "black"

	property real margin: 5

	Text
	{
		color: "white"
		text: "LED Cube light display emulator"
	}
	Graph
	{
		id: graph
		anchors.left: parent.left
		anchors.top: parent.top
		anchors.right: parent.right
		height: parent.height - 50
		model: GUIMain.data
	}

	Row
	{
		anchors.left: parent.left
		anchors.top: graph.bottom
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.topMargin: margin*2
		spacing: margin

		Button
		{
			action: step
		}
	}
	Action
	{
		id: step
		text: "&Step"
		//shortcut: "Ctrl+S"
		onTriggered: GUIMain.StepPattern()
	}
}
